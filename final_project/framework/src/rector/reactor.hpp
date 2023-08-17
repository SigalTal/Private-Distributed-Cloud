
/*********************************
 * Reviewer: 
 * Author: Mike Meyers
 * File: reactor.hpp
 * ******************************/

#pragma once


#include <unordered_map>
#include "monitor.hpp"
#include "function.hpp"

namespace ilrd
{
  
template<typename MONITOR> 
class Reactor
{
public:
  explicit Reactor();
	~Reactor(){}
    
  void RegisterReadHandler(int fd,const Function<int(void)>& func);
  void Run();
  //Void Stop();
  
  
private:
	std::unordered_map<int, Function<int(void)> > m_handlers;
  MONITOR m_monit;

};

template <typename MONITOR>
Reactor<MONITOR>::Reactor():m_handlers(),m_monit()
{
    (void)static_cast<MONITOR*>(&m_monit);
}

template <typename MONITOR>
void Reactor<MONITOR>::RegisterReadHandler(int fd,const Function<int(void)>& func)
{
  m_handlers[fd]=func;
  m_monit.SetRead(fd); 
}

template <typename MONITOR>
void Reactor<MONITOR>::Run()
{
  while (1)
  {
    int n = m_monit.WaitForEvent();

    for(; n>0; --n)
    {
        m_handlers[m_monit.GetNextReadFd()]();
    }
    
  }
  
}

} // ilrd
