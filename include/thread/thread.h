#ifndef _THREAD_H_
#define _THREAD_H_

#include <mutex>
#include <condition_variable>
#include <functional>     

namespace cppCoroutine {
/*
* @brief: Semaphore class, which is used to support the thread synchronization mechanism
*/
class Semaphore 
{
private:
    std::mutex mtx;                
    std::condition_variable cv;    
    int count;                   

public:
    //Initialize the semaphore with a count of 0
    explicit Semaphore(int count_ = 0) : count(count_) {}
    
    /*
    * @brief: P operation
    * @note: If the count is 0, the current thread will be blocked until the count is greater than 0
    * @note: If the count is greater than 0, the count will be decremented by 1
    * @note: If the count is less than 0, the count will not be decremented
    */
    void wait() 
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (count == 0) { 
            //Block the current thread
            cv.wait(lock);
        }
        count--;
    }

    /*
    * @brief: V operation
    * @note: If there are threads blocked on the semaphore, one of them will be woken up
    * @note: If the count is less than 0, the count will be incremented by 1
    */
    void signal() 
    {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        //Notify one of the waiting threads
        cv.notify_one(); 
    }
};

/*
* @brief: Thread class, which is used to support the thread creation and management
*/
class Thread {
public:
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    //Get the process id of the thread
    pid_t getId() const { return m_id; }
    //Get the name of the thread
    const std::string& getName() const { return m_name; }
    void join();

public:
    //Get the thread id of the current thread
	static pid_t GetThreadId();
    //Get the pointer of the current thread
    static Thread* GetThis();
    //Get the name of the current thread
    static const std::string& GetName();
    //Set the name of the current thread
    static void SetName(const std::string& name);

private:
	//The function that the thread needs to run
    static void* run(void* arg);

private:
    //The thread id of the current thread
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    //The function that the thread needs to run
    std::function<void()> m_cb;
    //The name of the thread
    std::string m_name;
    //The semaphore used to synchronize the thread
    Semaphore m_semaphore;
};
}


#endif
