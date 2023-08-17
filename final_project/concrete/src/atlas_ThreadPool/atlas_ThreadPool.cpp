#include <cstdlib>
#include <cstring>
#include <linux/nbd.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> /* read(), write(), close() */
#include <cassert>
#include <iostream>
#include <cstdio>

#include "nbd_comm.h"
#include "reactor.hpp"
#include "select.hpp"
#include "ThreadPool.hpp"
#include "singleton.hpp"

using namespace ilrd;

#define SA struct sockaddr
#define PORT 29000
#define NUMOFSOC 2

typedef struct AtlasHeader
{
    uint64_t    m_requestUid;
    uint32_t    m_alarmUid;
    uint32_t    m_iotOffset;
    uint32_t    m_type;
    uint32_t    m_len;
}AtlasHeader_t;

typedef struct SendMsgInfo
{
    int sockfd;
    AtlasHeader_t atlas_header;
    NbdRequest* req;
    

}SendMsgInfo_t;

int SendMsgWrapper(SendMsgInfo_t info)
{   
    struct iovec payload[2]={0,0};
    struct msghdr msg={};

    payload[0].iov_base = &info.atlas_header;
    payload[0].iov_len = sizeof(AtlasHeader_t);
    msg.msg_iovlen = 1;

    if(info.req->reqType == NBD_CMD_WRITE)
    {
        payload[1].iov_base = info.req->dataBuf;
        payload[1].iov_len = info.req->dataLen;
        msg.msg_iovlen = 2;
    }

    msg.msg_iov = payload;
    return sendmsg(info.sockfd,&msg,0);
}


static int read_all(int fd, char* buf, size_t count)
{
  int bytes_read;

  while (count > 0) 
 {
    bytes_read = read(fd, buf, count);
    assert(bytes_read > 0);
    buf += bytes_read;
    count -= bytes_read;
}
  assert(count == 0);

  return 0;
}

int InitSocket(int *sockfd,struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr)
{
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
    {
        perror("socket creation failed");
        return 1;
    }
       
    memset(servaddr, 0, sizeof(*servaddr));
    memset(cliaddr, 0, sizeof(*cliaddr));
       
    /* Filling server information */
    servaddr->sin_family    = AF_INET; 
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port = htons(PORT);

    return 0;
}

AtlasHeader_t ConvertToAtlas(NbdRequest* request)
{
    AtlasHeader_t hdr;
    hdr.m_requestUid = reinterpret_cast<uint64_t>(request);
    hdr.m_iotOffset = request->offset;
    hdr.m_type = request->reqType;
    hdr.m_len = request->dataLen;  

    return hdr; 
}


int NbdHandle(int* socfd)
{
    NbdRequest* req = NbdRequestRead(socfd[0]);
    SendMsgInfo_t info={};
    info.atlas_header = ConvertToAtlas(req);
    info.req =req;
    info.sockfd=socfd[1];
   
    Singleton<ThreadPool> tp;
    tp->Async(Function<int(void)>(&SendMsgWrapper,info)); 
       
    return 0;
}

int IotHandle(int* socfd)
{
    AtlasHeader_t atlas_header;
    NbdRequest* req = NULL;

    read_all(socfd[1], reinterpret_cast<char*>(&atlas_header), sizeof(AtlasHeader_t));
    req = reinterpret_cast<NbdRequest*>(atlas_header.m_requestUid);

    if(req->reqType == NBD_CMD_READ)
    {
        read_all(socfd[1], req->dataBuf, req->dataLen);
    }

    NbdRequestDone(req, 0);

    return 0;
}


int main()
{
    static const int StorageSize = 0x10000000;
    int socfd[2]={0};
    struct sockaddr_in servaddr, cli;

    
    socfd[0] = NbdDevOpen("/dev/nbd0", StorageSize);
    if(-1 == socfd[0])
    {
        return 1;
    }

    InitSocket(&socfd[1], &servaddr, &cli);

    if (connect(socfd[1], reinterpret_cast<SA*>(&servaddr), sizeof(servaddr))!= 0) 
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }

    ilrd::Reactor<ilrd::Select> rea;
    std::cout<<"line 170"<<std::endl;
    rea.RegisterReadHandler(socfd[0],ilrd::Function<int(void)>(&NbdHandle, socfd));
    rea.RegisterReadHandler(socfd[1],ilrd::Function<int(void)>(&IotHandle, socfd));
    
    rea.Run();

    return 0;
}