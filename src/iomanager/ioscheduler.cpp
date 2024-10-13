#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

#include "iomanager/ioscheduler.h"

static bool debug = true;

namespace cppCoroutine {

IOManager *IOManager::GetThis() { return dynamic_cast<IOManager *>(Scheduler::GetThis()); }

IOManager::FdContext::EventContext &IOManager::FdContext::getEventContext(Event event) {
  if (event != READ && event != WRITE) {
    throw std::invalid_argument("Unsupported event type");
  }
  switch (event) {
    case READ:
      return read;
    case WRITE:
      return write;
    default:
      throw std::logic_error("Unexpected event type");  // Fallback for safety, though it shouldn't reach here
  }
}

void IOManager::FdContext::resetEventContext(EventContext &ctx) {
  ctx.scheduler = nullptr;
  ctx.fiber.reset();
  ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event) {
  // Test whether the event is in the event set
  assert(events & event);
  // Delete the event
  events = static_cast<Event>(events & ~event);

  // Get the event context
  EventContext &ctx = getEventContext(event);
  // Add the callback function to the scheduler
  if (ctx.cb) {
    // Call ScheduleTask(std::function<void()>* f, int thr)
    ctx.scheduler->scheduleLock(&ctx.cb);
  } else {
    // call ScheduleTask(std::shared_ptr<Fiber>* f, int thr)
    ctx.scheduler->scheduleLock(&ctx.fiber);
  }

  // Reset the event context
  resetEventContext(ctx);
  return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string &name)
    : Scheduler(threads, use_caller, name), TimerManager() {
  // Create the epoll file descriptor
  m_epfd = epoll_create(5000);
  assert(m_epfd > 0);

  // Create pipe
  int rt = pipe(m_tickleFds);
  assert(!rt);

  // Add the read event to the epoll
  epoll_event event;
  // EPOLLIN: The associated file is available for read(2) operations.
  // EPOLLET: Sets the Edge Triggered behavior for the associated file descriptor.
  event.events = EPOLLIN | EPOLLET;
  // Event.data is a union and event.data.fd is the file descriptor
  event.data.fd = m_tickleFds[0];

  // Set the file descriptor to non-blocking
  rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
  assert(!rt);
  // EPOLL_CTL_ADD: Add a file descriptor to the interface.
  // Add the m_tickleFds[0] to the epoll instance
  rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
  assert(!rt);
  // Resize the context to the size of 32
  contextResize(32);

  start();
}

IOManager::~IOManager() {
  stop();
  // Close the epoll file descriptor
  close(m_epfd);
  // Close the pipe
  close(m_tickleFds[0]);
  close(m_tickleFds[1]);

  for (auto &fd_ctx : m_fdContexts) {
    if (fd_ctx) {
      delete fd_ctx;
    }
  }
}

void IOManager::contextResize(size_t size) {
  m_fdContexts.resize(size);
  for (size_t i = 0; i < m_fdContexts.size(); ++i) {
    if (m_fdContexts[i] == nullptr) {
      // Create a new FdContext
      m_fdContexts[i] = new FdContext();
      // Set the file descriptor
      m_fdContexts[i]->fd = i;
    }
  }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
  FdContext *fd_ctx = nullptr;
  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  // If the fd is in the range of the vector, get the FdContext
  if (static_cast<int>(m_fdContexts.size()) > fd) {
    fd_ctx = m_fdContexts[fd];
    read_lock.unlock();
  } else {  // If the fd is not in the range of the vector, resize the vector
    read_lock.unlock();
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    contextResize(fd * 1.5);  // Resize the vector to 1.5 times the size of the fd
    fd_ctx = m_fdContexts[fd];
  }

  std::lock_guard<std::mutex> lock(fd_ctx->mutex);

  // We don't allow to add the same event to the same fd multiple times
  //  The event already exists
  if (fd_ctx->events & event) {
    return -1;
  }

  // Add the event to the epoll, if the event is in the events set, modify the event, otherwise add the event
  // EPOLL_CTL_MOD means that the operation is to modify the file descriptor
  // EPOLL_CTL_ADD means that the operation is to add the file descriptor
  int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
  epoll_event epevent;
  // EPOLLET: Sets the Edge Triggered behavior for the associated file descriptor.
  // Set the event to the events set by using the edge-triggered mode
  epevent.events = static_cast<int>(EPOLLET) | static_cast<int>(fd_ctx->events) | static_cast<int>(event);
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  // If the epoll_ctl fails, print the error message
  if (rt) {
    fmt::print("addEvent::epoll_ctl failed: {0}\n", strerror(errno));
    return -1;
  }
  // Increase the number of pending events
  ++m_pendingEventCount;

  // Update the events set
  fd_ctx->events = (Event)(fd_ctx->events | event);

  // Update the event context
  FdContext::EventContext &event_ctx = fd_ctx->getEventContext(event);
  // Test whether the scheduler and fiber and callback function are all empty
  assert(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
  // Set the the scheduler of the event context
  event_ctx.scheduler = Scheduler::GetThis();
  if (cb) {
    // event_ctx.cb.swap(cb);
    event_ctx.cb = std::move(cb);
  } else {
    // Set the fiber of the event context
    event_ctx.fiber = Fiber::GetThis();
    assert(event_ctx.fiber->getState() == Fiber::RUNNING);
  }
  return 0;
}

bool IOManager::delEvent(int fd, Event event) {
  FdContext *fd_ctx = nullptr;
  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  if ((int)m_fdContexts.size() > fd) {
    fd_ctx = m_fdContexts[fd];
    read_lock.unlock();
  } else {  // If the fd is not in the range of the vector, return false because the event doesn't exist
    read_lock.unlock();
    return false;
  }

  std::lock_guard<std::mutex> lock(fd_ctx->mutex);

  // If the event doesn't exist, return false
  if (!(fd_ctx->events & event)) {
    return false;
  }

  // Delete the event
  Event new_events = static_cast<Event>(fd_ctx->events & ~event);
  // If the new_events is not empty, modify the event, otherwise delete the event
  // EPOLL_CTL_MOD means that the operation is to modify the file descriptor
  // EPOLL_CTL_DEL means that the operation is to delete the file descriptor
  int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
  epoll_event epevent;
  epevent.events = static_cast<int>(EPOLLET) | static_cast<int>(new_events);
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if (rt) {
    fmt::print("delEvent::epoll_ctl failed: {0}\n", strerror(errno));
    return false;  // If the epoll_ctl fails, return false
  }
  // Decrease the number of pending events
  --m_pendingEventCount;

  // Update fdcontext
  fd_ctx->events = new_events;

  // Update event context
  FdContext::EventContext &event_ctx = fd_ctx->getEventContext(event);
  fd_ctx->resetEventContext(event_ctx);
  return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
  FdContext *fd_ctx = nullptr;
  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  if ((int)m_fdContexts.size() > fd) {
    fd_ctx = m_fdContexts[fd];
    read_lock.unlock();
  } else {  // If the fd is not in the range of the vector, return false because the event doesn't exist
    read_lock.unlock();
    return false;
  }

  std::lock_guard<std::mutex> lock(fd_ctx->mutex);

  // The event doesn't exist
  if (!(fd_ctx->events & event)) {
    return false;
  }

  // Delete the event
  Event new_events = (Event)(fd_ctx->events & ~event);
  int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
  epoll_event epevent;
  epevent.events = static_cast<int>(EPOLLET) | static_cast<int>(new_events);
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if (rt) {
    fmt::print("cancelEvent::epoll_ctl failed: {0}\n", strerror(errno));
    return false;
  }
  // Decrease the number of pending events
  --m_pendingEventCount;

  // Update fdcontext, event context and trigger
  fd_ctx->triggerEvent(event);
  return true;
}

bool IOManager::cancelAll(int fd) {
  FdContext *fd_ctx = nullptr;

  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  if ((int)m_fdContexts.size() > fd) {
    fd_ctx = m_fdContexts[fd];
    read_lock.unlock();
  } else {
    read_lock.unlock();
    return false;
  }

  std::lock_guard<std::mutex> lock(fd_ctx->mutex);

  // No events exist
  if (!fd_ctx->events) {
    return false;
  }

  // Delete all events
  int op = EPOLL_CTL_DEL;
  epoll_event epevent;
  epevent.events = 0;
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if (rt) {
    fmt::print("cancelAll::epoll_ctl failed: {0}\n", strerror(errno));
    return false;
  }

  // Update fdcontext, event context and trigger
  if (fd_ctx->events & READ) {
    fd_ctx->triggerEvent(READ);
    --m_pendingEventCount;
  }

  if (fd_ctx->events & WRITE) {
    fd_ctx->triggerEvent(WRITE);
    --m_pendingEventCount;
  }

  assert(fd_ctx->events == 0);
  return true;
}

void IOManager::tickle() {
  // If there are no idle threads, return
  if (!hasIdleThreads()) {
    return;
  }
  int rt = write(m_tickleFds[1], "T", 1);
  assert(rt == 1);
}

bool IOManager::stopping() {
  uint64_t timeout = getNextTimer();
  // No timers left and no pending events left with the Scheduler::stopping()
  return timeout == ~0ull && m_pendingEventCount == 0 && Scheduler::stopping();
}

void IOManager::idle() {
  // The maximum number of events could be dealt with is 256
  static const uint64_t MAX_EVENTS = 256;
  std::unique_ptr<epoll_event[]> events(new epoll_event[MAX_EVENTS]);

  while (true) {
    if (debug) {
      fmt::print("IOManager::idle(),run in thread:{}\n", Thread::GetThreadId());
    }

    if (stopping()) {
      if (debug) {
        fmt::print("name = {0} idle exits in thread: {1}\n", getName(), Thread::GetThreadId());
      }
      break;
    }

    // Blocked at epoll_wait
    int rt = 0;
    while (true) {
      static const uint64_t MAX_TIMEOUT = 5000;
      uint64_t next_timeout = getNextTimer();
      next_timeout = std::min(next_timeout, MAX_TIMEOUT);
      // Epoll_wait: wait for events on an epoll instance "epfd". Returns the number of triggered events returned in
      // "events" buffer. Or -1 in case of error with the "errno" variable set to the specific error code. The "events"
      // parameter is a buffer that will contain triggered events. The "maxevents" is the maximum number of events to be
      // returned ( usually size of "events" ). The "timeout" parameter specifies the maximum wait time in milliseconds
      // (-1 == infinite).
      rt = epoll_wait(m_epfd, events.get(), MAX_EVENTS, (int)next_timeout);
      // EINTR -> retry

      if (rt < 0 && errno == EINTR) {
        continue;
      } else {
        break;
      }
    }

    // Collect all timers overdue
    std::vector<std::function<void()>> cbs;
    listExpiredCb(cbs);
    if (!cbs.empty()) {
      // Add the callback functions to the scheduler
      for (const auto &cb : cbs) {
        scheduleLock(cb);
      }
      cbs.clear();
    }

    // Collect all events ready
    for (int i = 0; i < rt; ++i) {
      epoll_event &event = events[i];

      // If the file descriptor is the m_tickleFds[0]
      if (event.data.fd == m_tickleFds[0]) {
        uint8_t dummy[256];
        // Read the data from the pipe
        while (read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
        continue;
      }

      // Get the FdContext
      FdContext *fd_ctx = (FdContext *)event.data.ptr;
      std::lock_guard<std::mutex> lock(fd_ctx->mutex);

      // Convert EPOLLERR and EPOLLHUP to EPOLLIN and EPOLLOUT
      if (event.events & (EPOLLERR | EPOLLHUP)) {
        event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
      }
      // Events happening during this turn of epoll_wait
      int real_events = NONE;
      if (event.events & EPOLLIN) {
        real_events |= READ;
      }
      if (event.events & EPOLLOUT) {
        real_events |= WRITE;
      }
      // If the event is not in the events set, continue
      if ((fd_ctx->events & real_events) == NONE) {
        continue;
      }

      // Delete the events that have already happened
      // left_events means that the events that have not happened
      int left_events = (fd_ctx->events & ~real_events);
      int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
      event.events = EPOLLET | left_events;
      // Modify the epoll instance by using the EPOLL_CTL_MOD to deal with the events that have not happened
      int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
      if (rt2) {
        fmt::print("idle::epoll_ctl failed: {0}\n", strerror(errno));
        continue;
      }

      // Trigger the events that have happened
      if (real_events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
      }
      if (real_events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
      }
    }
    // Release the CPU
    Fiber::GetThis()->yield();
  }
}

void IOManager::onTimerInsertedAtFront() { tickle(); }

}  // namespace cppCoroutine