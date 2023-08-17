/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: ThreadPool.cpp
 * ******************************/

#include <iostream>
#include "ThreadPool.hpp"

#define BADAPPLE -1


namespace ilrd
{

int BadApple(int a);

ThreadPool::ThreadPool(int num_of_threads): m_threads(num_of_threads), m_running(true)
{
    int i=0;
    for(i=0; i<num_of_threads; ++i)
    {
        std::cout << "TP::ctor:\n"<< i<<std::endl;
        m_threads[i] = std::thread(&ThreadPool::TaskDespacher,this);
    }
}

ThreadPool::Future ThreadPool::Async(const Function<int(void)> &func)
{
    std::shared_ptr<Task> t_ptr(new Task(func));

    m_tasks.Enqueue(t_ptr);

    return(Future(t_ptr));
}

void ThreadPool::ShutDown()
{
    size_t i=0;
    Function<int(void)> bad_apple_f(BadApple, 0);
    for(i=0; i<m_threads.size(); ++i)
    {
        Async(bad_apple_f);
    }

    for(i=0; i<m_threads.size(); ++i)
    {
        m_threads[i].join();
    }

    m_running = false;
}

ThreadPool::~ThreadPool()
{
    if(m_running == true)
    {
        ShutDown();
    }
}

int BadApple(int a)
{
    (void)(a);
    return BADAPPLE;
}

void ThreadPool::TaskDespacher()
{
    std::shared_ptr<Task> sp_task;

    while(1)
    {
        m_tasks.Dequeue(sp_task);
        if(BADAPPLE==sp_task->Run())
        {
            break;
        }
    }
}

ThreadPool::Future::Future(const std::shared_ptr<Task> &s_ptr):m_ptr(s_ptr)
{}

int ThreadPool::Future::Wait() const
{
    m_ptr->Wait();
    return m_ptr->GetStatus();
}

void ThreadPool::Future::Post()
{
    m_ptr->Post();
}

ThreadPool::Task::Task(const Function<int(void)> &func, int priority):m_function(func), m_priority(priority)
{}

void ThreadPool::Task::Wait()
{
    m_sem.Wait();
}

void ThreadPool::Task::Post()
{
    m_sem.Post();
}

void ThreadPool::Task::SetStatus(int status)
{
    m_status = status;
}

int ThreadPool::Task::GetStatus()
{
    return m_status;
}

int ThreadPool::Task::Run()
{
    m_status = m_function();
    m_sem.Post();
    return m_status;
}

} // ilrd
