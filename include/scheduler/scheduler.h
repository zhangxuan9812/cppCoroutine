#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "fiber/fiber.h"
#include "hook/hook.h"
#include "thread/thread.h"

#include <mutex>
#include <vector>

namespace cppCoroutine {
/*
 * @brief Scheduler class, which is used to support the thread pool creation and management
 * @property m_name The name of the scheduler
 * @property m_mutex The mutex used to protect the scheduler
 * @property m_threads The thread pool
 * @property m_tasks The tasks
 * @property m_threadIds The thread ids of worker threads
 * @property m_threadCount The number of threads
 * @property m_activeThreadCount The active thread count
 * @property m_idleThreadCount The idle thread count
 * @property m_useCaller Whether to use the caller thread
 * @property m_schedulerFiber The scheduler fiber
 * @property m_rootThread The main thread id
 * @property m_stopping Whether the scheduler is stopping
 */
class Scheduler {
 public:
  /*
   * @brief Constructor of the Scheduler class
   * @param threads The number of threads
   * @param use_caller Whether to use the caller thread
   * @param name The name of the scheduler
   */
  Scheduler(size_t threads = 1, bool use_caller = true, const std::string &name = "Scheduler");
  /*
   * @brief Destructor of the Scheduler class
   */
  virtual ~Scheduler();
  /*
   * @brief Get the name of the scheduler
   * @return The name of the scheduler
   */
  const std::string &getName() const { return m_name; }

 public:
  /*
   * @brief Get the pointer of the current scheduler
   * @return The pointer of the current scheduler
   */
  static Scheduler *GetThis();

 protected:
  /*
   * @brief Set the current scheduler
   */
  void SetThis();

 public:
  /*
   * @brief Add a task to the scheduler
   * @param fc The fiber or callback
   * @param thread The thread id of the task
   */
  template <class FiberOrCb>
  void scheduleLock(FiberOrCb fc, int thread = -1) {
    bool need_tickle;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      // empty ->  all threads are idle -> need to be waken up
      need_tickle = m_tasks.empty();

      ScheduleTask task(fc, thread);
      if (task.fiber || task.cb) {
        m_tasks.push_back(task);
      }
    }

    if (need_tickle) {
      tickle();
    }
  }

  /*
   * @brief start the thread pool
   */
  virtual void start();
  /*
   * @brief stop the thread pool
   */
  virtual void stop();

 protected:
  /*
   * @brief wake up the scheduler
   */
  virtual void tickle();

  /*
   * @brief the main function of the thread
   */
  virtual void run();

  /*
   * @brief the idle function of the thread
   */
  virtual void idle();

  /*
   * @brief whether the scheduler is stopping
   */
  virtual bool stopping();
  /*
   * @brief whether there are idle threads
   */
  bool hasIdleThreads() { return m_idleThreadCount > 0; }

 private:
  // Task structure
  /*
   * @brief Task structure
   * @property fiber The fiber
   * @property cb The callback
   * @property thread The thread id of the task
   */
  struct ScheduleTask {
    std::shared_ptr<Fiber> fiber;
    std::function<void()> cb;
    // The thread id of the task
    int thread;
    /*
     * @brief Constructor of the ScheduleTask class
     */
    ScheduleTask() {
      fiber = nullptr;
      cb = nullptr;
      thread = -1;
    }
    /*
     * @brief Constructor of the ScheduleTask class
     * @param f The fiber
     * @param thr The thread id of the task
     */
    ScheduleTask(std::shared_ptr<Fiber> f, int thr) {
      fiber = f;
      thread = thr;
    }
    /*
     * @brief Constructor of the ScheduleTask class
     * @param f The pointer of the shared_ptr of the fiber
     * @param thr The thread id of the task
     */
    ScheduleTask(std::shared_ptr<Fiber> *f, int thr) {
      fiber.swap(*f);
      thread = thr;
    }
    /*
     * @brief Constructor of the ScheduleTask class
     * @param f The callback
     * @param thr The thread id of the task
     */
    ScheduleTask(std::function<void()> f, int thr) {
      cb = f;
      thread = thr;
    }
    /*
     * @brief Constructor of the ScheduleTask class
     * @param f The pointer of the callback
     * @param thr The thread id of the task
     */
    ScheduleTask(std::function<void()> *f, int thr) {
      cb.swap(*f);
      thread = thr;
    }
    /*
     * @brief Reset the task
     */
    void reset() {
      fiber = nullptr;
      cb = nullptr;
      thread = -1;
    }
  };

 private:
  // Name of the scheduler
  std::string m_name;
  std::mutex m_mutex;
  // Thread pool
  std::vector<std::shared_ptr<Thread>> m_threads;
  // Tasks
  std::vector<ScheduleTask> m_tasks;
  // Thread ids of worker threads
  std::vector<int> m_threadIds;
  // Number of threads
  size_t m_threadCount = 0;
  // Active thread count
  std::atomic<size_t> m_activeThreadCount = {0};
  // Idle thread count
  std::atomic<size_t> m_idleThreadCount = {0};

  // Whether to use the caller thread
  bool m_useCaller;
  // Scheduler fiber
  std::shared_ptr<Fiber> m_schedulerFiber;
  // Main thread id
  int m_rootThread = -1;
  // Whether the scheduler is stopping
  bool m_stopping = false;
};

}  // namespace cppCoroutine

#endif