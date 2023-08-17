/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: command.cpp
 * ******************************/


#include "command.hpp"

namespace ilrd
{

Command::Command(std::shared_ptr<SendMsgInfo_t> info):m_info(info)
{}

ReadRequest::ReadRequest(std::shared_ptr<SendMsgInfo_t> info): Command(info)
{}

int ReadRequest::SendRequest()
{
    if(-1 == sendmsg(m_info->sockfd,&(m_info->msg),0))
    {
        return 1;
    }
    
    return 0;
}

WriteRequest::WriteRequest(std::shared_ptr<SendMsgInfo_t> info):Command(info)
{}

int WriteRequest::SendRequest()
{
    
    m_info->payload[1].iov_base = m_info->start_data_buff;
    m_info->payload[1].iov_len = m_info->atlas_header.m_len;
    m_info->msg.msg_iovlen = 2;

    if(-1 == sendmsg(m_info->sockfd,&(m_info->msg),0))
    {
        return 1;
    }
    
    return 0;
    
}

} // ilrd
