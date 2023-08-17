/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: ThreadPool.hpp
 * ******************************/

#include <vector>	/* std::vector */
#include <thread>	/* std::thread */
#include "BTreadSafeQ.hpp"
#include "function.hpp"
#include "semaphore.hpp"

namespace ilrd
{

class ThreadPool
{
public:

    class Future;

    explicit ThreadPool(int num_of_threads = 4);
    Future Async(const Function<int(void)>& func);
    
    void ShutDown();    // terminate New tasks! finishes what currently in queue
    void Cancel();        // cancel all tasks and empties the queue

    ~ThreadPool();

    ThreadPool(const ThreadPool& other) = delete;
    void operator=(const ThreadPool& other) = delete;

private:
    class Compare
    {
        public:
        bool operator()(const Function<int(void)>& a, const Function<int(void)>& b)
        {
            return true;
        }
    };
    class Task;
    std::vector<std::thread> m_threads;
    BtsQueue<std::shared_ptr<Task>> m_tasks;
    void TaskDespacher();
    bool m_running;
    
}; //class thread pool

class ThreadPool::Task
{
 public:
    explicit Task(const Function<int(void)> &func, int priority = 0);
    void Wait();
    void Post();
    void SetStatus(int status);
    int GetStatus();
    int Run();
  private:
    Function<int(void)> m_function;
    int m_status;
    int m_priority;
    Semaphore m_sem;
}; //class Future

class ThreadPool::Future
{
public:
    explicit Future(const std::shared_ptr<Task> &s_ptr = nullptr);
    int Wait() const;
    void Post();
    
private:
    std::shared_ptr<Task> m_ptr;
};

} // namespace ilrd
