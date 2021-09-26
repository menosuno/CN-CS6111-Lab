#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define PORT 3071
#define SIZE 256

int main()
{
    int clientSocket;
    struct sockaddr_in server_address;
    char msg[SIZE],buf[SIZE];
    clientSocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientSocket<0)
    {
        perror("Can't create Socket..\n");
        return 1;
    }
    printf("Client socket created..\n");
    memset(&server_address,'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int c = connect(clientSocket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(c<0)
    {
        perror("Can't connect to the server..\n");
        return 1;
    }
    printf("Connected to the server..\n");
    while(1)
    {
        printf("Enter your message and press enter..\n");
        scanf("%s",buf);
        send(clientSocket,buf,sizeof(buf),0);
        if(strcmp(buf,"exit")==0)
        {
            close(clientSocket);
            printf("Disconnected from the server..\n");
            return 1;
        }
        if(recv(clientSocket,msg,sizeof(msg),0)<0)
        {
            perror("Can't receive message..\n");
            return 1;
        }
        printf("%s\n",msg);
        bzero(msg,sizeof(msg));
    }
    return 0;
}
