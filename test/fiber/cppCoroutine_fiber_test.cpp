#include "fiber/fiber.h"
#include <vector>
#include <fmt/core.h>
#include <fmt/ostream.h>

using namespace cppCoroutine; 

class Scheduler
{
public:
	void schedule(std::shared_ptr<Fiber> task)
	{
		m_tasks.push_back(task);
	}

	
	//Execute the tasks
	void run()
	{
		//fmt::print(" number {0}\n", m_tasks.size());
		std::shared_ptr<Fiber> task;
		auto it = m_tasks.begin();
		while(it != m_tasks.end())
		{
			//fmt::print(" number {0}\n", it - m_tasks.begin());
			task = *it;
			//Switch the fiber from the main fiber to the task fiber, when the task fiber finishes, switch back to the main fiber automatically
			
			task->resume();
			//fmt::print("Return from resume()\n");
			it++;
		}
		//Clear the tasks
		m_tasks.clear();
	}

private:
	//Store the tasks
	std::vector<std::shared_ptr<Fiber>> m_tasks;
};

void test_fiber(int i)
{
	fmt::print("hello world {0}\n", i);
}

int main()
{
	//Initialize the main fiber
	Fiber::GetThis();
	//Create the scheduler
	Scheduler sc;
	for(auto i = 0; i < 20; i++)
	{
		//Create sub-fiber
		std::shared_ptr<Fiber> fiber = std::make_shared<Fiber>(std::bind(test_fiber, i), 0, false);
		sc.schedule(fiber);
	}
	//Execute the tasks
	sc.run();
	return 0;
}