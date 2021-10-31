#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5689
#define WIN_SIZE 4
#define SEQ_MAX 8
#define N 5
#define DATA_MAX 256
#define RECV_BUF_SIZE 3
#define SEND_BUF_SIZE 5

typedef struct tcp_packet {
	int seq_no;
	bool ack;
	bool dropped;
	int advertisedWindow;
	char data[N];
} TCP_Packet;
