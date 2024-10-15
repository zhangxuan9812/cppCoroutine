#ifndef __cppCoroutine_IOMANAGER_H__
#define __cppCoroutine_IOMANAGER_H__

#include "scheduler/scheduler.h"
#include "timer/timer.h"

namespace cppCoroutine {

// work flow
// 1 register one event -> 2 wait for it to ready -> 3 schedule the callback -> 4 unregister the event -> 5 run the
// callback
/*
 * @brief: IOManager class, which is used to support the IO event management
 * @property m_epfd The epoll file descriptor
 * @property m_tickleFds The tickle file descriptors
 * @property m_pendingEventCount The count of the pending events
 * @property m_mutex The mutex of the IOManager
 * @property m_fdContexts The vector of the fdcontexts
 */
class IOManager : public Scheduler, public TimerManager {
 public:
  /*
   * @brief Event enumeration
   * @enum NONE No event
   * @enum READ Read event
   * @enum WRITE Write event
   */
  enum Event { NONE = 0x0, READ = 0x1, WRITE = 0x4 };

 private:
  /*
   * @brief FdContext class, which is used to store the context of the file descriptor
   * @property read The read event context
   * @property write The write event context
   * @property fd The file descriptor
   * @property events The key of the epoll event
   * @property mutex The mutex of the FdContext
   */
  struct FdContext {
    /*
     * @brief EventContext class, which is used to store the context of the event
     * @property scheduler The scheduler
     * @property fiber The fiber
     * @property cb The callback function
     */
    struct EventContext {
      // Scheduler
      Scheduler *scheduler = nullptr;
      // Fiber
      std::shared_ptr<Fiber> fiber;
      // Callback function
      std::function<void()> cb;
    };

    // Read event context
    EventContext read;
    // Write event context
    EventContext write;
    // File descriptor
    int fd = 0;
    // The key of the epoll event
    Event events = NONE;
    std::mutex mutex;
    /*
     * @brief Get the event context
     * @param event the event type
     * @return the event context
     */
    EventContext &getEventContext(Event event);
    /*
     * @brief Reset the event context
     * @param ctx the event context
     */
    void resetEventContext(EventContext &ctx);
    /*
     * @brief Trigger the event
     * @param event the event type
     */
    void triggerEvent(Event event);
  };

 public:
  /*
   * @brief Construct a new IOManager object
   * @param threads the number of threads
   * @param use_caller whether to use the caller thread
   * @param name the name of the IOManager
   */
  IOManager(size_t threads = 1, bool use_caller = true, const std::string &name = "IOManager");
  /*
   * @brief Destructor of the IOManager class
   */
  ~IOManager();

  /*
   * @brief Add an event to the IOManager
   * @param fd the file descriptor
   * @param event the event type
   * @param cb the callback function
   * @return the result
   */
  int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
  /*
   * @brief Delete an event from the IOManager
   * @param fd the file descriptor
   * @param event the event type
   * @return whether the event is deleted successfully
   */
  bool delEvent(int fd, Event event);
  /*
   * @brief Delete and trigger the event
   * @param fd the file descriptor
   * @param event the event type
   * @return whether the event is canceled successfully
   */
  bool cancelEvent(int fd, Event event);
  /*
   * @brief Delete all events and trigger them
   * @param fd the file descriptor
   * @return whether all events are canceled successfully
   */
  bool cancelAll(int fd);
  /*
   * @brief Get the pointer of the current IOManager
   * @return the pointer of the current IOManager
   */
  static IOManager *GetThis();

 protected:
  void tickle() override;
  bool stopping() override;

  void idle() override;

  void onTimerInsertedAtFront() override;
  /*
   * @brief Resize the size of the vector that stores the fdcontexts
   * @param size the new size of the vector
   */
  void contextResize(size_t size);

 private:
  // The epoll file descriptor
  int m_epfd = 0;
  // m_tickleFds[0] is used to read, m_tickleFds[1] is used to write
  int m_tickleFds[2];
  std::atomic<size_t> m_pendingEventCount = {0};
  std::shared_mutex m_mutex;
  // Store fdcontexts for each fd
  std::vector<FdContext *> m_fdContexts;
};

}  // end namespace cppCoroutine

#endif