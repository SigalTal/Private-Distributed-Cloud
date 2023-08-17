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
#include "RequestDispatcher.hpp"
#include "RequestHost.hpp"

using namespace ilrd;

#define SA struct sockaddr
#define PORT 29000
#define NUMOFSOC 8



int InitSocket(int *sockfd,struct sockaddr_in *servaddr, int port)
{
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
    {
        perror("socket creation failed");
        return 1;
    }
       
    memset(servaddr, 0, sizeof(*servaddr));
       
    /* Filling server information */
    servaddr->sin_family    = AF_INET; 
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port = htons(port);

    return 0;
}

int main()
{
    static const int StorageSize = 0x10000000;
    int socfd[9]={0};
    struct sockaddr_in servaddr;

    socfd[0] = NbdDevOpen("/dev/nbd2", StorageSize);
    if(-1 == socfd[0])
    {
        return 1;
    }
    for(int i=1; i<=NUMOFSOC; ++i)
    {
        InitSocket(&socfd[i], &servaddr, PORT+(i-1));

        if (connect(socfd[i], reinterpret_cast<SA*>(&servaddr), sizeof(servaddr))!= 0) 
        {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
        {
            printf("connected to the server %d..\n", socfd[1]);
        }
    }

    Reactor<Select> rea;
    RequestDispatcher disp;
    std::cout<<"line 72"<<std::endl;
    rea.RegisterReadHandler(socfd[0],Function<int(void)>(&RequestDispatcher::NbdRequestsHandle,&disp,socfd));
    for(int i=1; i<=NUMOFSOC; ++i)
    {
        rea.RegisterReadHandler(socfd[i],Function<int(void)>(&RequestDispatcher::IotHandle,&disp,socfd[i]));
    }
    
    rea.Run();

    std::cout << "Terminating\n";

    return 0;
}