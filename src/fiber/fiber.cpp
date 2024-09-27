#include "fiber/fiber.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#ifdef __APPLE__
#include <boost/context/detail/fcontext.hpp>
#else
#include <ucontext.h>
#endif
#include <unistd.h>
static bool debug = false;

namespace cppCoroutine {
const std::string Scheduler_Fiber = "SchedulerFiber";
const std::string Main_Fiber = "MainFiber";
#ifdef __APPLE__
boost::context::detail::fcontext_t main_fcontext = nullptr;
boost::context::detail::fcontext_t scheduler_fiber_context = nullptr;
boost::context::detail::fcontext_t coroutine_context = nullptr; 
#endif

//t_fiber is a thread_local variable, which is used to store the pointer of the current fiber
static thread_local Fiber* t_fiber = nullptr;
//t_thread_fiber is a thread_local variable, which is used to store the pointer of the main fiber
static thread_local std::shared_ptr<Fiber> t_thread_fiber = nullptr;
//t_scheduler_fiber is a thread_local variable, which is used to store the pointer of the scheduler fiber
static thread_local Fiber* t_scheduler_fiber = nullptr;
//Fiber id
static std::atomic<uint64_t> s_fiber_id{0};
//Fiber count
static std::atomic<uint64_t> s_fiber_count{0};

// Set the current fiber
void Fiber::SetThis(Fiber *f)
{
	t_fiber = f;
}

//Use this function to create the main fiber of the current thread
std::shared_ptr<Fiber> Fiber::GetThis()
{
	if(t_fiber)
	{	
		return t_fiber->shared_from_this();
	}

	std::shared_ptr<Fiber> main_fiber(new Fiber());
	t_thread_fiber = main_fiber;
	//Set the main fiber as the scheduler fiber by default
	t_scheduler_fiber = main_fiber.get(); 
	
	assert(t_fiber == main_fiber.get());
	return main_fiber->shared_from_this();
}

void Fiber::SetSchedulerFiber(Fiber* f)
{
	t_scheduler_fiber = f;
	#ifdef __APPLE__
	scheduler_fiber_context = f->m_ctx;
	#endif
}

uint64_t Fiber::GetFiberId()
{
	if(t_fiber)
	{
		return t_fiber->getId();
	}
	return (uint64_t)-1;
}

Fiber::Fiber()
{
	SetThis(this);
	m_state = RUNNING;
	m_name = "MainFiber";
	#ifdef __APPLE__
	int m_stacksize = 128000;
	m_stack = malloc(m_stacksize);
	m_ctx = boost::context::detail::make_fcontext(static_cast<char*>(m_stack) + m_stacksize, m_stacksize, &Fiber::empty_coroutine_function);
	if (!m_ctx) {
		fmt::print(std::cerr, "Fiber() failed\n");
		pthread_exit(NULL);
	};
	main_fcontext = m_ctx;
	#else
	if(getcontext(&m_ctx))
	{
		fmt::print(std::cerr, "Fiber() failed\n");
		pthread_exit(NULL);
	}
	#endif
	m_id = s_fiber_id++;
	s_fiber_count ++;
	if (debug) {
		fmt::print("Fiber(): main id = {0}\n", m_id);
	}
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler):
m_cb(cb), m_runInScheduler(run_in_scheduler)
{
	fmt::print("m_runInscheduler is {0}\n", m_runInScheduler);
	m_state = READY;
	m_name = "SubFiber";
	//Allocate the stack
	m_stacksize = stacksize ? stacksize : 1024 * 1024;
	m_stack = malloc(m_stacksize);
	#ifdef __APPLE__
	if (!m_stack) {
		fmt::print(std::cerr, "Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler) failed\n");
		pthread_exit(NULL);
	}
	m_ctx = boost::context::detail::make_fcontext(static_cast<char*>(m_stack) + m_stacksize, m_stacksize, &Fiber::MainFunc);
	#else
	if (getcontext(&m_ctx)) {
		fmt::print(std::cerr, "Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler) failed\n");
		pthread_exit(NULL);
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	makecontext(&m_ctx, &Fiber::MainFunc, 0);
	#endif
	m_id = s_fiber_id++;
	s_fiber_count ++;
	if (debug) {
		fmt::print("Fiber(): child id = {0}\n", m_id);
	}
}

Fiber::~Fiber()
{
	s_fiber_count--;
	if(m_stack)
	{
		free(m_stack);
	}
	if (debug) {
		fmt::print("~Fiber(): id = {0}\n", m_id);
	}
}

void Fiber::reset(std::function<void()> cb)
{
	assert(m_stack != nullptr && m_state == TERM);
	m_state = READY;
	m_cb = cb;
	#ifdef __APPLE__
	m_ctx = boost::context::detail::make_fcontext(static_cast<char*>(m_stack) + m_stacksize, m_stacksize, &Fiber::MainFunc);
	#else
	if(getcontext(&m_ctx))
	{
		fmt::print(std::cerr, "reset() failed\n");
		pthread_exit(NULL);
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	makecontext(&m_ctx, &Fiber::MainFunc, 0);
	#endif
}


void Fiber::resume()
{
	fmt::print("resume()\n");
	if (this->m_cb) {
		fmt::print("m_cb() is not nullptr\n");
	}
	else {
		fmt::print("m_cb() is nullptr\n");
	}
	assert(m_state == READY);
    m_state = RUNNING;
    SetThis(this);
	#ifdef __APPLE__
    //Swithch the context according to whether the fiber runs in the scheduler
	//fmt::print("m_runInScheduler: {0}\n", m_runInScheduler);
	fmt::print("I'm here!\n");
    if (m_runInScheduler) {
        scheduler_fiber_context = boost::context::detail::jump_fcontext(m_ctx, (void*)&Scheduler_Fiber).fctx;
    } else {
        main_fcontext = boost::context::detail::jump_fcontext(m_ctx, (void*)&Main_Fiber).fctx;
		if (main_fcontext == nullptr) {
			fmt::print("main_fcontext is nullptr At resume!\n");
		}
    }
	#else
	if(m_runInScheduler)
	{
		if(swapcontext(&(t_scheduler_fiber->m_ctx), &m_ctx))
		{
			fmt::print(std::cerr, "resume() to t_scheduler_fiber failed\n");
			pthread_exit(NULL);
		}		
	}
	else
	{
		if(swapcontext(&(t_thread_fiber->m_ctx), &m_ctx))
		{
			fmt::print(std::cerr, "resume() to t_thread_fiber failed\n");
			pthread_exit(NULL);
		}	
	}
	#endif
}


void Fiber::yield()
{
	fmt::print("yield()\n");
	assert(m_state==RUNNING || m_state==TERM);
	if(m_state != TERM)
	{
		//fmt::print("yield() m_state != TERM\n");
		m_state = READY;
	}
	#ifdef __APPLE__
	if (m_runInScheduler) {
        SetThis(t_scheduler_fiber);
        //boost::context::detail::jump_fcontext(t_scheduler_fiber->m_ctx, &m_ctx);
		fmt::print("jump to scheduler\n");
		m_ctx = boost::context::detail::jump_fcontext(scheduler_fiber_context, this).fctx;
		fmt::print("jump to scheduler end\n");
    } else {
        SetThis(t_thread_fiber.get());
		fmt::print("jump to main\n");
        //boost::context::detail::jump_fcontext(t_thread_fiber->m_ctx, &m_ctx);
		if (main_fcontext == nullptr) {
			fmt::print("main_fcontext is nullptr\n");
		}
		m_ctx = boost::context::detail::jump_fcontext(main_fcontext, this).fctx;
		//fmt::print("jump to main\n");
    }
	fmt::print("yield() end\n");
	#else
	if(m_runInScheduler)
	{
		SetThis(t_scheduler_fiber);
		if(swapcontext(&m_ctx, &(t_scheduler_fiber->m_ctx)))
		{
			std::cerr << "yield() to to t_scheduler_fiber failed\n";
			pthread_exit(NULL);
		}		
	}
	else
	{
		SetThis(t_thread_fiber.get());
		if(swapcontext(&m_ctx, &(t_thread_fiber->m_ctx)))
		{
			std::cerr << "yield() to t_thread_fiber failed\n";
			pthread_exit(NULL);
		}	
	}
	#endif	
}


#ifdef __APPLE__
void Fiber::MainFunc(boost::context::detail::transfer_t t)
{
	//main_fcontext = t.fctx;
	fmt::print("MainFunc begin\n");
	if (t.data == &Scheduler_Fiber) {
		scheduler_fiber_context = t.fctx;
	}
	else if (t.data == &Main_Fiber) {
		main_fcontext = t.fctx;
	}
	fmt::print("MainFunc\n");
	std::shared_ptr<Fiber> curr = GetThis();
	if (curr) {
		fmt::print("curr is not nullptr\n");
	}
	else {
		fmt::print("curr is nullptr\n");
	}
	assert(curr != nullptr);
	if (!curr->m_cb) {
		fmt::print("m_cb() is nullptr\n");
	}
	//Run the function
	fmt::print("m_cb() begin\n");
	curr->m_cb(); 
	fmt::print("m_cb() end\n");
	curr->m_cb = nullptr;
	//Set the state of the fiber to TERM
	curr->m_state = TERM;
	//Get the pointer of the current fiber
	auto raw_ptr = curr.get();
	//Destroy the shared_ptr
	curr.reset(); 
	//Yield the fiber
	fmt::print("MainFunc yield\n");
	raw_ptr->yield(); 
	fmt::print("MainFunc end\n");
}


void empty_coroutine_function(boost::context::detail::transfer_t t) {
	//boost::context::detail::jump_fcontext(t.fctx, nullptr);
	fmt::print("empty_coroutine_function\n");
	coroutine_context = t.fctx;
	fmt::print("empty_coroutine_function\n");
	//boost::context::detail::jump_fcontext(t.fctx, nullptr);
	}
#else
void Fiber::MainFunc()
{
	std::shared_ptr<Fiber> curr = GetThis();
	assert(curr != nullptr);
	if (!curr->m_cb) {
		fmt::print("m_cb() is nullptr\n");
	}
	//Run the function
	curr->m_cb(); 
	curr->m_cb = nullptr;
	//Set the state of the fiber to TERM
	curr->m_state = TERM;
	//Get the pointer of the current fiber
	auto raw_ptr = curr.get();
	//Destroy the shared_ptr
	curr.reset(); 
	//Yield the fiber
	raw_ptr->yield(); 
}
#endif
}