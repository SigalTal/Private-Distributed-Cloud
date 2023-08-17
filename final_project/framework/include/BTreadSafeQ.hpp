/*********************************
 * Reviewer: 
 * Author: Connor Mcgregor
 * File: PriQueue.hpp
 * ******************************/

#pragma once
#include <queue>	/*std::priority_queue*/
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace ilrd
{

template<typename T, typename COMPARE = std::less<T> >
class BtsQueue
{
public:
    explicit BtsQueue() = default;
    void Enqueue(const T& task);
    void Dequeue(T& task);
	void Clear();
    BtsQueue(BtsQueue const &) = delete;
    void operator=(const BtsQueue& other) = delete;
  
private:
	std::priority_queue<T, std::vector<T>, COMPARE> m_priqueue;
	std::mutex m_lock_dequeue;
    std::condition_variable m_cond;

}; // class PriQueue

template <typename T, typename COMPARE>
void BtsQueue<T, COMPARE>::Enqueue(const T &task)
{
    std::unique_lock<std::mutex> mlock(m_lock_dequeue);
    m_priqueue.push(task);
    mlock.unlock();
    m_cond.notify_one();
}

template <typename T, typename COMPARE>
void BtsQueue<T, COMPARE>::Dequeue(T &task)
{
    std::unique_lock<std::mutex> mlock(m_lock_dequeue);
    while (m_priqueue.empty())
    {
      m_cond.wait(mlock);
    }
    task = m_priqueue.top();
    m_priqueue.pop();
}

template <typename T, typename COMPARE>
void BtsQueue<T, COMPARE>::Clear()
{
     std::unique_lock<std::mutex> mlock(m_lock_dequeue);

     while (!m_priqueue.empty())
     {
        m_priqueue.pop();
     }
}

} // ilrd