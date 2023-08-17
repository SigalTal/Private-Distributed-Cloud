/*********************************
 * Reviewer:  
 * Author: Sigal Tal
 * File Name: command.hpp
 * ******************************/

#pragma once

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>

#include "nbd_comm.h"


namespace ilrd
{


typedef struct AtlasHeader
{
    uint32_t    m_requestUid;
    uint32_t    m_fragmentUid;
    uint32_t    m_alarmUid;
    uint32_t    m_iotOffset;
    uint32_t    m_type;
    uint32_t    m_len;
}AtlasHeader_t;

typedef struct SendMsgInfo
{
    uint32_t sockfd;
    AtlasHeader_t atlas_header;
    NbdRequest* req;
    char* start_data_buff;
    struct msghdr msg;
    struct iovec payload[2];
}SendMsgInfo_t;



class Command
{
public:
    explicit Command(std::shared_ptr<SendMsgInfo_t> info); 
    virtual int SendRequest() = 0;

protected:
    std::shared_ptr<SendMsgInfo_t> m_info;

private:
    
};

class ReadRequest:public Command
{
public:
    explicit ReadRequest(std::shared_ptr<SendMsgInfo_t> info);
    int SendRequest();

private:

};

class WriteRequest: public Command
{
public:
    explicit WriteRequest(std::shared_ptr<SendMsgInfo_t> info);
    int SendRequest();

private:

};




}//ilrd
