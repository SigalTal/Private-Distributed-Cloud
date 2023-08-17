/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: RequestDispatcher.cpp
 * ******************************/

#include <iostream>
#include <memory>

#include "RequestDispatcher.hpp"
#include "factory.hpp"
#include "singleton.hpp"
#include "command.hpp"

namespace ilrd

{
RequestDispatcher::RequestDispatcher()
{
    Singleton<Factory<Command,uint32_t,std::shared_ptr<SendMsgInfo_t>>> fac;
    fac->Add<WriteRequest>(NBD_CMD_WRITE);
    fac->Add<ReadRequest>(NBD_CMD_READ);
}

int RequestDispatcher::NbdRequestsHandle(int *socket)
{
    std::cout<<"NbdRequestsHandle"<<std::endl;
    

    std::shared_ptr<RequestHost> req(RequestHost::Create(socket));
    m_activeRequests[req->GetUid()] = req;
    req->Distribute(socket);

    return 0;
}

int RequestDispatcher::IotHandle(int socket)
{
    uint32_t req_uid = 0;
    req_uid = RequestHost::Peek(socket);
    std::cout << "IotHandle: " << req_uid << std::endl;
    if(m_activeRequests[req_uid]->ReadReaply(socket)==1)
    {
        m_activeRequests.erase(req_uid);
    }
    return 0;
}

} // ilrd
