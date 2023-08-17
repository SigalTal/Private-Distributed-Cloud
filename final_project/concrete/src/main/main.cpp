#include <cstdlib>
#include <cstring>
#include <linux/nbd.h>

#include "nbd_comm.h"
#include "reactor.hpp"
#include "select.hpp"

char* storage = NULL;


int Handle(int nbdSock)
{
    NbdRequest* req = NbdRequestRead(nbdSock);

        switch (req->reqType)
        {
        case NBD_CMD_READ:
            memcpy(req->dataBuf, storage + req->offset, req->dataLen);
            break;
        
        case NBD_CMD_WRITE:
            memcpy(storage + req->offset, req->dataBuf, req->dataLen);
            break;
        
        default:
            break;
        }

        NbdRequestDone(req, 0);

        return 1;
}



int main()
{
    static const int StorageSize = 0x10000000;
    storage = new char[StorageSize];
    
    int nbdSock = NbdDevOpen("/dev/nbd0", StorageSize);


    if(-1 == nbdSock)
    {
        return 1;
    }

    ilrd::Reactor<ilrd::Select> rea;
    rea.RegisterReadHandler(nbdSock,ilrd::Function<int(void)>(&Handle, nbdSock));
    
    rea.Run();

    return 0;
}