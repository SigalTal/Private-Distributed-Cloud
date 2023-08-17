/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: semaphore.hpp
 * ******************************/


#include <mutex>
#include <condition_variable>

namespace ilrd
{

class Semaphore 
{
public:
    explicit Semaphore(int count_ = 0);
    void Post();
    void Wait();

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    int m_count;
};//class semaphore

}//ilrd

