#include "../header.h"

int main() { 
    struct sockaddr_in serveraddr;
    int opt = 1; 
    int server_socket = socket(AF_INET,SOCK_STREAM,0); 
    if(server_socket < 0) { 
        printf("error in socket creation!!\n");
        return 0;
    } 
    if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR |SO_REUSEPORT,&opt,sizeof(opt))) {
        printf("error in setsockopt!!\n");
        return 0;
    } 
    printf("Socket Created\n");
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_port = ntohs(port1); 
    serveraddr.sin_addr.s_addr = INADDR_ANY; 
    int ret = bind(server_socket,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(ret < 0) { 
        printf("error in binding!!\n");
        return 0;
    } 
    printf("Binding successful\n");
    if(listen(server_socket,N) < 0) {
        printf("error in listening!!\n");
        return 0;
    }
    int ctl_sock = accept(server_socket,NULL,NULL);
    while(1) { 
        char buff[SIZE],name[SIZE]; 
        recv(ctl_sock,buff,sizeof(buff),0);
        if(strcmp(buff,"quit") == 0) { 
            printf("Disconnected\n");
            break;
        } 
        recv(ctl_sock,name,sizeof(name),0); 
        sleep(3);
        struct sockaddr_in cliaddr;
        int data_socket = socket(AF_INET,SOCK_STREAM,0); 
        if(data_socket < 0) { 
            printf("error in data socket creation!!\n");
            return 0;
        } 
        printf("Socket created\n");
        cliaddr.sin_family = AF_INET; 
        cliaddr.sin_port = ntohs(port2); 
        cliaddr.sin_addr.s_addr = INADDR_ANY; 
        if(strcmp(buff,"STOR")== 0) { 
            int ret = connect(data_socket,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
            if(ret < 0) { 
                printf("error in connection!!\n");
                return 0;
            } 
            printf("Connection established\n");
            int err = recvfile(data_socket,name); 
            if(err != 0) { 
                printf("error in uploading file!!\n"); 
            } else { 
                printf("Received file : %s\n",name);
            } 
            sleep(1); 
            close(data_socket); 
        } 
        else if(strcmp(buff,"RETR") == 0) { 
            int ret = connect(data_socket,(struct sockaddr*)&cliaddr,sizeof(cliaddr)); 
            if(ret < 0) { 
                printf("error in connection!!\n");
                return 0;
            } 
            printf("Connection established\n");
            int err = sendFile(data_socket,name);
            if(err != 0) { 
                printf("error in sending file!!\n");
            } else {
                printf("File has been successfully sent\n");
            }
            sleep(1);
            close(data_socket); 
        } 
    } 
    close(ctl_sock);
    close(server_socket); 
    return 0;
}
