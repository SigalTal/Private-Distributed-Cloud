#include <linux/nbd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <stdio.h>/*stderr*/
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>/*offsetof*/
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> /*htol*/
#include <assert.h>


#include "nbd_comm.h"

#define NUM_OF_FD 3

typedef struct
{
    NbdRequest  nbd_req;
    int         nbd_sock;
    struct nbd_reply reply;
    char        buffer[1];
}nbd_message_t;

int CleanUp(int arr[], size_t size);
static int read_all(int fd, char* buf, size_t count);
static int write_all(int fd, char* buf, size_t count);
u_int64_t ntohll(u_int64_t a) ;

int NbdDevOpen(const char *dev_name, uint64_t size)
{
    int sp[2], fail_arr[NUM_OF_FD] = {0};
    int nbd;
    pid_t pid;

    do
    {
        if( -1 == (nbd = open(dev_name, O_RDWR)) ||
            -1 == socketpair(AF_UNIX, SOCK_STREAM, 0, sp) ||
            -1 == ioctl(nbd, NBD_SET_BLKSIZE, 1024) ||
            -1 == ioctl(nbd, NBD_SET_SIZE, size) ||
            -1 == ioctl(nbd, NBD_CLEAR_SOCK) ||
            -1 == ioctl(nbd, NBD_CLEAR_QUE) ||
            -1 == ioctl(nbd, NBD_SET_SOCK, sp[1]) ||
            -1 == ioctl(nbd, NBD_SET_TIMEOUT,30) ||
            -1 == (pid = fork())
        )
        {
            break;
        } 

        if(0==pid)
        {
            close(sp[0]);
        
            if(-1 ==ioctl(nbd, NBD_DO_IT))
            {
                break;
            }
        }

        close(sp[1]);

        return sp[0];

    } 
    while (0);

    fail_arr[0]=sp[0];
    fail_arr[1]=sp[1];
    fail_arr[2]=nbd;


    CleanUp(fail_arr, NUM_OF_FD);
    return -1;
   
}

NbdRequest *NbdRequestRead(int sock)
{
    struct nbd_request nbd_request = {0};
    nbd_message_t *message=NULL;

    read_all(sock,(char *)&nbd_request, sizeof(nbd_request));
    

    assert(nbd_request.magic == htonl(NBD_REQUEST_MAGIC));

    message = malloc(offsetof(nbd_message_t,buffer)+ntohl(nbd_request.len));

    message->reply.error=htonl(0);
    message->reply.magic= htonl(NBD_REPLY_MAGIC);
    memcpy(message->reply.handle, nbd_request.handle, sizeof(message->reply.handle));

    message->nbd_req.dataLen = ntohl(nbd_request.len);
    message->nbd_req.offset = ntohll(nbd_request.from);
    message->nbd_req.reqType = ntohl(nbd_request.type);
    message->nbd_req.dataBuf = message->buffer;

    message->nbd_sock = sock;

    if(NBD_CMD_WRITE==message->nbd_req.reqType)
    {
       read_all(sock, message->buffer, message->nbd_req.dataLen);
    }

    return (NbdRequest *)message;
}

void NbdRequestDone(NbdRequest *req, int error)
{
    nbd_message_t *message = (nbd_message_t *)req;
    size_t len = sizeof(message->reply); 

    message->reply.error=htonl(error);

    if(NBD_CMD_READ==message->nbd_req.reqType)
    {
       len += message->nbd_req.dataLen;
    }

    write_all(message->nbd_sock,(char *)&message->reply, len);

    free(message);
}

int CleanUp(int arr[], size_t size)
{
    int i=0;

    for(i=0; i<size; ++i)
    {
       if( -1 ==close(arr[i]))
       {
            return -1;
       }
    }

    return 0;    
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

u_int64_t ntohll(u_int64_t a) 
{
  u_int32_t lo = a & 0xffffffff;
  u_int32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t) lo) << 32U | hi;
}

static int write_all(int fd, char* buf, size_t count)
{
  int bytes_written;

  while (count > 0) {
    bytes_written = write(fd, buf, count);
    assert(bytes_written > 0);
    buf += bytes_written;
    count -= bytes_written;
  }
  assert(count == 0);

  return 0;
}
