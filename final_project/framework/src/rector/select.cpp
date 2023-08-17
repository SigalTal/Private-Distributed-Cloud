/*********************************
 * Reviewer: 
 * Author: Mike Meyers
 * File: select.hpp
 * ******************************/

#include <cstddef>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <iostream>
#include "select.hpp"

#define MAX(x,y) ((x>y)?x:y)

namespace ilrd
{
    Select::Select():m_fd_max(0)
    {
        FD_ZERO(&m_read_set);
        FD_ZERO(&m_master);
    }

    int Select::WaitForEvent()
    {
        m_read_set = m_master;
       return select(m_fd_max+1, &m_read_set, NULL ,NULL, NULL); 
    }

    int Select::GetNextReadFd()
    {
        int i=0;

        for(i=0; i<m_fd_max+1; ++i)
        {
            if(FD_ISSET(i,&m_read_set))
            {
                FD_CLR(i,&m_read_set);
                return i;
            }
        } 

        return -1;  
    }

    void Select::SetRead(int fd)
    {
       FD_SET(fd, &m_master); 
       m_fd_max = MAX(fd,m_fd_max);
    }

    void Select::ClearRead(int fd)
    {
        FD_CLR(fd,&m_master);
    }

} // ilrd