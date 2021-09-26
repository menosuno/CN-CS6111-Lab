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
    int serverSocket,newSocket;
    struct sockaddr_in server_address,new_address;
    pid_t childpid;
    socklen_t serverLen = sizeof(server_address);
    socklen_t clientLen = sizeof(new_address);
    char msg[SIZE],buf[SIZE];
    serverSocket = socket(AF_INET,SOCK_STREAM,0);
    if(serverSocket<0)
    {
        perror("Can't create Socket..\n");
        return 1;
    }
    printf("Server socket created..\n");
    memset(&server_address,'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int b = bind(serverSocket,(struct sockaddr *)&server_address,serverLen);
    if(b<0)
    {
        perror("Binding error..\n");
        return 1;
    }
    if(listen(serverSocket,2)==0)
    {
        printf("Listening..\n");
    }
    else
    {
        perror("Can't listen to requests..\n");
        return 1;
    }
    while(1)
    {
        newSocket = accept(serverSocket,(struct sockadddr*)&new_address,&clientLen);
        if(newSocket<0)
        {
            perror("Can't accept requests..\n");
            return 1;
        }
        printf("Connection accepted from %d:%d\n",inet_ntoa(new_address.sin_addr),ntohs(new_address.sin_port));
        if((childpid=fork())==0)
        {
            close(serverSocket);
            // sprintf(buf,"Server says hello!");
            // send(newSocket, buf, strlen(buf), 0);
            // bzero(buf,sizeof(buf));
            while(1)
            {
                recv(newSocket,msg,sizeof(msg),0);
                if(strcmp(msg,"exit")==0)
                {
                    printf("Client %d disconnected from server..\n",inet_ntoa(new_address.sin_addr));
                    return 1;
                }
                else
                {
                    printf("Client %d: %s\n",ntohs(new_address.sin_port),msg);
                    //sprintf(buf,"Client %d: %s\n",ntohs(new_address.sin_port),msg);
                    sprintf(buf,"Server: %s\n",msg);
                    send(newSocket, buf, strlen(buf), 0);
                    bzero(buf,sizeof(buf));
                }
            }
        }
    }
    return 0;
}
