/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: RequestHost.cpp
 * ******************************/
#include <linux/nbd.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> /* read(), write(), close() */
#include <cstring>
#include <cassert>
#include <memory>
#include <iostream>

#include "RequestHost.hpp"
#include "function.hpp"
#include "ThreadPool.hpp"
#include "singleton.hpp"
#include "command.hpp"
#include "factory.hpp"

namespace ilrd
{

static int recv_all(int fd, char* buf, size_t count, int flags = 0);


typedef struct Consts
{
    static uint32_t const frag_size = 1024;
    static uint32_t const num_of_iot = 8;
}Consts_t;



int RequestHost::SendMsgWrapper(std::shared_ptr<SendMsgInfo_t> info)
{ 
    std::cout<<"sendmsgwrapper"<<std::endl;
    uint32_t i=0;
    Singleton<Factory<Command,uint32_t,std::shared_ptr<SendMsgInfo_t>>> fac;

    for(i=info->atlas_header.m_fragmentUid; i<m_nbq_req->dataLen/Consts::frag_size; i+=Consts::num_of_iot)
    {
        
        std::cout<<"loop: "<< i <<std::endl;
        memset(&info->msg, 0, sizeof(info->msg));
        info->payload[0].iov_base = &info->atlas_header;
        info->payload[0].iov_len = sizeof(info->atlas_header);
        info->msg.msg_iovlen = 1;
        info->msg.msg_iov = info->payload;


        std::cout<<"line 79" <<std::endl;
        std::cout<<"offset:%d"<< info->atlas_header.m_iotOffset <<std::endl;

        fac->Create(info->atlas_header.m_type, info)->SendRequest();

        /*if(info->req->reqType == NBD_CMD_WRITE)
        {
            info->payload[1].iov_base = info->start_data_buff;
            //info->start_data_buff = info->start_data_buff+(Consts::num_of_iot*Consts::frag_size);
            info->payload[1].iov_len = Consts::frag_size;
            info->msg.msg_iovlen = 2;
        }

        info->msg.msg_iov = info->payload;
        std::cout<<"line 91" <<std::endl;
        std::cout<<"offset: "<< info->atlas_header.m_iotOffset <<std::endl;
        if(-1 == sendmsg(info->sockfd,&(info->msg),0))
        {
            return 1;
        }*/

        std::cout<<"line 98" <<std::endl;

        std::cout<<"line 101" <<std::endl;
        info->atlas_header.m_iotOffset+=Consts::frag_size;
        info->atlas_header.m_fragmentUid+=Consts::num_of_iot;
        info->start_data_buff += Consts::num_of_iot*Consts::frag_size;

    }

    std::cout<<"line 105" <<std::endl;

    std::cout<<"line 108" <<std::endl;

    return 0;
}

static int recv_all(int fd, char* buf, size_t count, int flags)
{
  int bytes_read;

  while (count > 0) 
 {
    bytes_read = recv(fd, buf, count, flags);
    assert(bytes_read > 0);
    buf += bytes_read;
    count -= bytes_read;
}
  assert(count == 0);

  return 0;
}

int RequestHost::ReadReaply(int socket)
{
    AtlasHeader_t atlas_header;

    recv_all(socket, reinterpret_cast<char*>(&atlas_header), sizeof(atlas_header));
    std::cout<<"ReadReaply " << atlas_header.m_requestUid << "; " << atlas_header.m_fragmentUid << "; " << atlas_header.m_type << "; " << atlas_header.m_len <<std::endl;

    if(m_nbq_req->reqType == NBD_CMD_READ)
    {
        recv_all(socket, m_nbq_req->dataBuf+atlas_header.m_fragmentUid*Consts::frag_size, Consts::frag_size);
    }

    fragmentation_set.erase(atlas_header.m_fragmentUid);

    if(fragmentation_set.empty())
    {
        NbdRequestDone(m_nbq_req, 0);
        return 1;
    }

    return 0;
    
}

uint32_t RequestHost::Peek(int socket)
{
    uint32_t req_uid = 0;
    recv_all(socket,  reinterpret_cast<char *> (&req_uid), sizeof(req_uid), MSG_PEEK);
    std::cout << "Peek " << req_uid << std::endl;
    return req_uid;
}

std::shared_ptr<RequestHost> RequestHost::Create(int *socket)
{
    std::cout<<"create"<<std::endl;
    static uint32_t req_uid = 0;
    std::shared_ptr<RequestHost> req(new RequestHost);
    req->m_nbq_req = NbdRequestRead(socket[0]);

    std::cout << "Req: " << req->m_nbq_req->reqType << "; " << req->m_nbq_req->dataLen << std::endl;
    req->m_req_uid = req_uid;
    ++req_uid;

    for(uint32_t i=0; i<req->m_nbq_req->dataLen/Consts::frag_size; ++i)
    {
        std::cout <<"num of frag: " <<req->m_nbq_req->dataLen/Consts::frag_size <<std::endl;
        req->fragmentation_set.insert(i);     
    }

    return req;
}

uint32_t RequestHost::GetUid()
{
    return m_req_uid;
}

void RequestHost::Distribute(int *socket)
{
    Singleton<ThreadPool> tp;
    std::cout<<"distribute"<<std::endl;

    int first_frag_indx = m_nbq_req->offset/Consts::frag_size;

    int num_of_iterations = (m_nbq_req->dataLen/Consts::frag_size < Consts::num_of_iot)? 
        m_nbq_req->dataLen/Consts::frag_size : Consts::num_of_iot;

    int iot=0;
    for(int j=0; j<num_of_iterations; ++j)
    {
        iot=first_frag_indx%Consts::num_of_iot;
        std::shared_ptr<SendMsgInfo_t> info(new SendMsgInfo_t);
        info->req = m_nbq_req;
        info->atlas_header.m_alarmUid = 0;
        info->atlas_header.m_type = m_nbq_req->reqType;
        info->atlas_header.m_len= Consts::frag_size;
        info->atlas_header.m_requestUid = m_req_uid;
        info->atlas_header.m_fragmentUid = j;
        info->atlas_header.m_iotOffset = first_frag_indx*Consts::frag_size/Consts::num_of_iot;
        info->start_data_buff=m_nbq_req->dataBuf+j*Consts::frag_size;
        info->sockfd = socket[iot+1];
        ++first_frag_indx;
        std::cout << "socket: " << info->sockfd << std::endl;
        tp->Async(Function<int(void)>(&RequestHost::SendMsgWrapper,this,info));
    }    
}

}
