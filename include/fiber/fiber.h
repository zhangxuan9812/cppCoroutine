#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <iostream>  
#include <string>   
#include <memory>       
#include <atomic>       
#include <functional>   
#include <cassert> 
#include <fmt/core.h>
#include <fmt/ostream.h>
#ifdef __APPLE__
#include <boost/context/detail/fcontext.hpp>
#else
#ifndef _XOPEN_SOURCE
//In order to use ucontext.h, you need to define _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <ucontext.h>
#endif
#include <unistd.h>
#include <mutex>

namespace cppCoroutine {
class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
	//Fiber has three states: READY, RUNNING, TERM
	//READY: The fiber is ready to run
	//RUNNING: The fiber is running
	//TERM: The fiber is terminated
	enum State
	{
		READY, 
		RUNNING, 
		TERM 
	};

private:
	//Constructor of the Fiber class, only used by GetThis() 
	Fiber();

public:
	Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = true);
	~Fiber();
	//Reset the fiber
	void reset(std::function<void()> cb);
	//Resume the fiber
	void resume();
	//Yield the fiber
	void yield();
	//Get the id of the fiber
	uint64_t getId() const {return m_id;}
	//Get the state of the fiber
	State getState() const {return m_state;}

public:
	//Set the current fiber
	static void SetThis(Fiber *f);

	//Get the current fiber 
	static std::shared_ptr<Fiber> GetThis();

	//Set the scheduler fiber
	static void SetSchedulerFiber(Fiber* f);
	
	//Get the id of the current fiber
	static uint64_t GetFiberId();

	//Main function of the fiber
	static void MainFunc(boost::context::detail::transfer_t t);	

	static void empty_coroutine_function(boost::context::detail::transfer_t t) {
	}

public:
	//id
	uint64_t m_id = 0;
	//the size of the stack
	uint32_t m_stacksize = 0;
	//the state of the fiber
	State m_state = READY;
	//the context of the fiber
	#ifdef __APPLE__
	boost::context::detail::fcontext_t m_ctx = nullptr;
	#else
	ucontext_t m_ctx;
	#endif

	//the pointer of the stack
	void* m_stack = nullptr;
	//the function of the fiber
	std::function<void()> m_cb;
	//whether the fiber runs in the scheduler
	bool m_runInScheduler = false;
public:
	std::mutex m_mutex;
	std::string m_name;
};
}


#endif

