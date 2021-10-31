#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <pthread.h>

#define PORT 9797
#define SIZE 256
#define N 4
#define seq_num_max 7

typedef struct tcpHeader
{
    int drop_flag;
    int ack_flag;
    int seq_num;
} tcpHeader;

typedef struct tcpPacket
{
    tcpHeader header;
    char data[SIZE];
} tcpPacket;

int nextSeqNum(int num)
{
    if (num >= seq_num_max)
    {
        return 0;
    }

    return (num + 1);
}