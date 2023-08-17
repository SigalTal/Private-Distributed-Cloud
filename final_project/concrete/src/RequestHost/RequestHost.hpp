/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: RequestHost.hpp
 * ******************************/

#pragma once

#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> 

#include <memory>
#include <set>
#include "nbd_comm.h"
#include "command.hpp"

namespace ilrd
{

class RequestHost
{
public:
	static std::shared_ptr<RequestHost> Create(int *socket);
    uint32_t GetUid();
    void Distribute(int *socket);
    int SendMsgWrapper(std::shared_ptr<SendMsgInfo_t> info);
    int ReadReaply(int socket);
    static uint32_t Peek(int socket);

private:

    NbdRequest *m_nbq_req;
    uint32_t m_req_uid;
	std::set<int> fragmentation_set;
    RequestHost(){};
};



}//ilrd
