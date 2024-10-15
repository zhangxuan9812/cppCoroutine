#ifndef _THREAD_H_
#define _THREAD_H_

#include <condition_variable>
#include <functional>
#include <mutex>

namespace cppCoroutine {
/*
 * @brief Semaphore class, which is used to support the thread synchronization mechanism
 * @property count The count of the semaphore
 * @property mtx The mutex used to protect the count
 * @property cv The condition variable used to block the thread
 */
class Semaphore {
 private:
  std::mutex mtx;
  std::condition_variable cv;
  int count;

 public:
  /*
   * @brief Constructor
   * @note Initialize the semaphore with a count of 0
   * @param count_ The count of the semaphore
   */
  explicit Semaphore(int count_ = 0) : count(count_) {}

  /*
   * @brief P operation
   * @note If the count is 0, the current thread will be blocked until the count is greater than 0
   * @note If the count is greater than 0, the count will be decremented by 1
   * @note If the count is less than 0, the count will not be decremented
   */
  void wait() {
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0) {
      // Block the current thread
      cv.wait(lock);
    }
    count--;
  }

  /*
   * @brief V operation
   * @note If there are threads blocked on the semaphore, one of them will be woken up
   * @note If the count is less than 0, the count will be incremented by 1
   */
  void signal() {
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    // Notify one of the waiting threads
    cv.notify_one();
  }
};

/*
 * @brief Thread class, which is used to support the thread creation and management
 * @property m_id The process id of the thread
 * @property m_thread The thread object
 * @property m_cb The function that the thread needs to run
 * @property m_name The name of the thread
 * @property m_semaphore The semaphore used to synchronize the thread
 */
class Thread {
 public:
  /*
   * @brief Constructor of the Thread class
   * @param cb the function pointer of the thread function
   * @param name the name of the thread
   */
  Thread(std::function<void()> cb, const std::string &name);
  /*
   * @brief Destructor of the Thread class
   */
  ~Thread();
  /*
   * @brief Get the process ID of the thread
   * @return The process ID of the thread
   */
  pid_t getId() const { return m_id; }
  /*
   * @brief Get the name of the thread
   * @return The name of the thread
   */
  const std::string &getName() const { return m_name; }
  /*
   * @brief Wait for the thread to finish
   * @note If the thread is not finished, the current thread will be blocked
   * @note If the thread is finished, the function will return immediately
   */
  void join();

 public:
  /*
   * @brief Get the thread ID of the current thread
   * @return The thread ID of the current thread
   */
  static pid_t GetThreadId();
  /*
   * @brief Get the pointer of the current thread
   * @return The pointer of the current thread
   */
  static Thread *GetThis();
  /*
   * @brief Get the name of the current thread
   * @return The name of the current thread
   */
  static const std::string &GetName();

  /*
   * @brief Set the name of the current thread
   * @param name The name of the current thread
   */
  static void SetName(const std::string &name);

 private:
  /*
   * @brief The function that the thread needs to run
   * @param arg The pointer of the thread object
   */
  static void *run(void *arg);

 private:
  // The thread id of the current thread
  pid_t m_id = -1;
  pthread_t m_thread = 0;
  // The function that the thread needs to run
  std::function<void()> m_cb;
  // The name of the thread
  std::string m_name;
  // The semaphore used to synchronize the thread
  Semaphore m_semaphore;
};
}  // namespace cppCoroutine

#endif
