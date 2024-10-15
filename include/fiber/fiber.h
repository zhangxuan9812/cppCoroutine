#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <atomic>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#ifdef __APPLE__
#include <boost/context/detail/fcontext.hpp>
#else
#ifndef _XOPEN_SOURCE
// In order to use ucontext.h, you need to define _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <ucontext.h>
#endif
#include <unistd.h>
#include <mutex>

namespace cppCoroutine {
/*
 * @brief Fiber class, which is used to support the coroutine creation
 * @property m_id The id of the fiber
 * @property m_stacksize The size of the stack
 * @property m_state The state of the fiber
 * @property m_ctx The context of the fiber
 * @property m_stack The pointer of the stack
 * @property m_cb The function of the fiber
 * @property m_runInScheduler Whether the fiber runs in the scheduler
 * @property m_mutex The mutex used to protect the fiber
 * @property m_name The name of the fiber
 */
class Fiber : public std::enable_shared_from_this<Fiber> {
 public:
  /*
   * @brief The state of the fiber
   * @enum READY The fiber is ready to run
   * @enum RUNNING The fiber is running
   * @enum TERM The fiber is terminated
   */
  enum State { READY, RUNNING, TERM };

 private:
  /*
   * @brief Constructor of the Fiber class
   * @note The constructor is private, which is only used by GetThis()
   */
  Fiber();

 public:
  /*
   * @brief Constructor of the Fiber class
   * @param cb The function that the fiber needs to run
   * @param stacksize The size of the stack
   * @param run_in_scheduler Whether the fiber runs in the scheduler
   */
  Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = true);
  /*
   * @brief Destructor of the Fiber class
   */
  ~Fiber();
  /*
   * @brief Reset the fiber
   * @param cb The function that the fiber needs to run
   */
  void reset(std::function<void()> cb);
  /*
   * @brief Resume the fiber
   */
  void resume();
  /*
   * @brief Yield the fiber
   */
  void yield();
  /*
   * @brief Get the id of the fiber
   * @return The id of the fiber
   */
  uint64_t getId() const { return m_id; }
  /*
   * @brief Get the state of the fiber
   * @return The state of the fiber
   */
  State getState() const { return m_state; }

 public:
  /*
   * @brief Set the current fiber
   * @param f The fiber that needs to be set
   */
  static void SetThis(Fiber *f);

  /*
   * @brief Get the current fiber
   * @return The shared pointer of the current fiber
   */
  static std::shared_ptr<Fiber> GetThis();

  /*
   * @brief Get the main fiber of the current thread
   * @return The shared pointer of the main fiber
   */
  static void SetSchedulerFiber(Fiber *f);

  /*
   * @brief Get the id of the current fiber
   * @return The id of the current fiber
   */
  static uint64_t GetFiberId();
#ifdef __APPLE__
  // Main function of the fiber
  static void MainFunc(boost::context::detail::transfer_t t);

  static void empty_coroutine_function(boost::context::detail::transfer_t t) {}
#else
  /*
   * @brief Main function of the fiber
   */
  static void MainFunc();
#endif

 public:
  // id
  uint64_t m_id = 0;
  // the size of the stack
  uint32_t m_stacksize = 0;
  // the state of the fiber
  State m_state = READY;
// the context of the fiber
#ifdef __APPLE__
  boost::context::detail::fcontext_t m_ctx = nullptr;
#else
  ucontext_t m_ctx;
#endif

  // the pointer of the stack
  void *m_stack = nullptr;
  // the function of the fiber
  std::function<void()> m_cb;
  // whether the fiber runs in the scheduler
  bool m_runInScheduler = false;

 public:
  std::mutex m_mutex;
  std::string m_name;
};
}  // namespace cppCoroutine

#endif
