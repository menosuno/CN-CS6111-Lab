#include "def.h"

int createSocket()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("SOCKET: ");
		exit(EXIT_FAILURE);
	}

	return sock;
}

void defineAddress(struct sockaddr_in* addr, int port)
{
	addr->sin_family = AF_INET;
	addr->sin_port = htons(PORT);
	addr->sin_addr.s_addr = INADDR_ANY;
}

int sendPacket(int skt, TCP_Packet* pkt)
{
	int len = send(skt, pkt, sizeof(*pkt), 0);
	if (len < 0) {
		perror("SEND: ");
		exit(EXIT_FAILURE);
	}

	return len;
}	

int recvPacket(int skt, TCP_Packet* pkt)
{
	int len = recv(skt, pkt, sizeof(TCP_Packet), 0);
	if (len < 0) {
		perror("RECV: ");
		exit(EXIT_FAILURE);
	}

	return len;
}

int wrap(int seqNo, int range)
{
	return (seqNo % range);
}

int readFile(FILE* fptr, char buf[], int buf_size)
{
	if (buf_size < 2) {
		printf("Buffer size should greater than 1!\n");
		exit(EXIT_FAILURE);
	}

	int len = fread(buf, sizeof(char), (buf_size - 1), fptr);
	if (len < 0) {
		perror("FREAD: ");
		exit(EXIT_FAILURE);
	}
	else if (len != EOF) {
		buf[len] = '\0';
	}

	return len;
}

void writeFile(char filename[], char data[])
{
	FILE* fptr;
	fptr = fopen(filename, "a");
	if (!fptr) {
		perror("WRITE FOPEN: ");
		exit(EXIT_FAILURE);
	}

	int len = fwrite(data, sizeof(char), strlen(data), fptr);
	if (len < 0) {
		perror("FWRITE: ");
		exit(EXIT_FAILURE);
	}

	fclose(fptr);
}

