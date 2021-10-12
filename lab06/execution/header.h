#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<netinet/in.h> 
#include<pthread.h> 
#include<errno.h> 
#include<dirent.h> 
#define DATA_PORT 3071
#define CONTROL_PORT 30071

int sendDirConList(int sock) { 

	DIR *d; 
	struct dirent *dir; 
	char cwd[BUFSIZ] = {0}; 
	char *p; 
	p = getcwd(cwd, BUFSIZ); 
	if(!p) { 
		perror("getcwd"); 
		return 1; 
	} 
	d = opendir(cwd); 
	if(!d) { 
		perror("opendir"); 
		return 2; 
	} 
	long int file_list_len; 
	char file_list[BUFSIZ] = {0}; 
	while((dir = readdir(d)) != NULL) { 
		if(dir->d_type == DT_REG) { 
			strcat(file_list, dir->d_name); 
			strcat(file_list, "\n"); 
		} 
	} 
	file_list_len = strlen(file_list); 
	send(sock, &file_list_len, sizeof(long int), 0); 
	send(sock, &file_list, file_list_len, 0); 
	closedir(d); 
	return 0; 

} 

int recvFile(int sock, char* filename) { 
	FILE *fp = fopen(filename, "w"); 
	if(!fp) { 
		perror("fopen"); 
		return 1; 
	} 
	char recv_buffer[1] = {0}; 
	int nb = recv(sock, recv_buffer, 1, 0); 
	while (nb > 0) { 
		fwrite(recv_buffer, 1, 1, fp); 
		nb = recv(sock, recv_buffer, 1, 0); 
	} 
	fclose(fp); 
	return 0; 
} 

int sendFile(int sock, char* filename) { 
	FILE *fp = fopen(filename, "r"); 
	if(!fp) { 
		perror("fopen"); 
		return 1; 
	} 
	char send_buffer[1] = {0}; 
	fread(send_buffer, 1, 1, fp); 
	while(!feof(fp)) { 
		send(sock, send_buffer, 1, 0); 
		fread(send_buffer, 1, 1, fp); 
	} 
	fclose(fp); 
	return 0; 
} 

char* getArgFromRequest(char* request) { 
	char *arg = (char*)malloc(BUFSIZ*sizeof(char)); 
	int i=0, j; 
	while(request[i]!=' ' && request[i]!='\0') 
		i++; 
	if(request[i]=='\0') 
		return NULL; 
	strncpy(arg, request+i+1, (strlen(request)-i)); 
	return arg; 
} 
