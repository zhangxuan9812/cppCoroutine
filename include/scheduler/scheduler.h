#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

//#include "hook.h"
#include "fiber/fiber.h"
#include "thread/thread.h"

#include <mutex>
#include <vector>

namespace cppCoroutine {
class Scheduler
{
public:
	Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name="Scheduler");
	virtual ~Scheduler();
	
	const std::string& getName() const {return m_name;}

public:	
	//Get the pointer of the current scheduler
	static Scheduler* GetThis();

protected:
	//Set the current scheduler
	void SetThis();
	
public:	
	//Add a task to the scheduler
    template <class FiberOrCb>
    void scheduleLock(FiberOrCb fc, int thread = -1) 
    {
    	bool need_tickle;
    	{
    		std::lock_guard<std::mutex> lock(m_mutex);
    		// empty ->  all threads are idle -> need to be waken up
    		need_tickle = m_tasks.empty();
	        
	        ScheduleTask task(fc, thread);
	        if (task.fiber || task.cb) 
	        {
	            m_tasks.push_back(task);
	        }
    	}
    	
    	if(need_tickle)
    	{
    		tickle();
    	}
    }
	
	//Start the thread pool
	virtual void start();
	//Stop the thread pool
	virtual void stop();	
	
protected:
	virtual void tickle();
	
	//The main function of the worker thread
	virtual void run();

	// 空闲协程函数
	virtual void idle();
	
	// 是否可以关闭
	virtual bool stopping();

	bool hasIdleThreads() {return m_idleThreadCount>0;}

private:
	//Task structure
	struct ScheduleTask
	{
		std::shared_ptr<Fiber> fiber;
		std::function<void()> cb;
		//The thread id of the task
		int thread; 
		ScheduleTask()
		{
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}

		ScheduleTask(std::shared_ptr<Fiber> f, int thr)
		{
			fiber = f;
			thread = thr;
		}

		ScheduleTask(std::shared_ptr<Fiber>* f, int thr)
		{
			fiber.swap(*f);
			thread = thr;
		}	

		ScheduleTask(std::function<void()> f, int thr)
		{
			cb = f;
			thread = thr;
		}		

		ScheduleTask(std::function<void()>* f, int thr)
		{
			cb.swap(*f);
			thread = thr;
		}
		//Reset the task
		void reset()
		{
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}	
	};

private:
    //Name of the scheduler
	std::string m_name;
	std::mutex m_mutex;
	//Thread pool
	std::vector<std::shared_ptr<Thread>> m_threads;
	//Tasks
	std::vector<ScheduleTask> m_tasks;
	//Thread ids of worker threads
	std::vector<int> m_threadIds;
	//Number of threads
	size_t m_threadCount = 0;
	//Active thread count
	std::atomic<size_t> m_activeThreadCount = {0};
	//Idle thread count
	std::atomic<size_t> m_idleThreadCount = {0};

	//Whether to use the caller thread
	bool m_useCaller;
	//Scheduler fiber
	std::shared_ptr<Fiber> m_schedulerFiber;
	//Main thread id
	int m_rootThread = -1;
	//Whether the scheduler is stopping
	bool m_stopping = false;	
};

}

#endif