#include <fmt/core.h>
#include <fmt/ostream.h>
#include <unistd.h>
#include <iostream>
#include "timer/timer.h"
using namespace cppCoroutine;
// Callback function
void func(int i) { fmt::print("func {} is called\n", i); }

int main(int argc, char const *argv[]) {
  // Create the timer manager
  std::shared_ptr<TimerManager> manager(new TimerManager());
  // Callback function vector
  std::vector<std::function<void()>> cbs;
  // Test the function of listExpiredCb
  {
    // Add 10 timers
    for (int i = 0; i < 10; i++) {
      manager->addTimer((i + 1) * 1000, std::bind(&func, i), false);
    }
    fmt::print("All timers have been set up\n");

    sleep(5);
    manager->listExpiredCb(cbs);
    // Execute the callback function
    while (!cbs.empty()) {
      std::function<void()> cb = *cbs.begin();
      cbs.erase(cbs.begin());
      cb();
    }
    fmt::print("--------------------\n");
    sleep(5);
    manager->listExpiredCb(cbs);
    if (cbs.empty()) {
      fmt::print("No timer has expired\n");
    }
    while (!cbs.empty()) {
      std::function<void()> cb = *cbs.begin();
      cbs.erase(cbs.begin());
      cb();
    }
  }

  // Test the function of addTimer by setting the timer to be recurring
  {
    // Add a timer that is recurring and the timeout time is 1000ms
    manager->addTimer(1000, std::bind(&func, 1000), true);
    int j = 10;
    while (j-- > 0) {
      sleep(1);
      manager->listExpiredCb(cbs);
      std::function<void()> cb = *cbs.begin();
      cbs.erase(cbs.begin());
      cb();
    }
  }
  return 0;
}