#ifndef COMMON_OPERATIONS
#define COMMON_OPERATIONS

int createSocket();
int sendPacket(int skt, TCP_Packet* pkt);
int recvPacket(int skt, TCP_Packet* pkt);
int wrap(int seqNo, int range);

void defineAddress(struct sockaddr_in* addr, int port);

int readFile(FILE* fptr, char buf[], int buf_len);
void writeFile(char filename[], char data[]);

#endif

