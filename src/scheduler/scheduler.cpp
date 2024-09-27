#include "scheduler/scheduler.h"
#include "thread/thread.h"
#include <fmt/core.h>
#include <fmt/ostream.h>

static bool debug = true;
namespace cppCoroutine {
//t_scheduler is a thread_local variable, which is used to store the pointer of the current scheduler
static thread_local Scheduler* t_scheduler = nullptr;


/*
* @brief: Get the pointer of the current scheduler
*/
Scheduler* Scheduler::GetThis()
{
	return t_scheduler;
}


/*
* @brief: Set the current scheduler
*/
void Scheduler::SetThis()
{
	t_scheduler = this;
}


/*
* @brief: Constructor of the Scheduler class
* @param: threads, the number of threads in the thread pool
* @param: use_caller, whether to use the caller thread as the worker thread
* @param: name, the name of the scheduler
*/
Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name):
m_name(name), m_useCaller(use_caller)
{
	assert(threads > 0 && Scheduler::GetThis() == nullptr);
	SetThis();
	//Set the name of the current thread
	Thread::SetName(m_name);
	//If use_caller is true, the caller thread will be used as the worker thread
	if(use_caller)
	{
		threads--;
		//Initialize the main fiber
		Fiber::GetThis();
		//Create the scheduler fiber
		m_schedulerFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));
		//Set the scheduler fiber
		Fiber::SetSchedulerFiber(m_schedulerFiber.get());
		//Get the thread id of the current thread
		m_rootThread = Thread::GetThreadId();
		m_threadIds.push_back(m_rootThread);
	}
	//Set the number of threads that the thread pool needs to create
	m_threadCount = threads;
	if (debug) {
		fmt::print("Scheduler::Scheduler() success\n");
	}
}


/*
* @brief: Destructor of the Scheduler class
*/
Scheduler::~Scheduler()
{
	assert(stopping() == true);
	if (GetThis() == this) 
	{
        t_scheduler = nullptr;
    }
	if (debug) {
		fmt::print("Scheduler::~Scheduler() success\n");
	}
}


/*
* @brief: Start the thread pool
*/
void Scheduler::start()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	//If the scheduler is stopping, return directly
	if(m_stopping)
	{
		fmt::print(std::cerr, "Scheduler is stopped\n");
		return;
	}
	assert(m_threads.empty());
	//Create the worker threads
	m_threads.resize(m_threadCount);
	for (size_t i = 0; i < m_threadCount; i++)
	{
		fmt::print("create thread {0}\n", i);
		m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
		m_threadIds.push_back(m_threads[i]->getId());
		fmt::print("create thread {0} success\n", i);
	}
	if (debug) {
		fmt::print("Scheduler::start() success\n");
	}
}


/*
* @brief: The main function of the worker thread
*/
void Scheduler::run()
{
	//Get the thread id of the current thread
	int thread_id = Thread::GetThreadId();
	if (debug) {
		fmt::print("Scheduler::run() starts in thread: {0}\n", thread_id);
	}
	//set_hook_enable(true);
	//Set the current scheduler
	SetThis();
	//If the thread id is not the root thread, create a fiber as the main fiber of the worker thread
	if(thread_id != m_rootThread)
	{
		Fiber::GetThis();
	}
	//Create the idle fiber
	std::shared_ptr<Fiber> idle_fiber = std::make_shared<Fiber>(std::bind(&Scheduler::idle, this));
	fmt::print("AT here, idle_fiber->m_runInScheduler = {0}\n", idle_fiber->m_runInScheduler);
	ScheduleTask task;
	while(true)
	{
		//Reset the task
		task.reset();
		//Whether to wake up the idle fiber
		bool tickle_me = false;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_tasks.begin();
			while (it != m_tasks.end())
			{
				if (it->thread != -1 && it->thread != thread_id)
				{
					it++;
					tickle_me = true;
					continue;
				}

				//Get the task
				assert(it->fiber || it->cb);
				task = *it;
				m_tasks.erase(it); 
				m_activeThreadCount++;
				break;
			}	
			tickle_me = tickle_me || (it != m_tasks.end());
		}
		fmt::print("tickle_me: {0}\n", tickle_me);
		if(tickle_me)
		{
			tickle();
		}

		//Execute the task
		if(task.fiber)
		{
			if (debug) {
				fmt::print("task.fiber is not nullptr\n");
			}
			{					
				std::lock_guard<std::mutex> lock(task.fiber->m_mutex);
				if (task.fiber->getState() != Fiber::TERM)
				{
					task.fiber->resume();	
				}
			}
			m_activeThreadCount--;
			task.reset();
		}
		else if(task.cb)
		{
			if (debug) {
				fmt::print("task.cb is not nullptr\n");
			}
			std::shared_ptr<Fiber> cb_fiber = std::make_shared<Fiber>(task.cb);
			{
				std::lock_guard<std::mutex> lock(cb_fiber->m_mutex);
				cb_fiber->resume();			
			}
			m_activeThreadCount--;
			task.reset();	
		}
		//If there is no task, execute the idle fiber
		else
		{	
			if (debug) {
				fmt::print("task is nullptr\n");
			}
			//System shutdown -> idle fiber will jump out of the loop and end -> the state of the idle fiber is TERM -> jump out of the loop and exit run() again
            if (idle_fiber->getState() == Fiber::TERM) 
            {
				if (debug) {
					fmt::print("idle fiber term\n");
				}
                break;
            }
			m_idleThreadCount++;
			if (!idle_fiber) {
				fmt::print("idle_fiber is nullptr\n");
			}
			idle_fiber->resume();	
			if (debug) {
				fmt::print("idle_fiber resume\n");
			}
			m_idleThreadCount--;
			fmt::print("idle_fiber term\n");
		}
	}
	fmt::print("Scheduler::run() ends in thread: {0}\n", thread_id);
}

void Scheduler::stop()
{
	if (debug) {
		fmt::print("Scheduler::stop() starts: {0}\n", Thread::GetThreadId());
	}
	if(stopping())
	{
		return;
	}
	m_stopping = true;	
    if (m_useCaller) 
    {
        assert(GetThis() == this);
    } 
    else 
    {
        assert(GetThis() != this);
    }
	for (size_t i = 0; i < m_threadCount; i++) 
	{
		tickle();
	}
	if (m_schedulerFiber) 
	{
		tickle();
	}
	if (m_schedulerFiber)
	{
		m_schedulerFiber->resume();
		if (debug) {
			fmt::print("m_schedulerFiber ends in thread: \n", Thread::GetThreadId());
		}
	}

	std::vector<std::shared_ptr<Thread>> thrs;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		thrs.swap(m_threads);
	}

	for(auto &i : thrs)
	{
		i->join();
	}
	if (debug) {
		fmt::print("Scheduler::stop() ends: {0}\n", Thread::GetThreadId());
	}	
}


void Scheduler::tickle()
{
}

void Scheduler::idle()
{	
	
	while(!stopping())
	{
		if (debug) {
			fmt::print("Scheduler::idle(), sleeping in thread: {0}\n", Thread::GetThreadId());
		}
		sleep(1);	
		fmt::print("Scheduler::idle(), wake up in thread: {0}\n", Thread::GetThreadId());
		Fiber::GetThis()->yield();
		fmt::print("Scheduler::idle(), yield in thread: {0}\n", Thread::GetThreadId());
	}
}

bool Scheduler::stopping() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stopping && m_tasks.empty() && m_activeThreadCount == 0;
}


}