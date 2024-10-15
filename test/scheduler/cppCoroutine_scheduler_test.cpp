#include <fmt/core.h>
#include <fmt/ostream.h>
#include "scheduler/scheduler.h"

using namespace cppCoroutine;

static unsigned int test_number;
std::mutex mutex_cout;
void task() {
  {
    std::lock_guard<std::mutex> lock(mutex_cout);
    fmt::print("task {0} is under processing in thread: {1}\n", test_number, Thread::GetThreadId());
  }
  sleep(1);
}

int main(int argc, char const *argv[]) {
  {
    // Create the scheduler
    // If the second parameter is true, the main thread will be added to the scheduler
    // Otherwise, the main thread will not be added to the scheduler
    std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>(3, true, "scheduler_1");

    scheduler->start();

    sleep(2);

    std::cout << "\nbegin post\n\n";
    for (int i = 0; i < 5; i++) {
      std::shared_ptr<Fiber> fiber = std::make_shared<Fiber>(task);
      scheduler->scheduleLock(fiber);
    }

    sleep(6);

    std::cout << "\npost again\n\n";
    for (int i = 0; i < 15; i++) {
      std::shared_ptr<Fiber> fiber = std::make_shared<Fiber>(task);
      scheduler->scheduleLock(fiber);
    }

    sleep(3);
    // scheduler如果有设置将加入工作处理
    scheduler->stop();
  }
  return 0;
}