#include "thread/thread.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <sys/syscall.h> 
#include <iostream>
#include <unistd.h>  

namespace cppCoroutine {
//thread_local is a keyword in C++11, which is used to define thread_local variables
//t_thread is a thread_local variable, which is used to store the pointer of the current thread
static thread_local Thread* t_thread          = nullptr;
//t_thread_name is a thread_local variable, which is used to store the name of the current thread
static thread_local std::string t_thread_name = "UNKNOWN";


/*
* @brief: Constructor of the Thread class
* @param: cb, the function pointer of the thread function
* @param: name, the name of the thread
*/
Thread::Thread(std::function<void()> cb, const std::string &name): 
m_cb(cb), m_name(name) 
{
    //pthread_create is used to create a thread
    //m_thread is the thread ID of the created thread
    //The first parameter stores the thread ID of the created thread
    //The second parameter is the thread attribute, which is NULL by default
    //The third parameter is the thread function, which is the run function of the Thread class
    //The fourth parameter is the current object, which is passed to the run function
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) 
    {
        std::cerr << "pthread_create thread fail, rt=" << rt << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
    //Wait for new thread finish initialization 
    m_semaphore.wait();
}

Thread::~Thread() 
{
    if (m_thread) 
    {
        pthread_detach(m_thread);
        m_thread = 0;
    }
}
/*
* @brief: Get the thread ID of the current thread
*/
pid_t Thread::GetThreadId() {
    //return syscall(SYS_gettid);
    uint64_t tid;
    //pthread_threadid_np is a non-standard function, but it is available on both Linux and macOS, which can be used to get the thread ID
    //The first parameter is the thread, and the second parameter is the thread ID
    //If you pass NULL as the first parameter, you can get the thread ID of the current thread
    //If you pass a thread as the first parameter, you can get the thread ID of the specified thread
    //If you are in linux, you can use syscall(SYS_gettid) to get the thread ID
    #ifdef __APPLE__
    pthread_threadid_np(NULL, &tid);
    #else
    tid = syscall(SYS_gettid);
    #endif
    return static_cast<pid_t>(tid);
}


/*
* @brief: Get the pointer of the current thread
*/
Thread* Thread::GetThis() {
    return t_thread;
}


/*
* @brief: Get the name of the current thread
*/
const std::string& Thread::GetName() 
{
    return t_thread_name;
}


/*
* @brief: Set the name of the current thread
*/
void Thread::SetName(const std::string &name) 
{
    if (t_thread) 
    {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}


/*
* @brief: Wait for the thread to finish
* @note: If the thread is not finished, the current thread will be blocked
* @note: If the thread is finished, the function will return immediately
*/
void Thread::join() 
{
    if (m_thread) 
    {
        //The pthread_join function is used to wait for the thread to finish
        //The first parameter is the thread to wait for
        //The second parameter means that the return value of the thread function is not needed
        int rt = pthread_join(m_thread, nullptr);
        //If rt is not 0, it means that the thread join failed
        if (rt) 
        {
            fmt::print(std::cerr, "pthread_join failed, rt = {0}, name = {1}\n", rt, m_name);
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}


/*
* @brief: The function that the thread needs to run
*/
void* Thread::run(void* arg) 
{
    Thread* thread = (Thread*)arg;
    //Set the thread_local variable t_thread to the pointer of the current thread
    t_thread       = thread;
    //Set the thread_local variable t_thread_name to the name of the current thread
    t_thread_name  = thread->m_name;
    //Set the thread ID of the current thread
    thread->m_id   = GetThreadId();
    #ifdef __APPLE__
    //pthread_setname_np is a non-standard function, but it is available on both Linux and macOS, which can be used to set the name of the thread in order to facilitate debugging
    pthread_setname_np(thread->m_name.substr(0, 15).c_str());
    #else
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    #endif

    std::function<void()> cb;
    cb = std::move(thread->m_cb);
    //Notify the main thread that the new thread has been initialized
    thread->m_semaphore.signal();
    //Call the thread function
    cb();
    return 0;
}
} 

