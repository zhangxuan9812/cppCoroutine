#include "timer/timer.h"

namespace cppCoroutine {

bool Timer::cancel() {
  std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);
  // If the callback function is nullptr, the timer has been deleted
  if (m_cb == nullptr) {
    return false;
  } else {
    m_cb = nullptr;
  }
  // Find the timer and delete it
  auto it = m_manager->m_timers.find(shared_from_this());
  if (it != m_manager->m_timers.end()) {
    m_manager->m_timers.erase(it);
  }
  return true;
}

bool Timer::refresh() {
  std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);
  // If the callback function is nullptr, the timer has been deleted
  // We cannot refresh the timer
  if (!m_cb) {
    return false;
  }
  // Find the timer and delete it
  auto it = m_manager->m_timers.find(shared_from_this());
  if (it == m_manager->m_timers.end()) {
    return false;
  }
  m_manager->m_timers.erase(it);
  // Reset the absolute time of timeout
  m_next = std::chrono::system_clock::now() + std::chrono::milliseconds(m_ms);
  m_manager->m_timers.insert(shared_from_this());
  return true;
}

bool Timer::reset(uint64_t ms, bool from_now) {
  // If the timeout time is the same as the new timeout time and the new timeout time is not from now
  if (ms == m_ms && !from_now) {
    return true;
  }

  {
    std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);
    // If the callback function is nullptr, the timer has been deleted
    if (!m_cb) {
      return false;
    }
    // Find the timer and delete it
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
      return false;
    }
    m_manager->m_timers.erase(it);
  }

  // Reinstate the timer
  auto start = from_now ? std::chrono::system_clock::now() : m_next - std::chrono::milliseconds(m_ms);
  // Reset the timeout time
  m_ms = ms;
  // Reset the absolute time of timeout
  m_next = start + std::chrono::milliseconds(m_ms);
  // Add the timer to the timer manager
  m_manager->addTimer(shared_from_this());
  return true;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager)
    : m_recurring(recurring), m_ms(ms), m_cb(cb), m_manager(manager) {
  auto now = std::chrono::system_clock::now();
  m_next = now + std::chrono::milliseconds(m_ms);
}

bool Timer::Comparator::operator()(const std::shared_ptr<Timer> &lhs, const std::shared_ptr<Timer> &rhs) const {
  // We sort the timers by the absolute time of timeout
  assert(lhs != nullptr && rhs != nullptr);
  return lhs->m_next < rhs->m_next;
}

TimerManager::TimerManager() { m_previouseTime = std::chrono::system_clock::now(); }

TimerManager::~TimerManager() {}

std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
  std::shared_ptr<Timer> timer(new Timer(ms, cb, recurring, this));
  addTimer(timer);
  return timer;
}

/*
 * @brief If the condition is valid, execute the callback function
 * @param weak_cond the weak pointer of the condition
 * @param cb the callback function
 */
static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
  std::shared_ptr<void> tmp = weak_cond.lock();
  if (tmp) {
    cb();
  }
}

std::shared_ptr<Timer> TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb,
                                                       std::weak_ptr<void> weak_cond, bool recurring) {
  return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  // reset m_tickled
  m_tickled = false;
  if (m_timers.empty()) {
    // If there is no timer, return the maximum value of uint64_t
    return ~0ull;
  }

  auto now = std::chrono::system_clock::now();
  auto time = (*m_timers.begin())->m_next;
  // if now >= time, which there are some timer expired
  if (now >= time) {
    return 0;
  } else {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time - now);
    return static_cast<uint64_t>(duration.count());
  }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs) {
  // Get the current time
  auto now = std::chrono::system_clock::now();
  // Lock the mutex
  std::unique_lock<std::shared_mutex> write_lock(m_mutex);
  // Detect whether the clock has rolled over
  bool rollover = detectClockRollover();
  // If the clock has rolled over, clear all timers
  // If the timer has expired, clear the timeout timer
  while ((!m_timers.empty() && rollover) || (!m_timers.empty() && (*m_timers.begin())->m_next <= now)) {
    std::shared_ptr<Timer> temp = *m_timers.begin();
    m_timers.erase(m_timers.begin());
    // Add the callback function to the vector
    cbs.push_back(temp->m_cb);
    // If the timer is recurring, reset the timer
    if (temp->m_recurring) {
      // Reset the timer by the timeout time
      temp->m_next = now + std::chrono::milliseconds(temp->m_ms);
      // Insert the timer
      m_timers.insert(temp);
    } else {
      // If the timer is not recurring, set the callback function to nullptr
      // This means that the timer is deleted
      temp->m_cb = nullptr;
    }
  }
}

bool TimerManager::hasTimer() {
  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  return !m_timers.empty();
}

// lock + tickle()
void TimerManager::addTimer(std::shared_ptr<Timer> timer) {
  bool at_front = false;
  {
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    auto it = m_timers.insert(timer).first;
    at_front = (it == m_timers.begin()) && !m_tickled;

    // only tickle once till one thread wakes up and runs getNextTime()
    if (at_front) {
      m_tickled = true;
    }
  }
  if (at_front) {
    // Add the timer to the front of the timer list
    onTimerInsertedAtFront();
  }
}

bool TimerManager::detectClockRollover() {
  bool rollover = false;
  // Get the current time
  auto now = std::chrono::system_clock::now();
  // Detect whether the clock has rolled over
  // If the time difference is greater than 1 hour, it is considered to be a clock rollover
  if (now < (m_previouseTime - std::chrono::milliseconds(60 * 60 * 1000))) {
    rollover = true;
  }
  m_previouseTime = now;
  return rollover;
}

}  // namespace cppCoroutine
