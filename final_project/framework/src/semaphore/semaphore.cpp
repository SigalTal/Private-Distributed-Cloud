/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: semaphore.cpp
 * ******************************/


#include "semaphore.hpp"

using namespace ilrd;

Semaphore::Semaphore(int count): m_count(count)
{}

void Semaphore::Wait()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    while (m_count == 0) 
    {
        m_cv.wait(lock);
    }
    --m_count;
}

void Semaphore::Post()
{
    std::lock_guard<std::mutex> lock(m_mtx);
    ++m_count;
    m_cv.notify_one();
}


