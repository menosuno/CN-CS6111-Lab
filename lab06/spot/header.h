#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<sys/sendfile.h>

#define port1 30071
#define port2 30072
#define N 5 
#define SIZE 300

int recvfile(int sock,char* filename) {
	FILE* fp = fopen(filename,"w"); 
	if(!fp) { 
		printf("error in opening file!!!\n"); 
		return 1; 
	} 
	char buff[15] = {0};
	recv(sock,buff,15,0);
	fwrite(buff,1,15,fp);
	printf("First part of the file has been received\n");
	char buff1[1]={0};
	int bytes=recv(sock,buff1,1,0);
	while(bytes>0) {
		fwrite(buff1,1,1,fp);
		bytes=recv(sock,buff1,1,0);
	}
	printf("Second part of the file has been received\n");
	fclose(fp); 
	return 0;
} 

int sendFile(int sock,char *filename) { 
	FILE* fp = fopen(filename,"r"); 
	if(!fp)  { 
		printf("error in opening file!!\n"); 
		return 1;
	}

	char buff[15]={0};
	fread(buff,1,15,fp); 
	send(sock,buff,15,0);
	printf("First part of the file has been sent\n");

	char buff1[1]={0};
	fread(buff1,1,1,fp);
	while(!feof(fp)) {
		send(sock,buff1,1,0);
		fread(buff1,1,1,fp);
	}
	printf("Second part of the file has been sent\n");
	fclose(fp); 
	return 0; 
}
