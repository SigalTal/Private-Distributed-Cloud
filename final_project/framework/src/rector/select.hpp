/*********************************
 * Reviewer: 
 * Author: Mike Meyers
 * File: select.hpp
 * ******************************/

#pragma once
#include <sys/select.h>
#include "monitor.hpp"

namespace ilrd
{


class Select : public IMonitor
{
public:
	explicit Select();
	
    int WaitForEvent();
    int GetNextReadFd();
    
    void SetRead(int fd);
   // int SetWrite(int fd);
   // int SetError(int fd);
    
    void ClearRead(int fd);
   // int ClearWrite(int fd);
   // int ClearError(int fd);
    
    
private:
    fd_set m_read_set; 
    fd_set m_master;
    int m_fd_max;
	
};

}//ilrd