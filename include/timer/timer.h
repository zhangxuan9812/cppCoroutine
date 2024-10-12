#ifndef __cppCoroutine_TIMER_H__
#define __cppCoroutine_TIMER_H__

#include <assert.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <vector>
namespace cppCoroutine {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer> {
  friend class TimerManager;

 public:
  /*
   * @brief Delete the timer
   * @return true if the timer is deleted successfully, otherwise false
   */
  bool cancel();
  /*
   * @brief Refresh the timer
   * @return true if the timer is refreshed successfully, otherwise false
   */
  bool refresh();
  /*
   * @brief Reset the timer
   * @param ms the new timeout time
   * @param from_now whether the new timeout time is from now
   * @return true if the timer is reset successfully, otherwise false
   */
  bool reset(uint64_t ms, bool from_now);

 private:
  /*
   * @brief Construct a new Timer object
   * @param[in] ms the timeout time
   * @param[in] cb the callback function
   * @param[in] recurring whether the timer is recurring
   * @param[in] manager the manager of this timer
   */
  Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager);

 private:
  // Whether the timer is recurring
  bool m_recurring = false;
  // Timeout time
  uint64_t m_ms = 0;
  // The absolute time of timeout
  std::chrono::time_point<std::chrono::system_clock> m_next;
  // The callback function
  std::function<void()> m_cb;
  // The manager of this timer
  TimerManager *m_manager = nullptr;

 private:
  // The comparator of the timer
  struct Comparator {
    /*
     * @brief Compare two timers
     * @param lhs the left timer
     * @param rhs the right timer
     * @return true if the left timer is less than the right timer, otherwise false
     */
    bool operator()(const std::shared_ptr<Timer> &lhs, const std::shared_ptr<Timer> &rhs) const;
  };
};

/*
 * @brief The timer manager
 * @details The timer manager is used to manage the timers
 */
class TimerManager {
  friend class Timer;

 public:
  TimerManager();
  virtual ~TimerManager();

  /*
   * @brief Add a timer to the timer manager
   * @param ms the timeout time
   * @param cb the callback function
   * @param recurring whether the timer is recurring
   * @return the timer that has been added
   */
  std::shared_ptr<Timer> addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

  /*
   * @brief Add a conditional timer to the timer manager
   * @param ms the timeout time
   * @param cb the callback function
   * @param weak_cond the condition of the timer
   * @param recurring whether the timer is recurring
   * @return the timer that has been added
   */
  std::shared_ptr<Timer> addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond,
                                           bool recurring = false);

  /*
   * @brief Get the next timer
   * @return the timeout time of the next timer
   */
  uint64_t getNextTimer();

  /*
   * @brief List the expired timers
   * @param cbs the vector of the callback functions
   */
  void listExpiredCb(std::vector<std::function<void()>> &cbs);

  /*
   * @brief Detect whether the timer manager has a timer
   * @return true if the timer manager has a timer, otherwise false
   */
  bool hasTimer();

 protected:
  /*
   * @brief Add a timer to the front of the timer manager
   * @brief The function is a virtual function, which can be overridden by the subclass
   */
  virtual void onTimerInsertedAtFront() {};

  /*
   * @brief Add a timer to the timer manager called in the multiple parameters addTimer() fuction
   * @param timer the timer to be added
   */
  void addTimer(std::shared_ptr<Timer> timer);

 private:
  /*
   * @brief Detect whether the clock has rolled over
   * @return true if the clock has rolled over, otherwise false
   */
  bool detectClockRollover();

 private:
  // The mutex of the timer manager
  // The mutex is a shared mutex, which can be shared by multiple threads
  std::shared_mutex m_mutex;
  // A set of timers that are sorted by the timeout time
  std::set<std::shared_ptr<Timer>, Timer::Comparator> m_timers;
  // Whether the timer has been triggered
  bool m_tickled = false;
  // The previous time
  std::chrono::time_point<std::chrono::system_clock> m_previouseTime;
};

}  // namespace cppCoroutine

#endif