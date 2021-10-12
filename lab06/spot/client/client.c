#include "../header.h"

int main() { 
    int opt = 1; 
    struct sockaddr_in cliaddr; 
    int ctl_socket = socket(AF_INET,SOCK_STREAM,0); 
    if(ctl_socket < 0) { 
        printf("error in socket creation!!\n");
        return 0; 
    }
    printf("Socket created\n");
    cliaddr.sin_family = AF_INET; 
    cliaddr.sin_port = ntohs(port1); 
    cliaddr.sin_addr.s_addr = INADDR_ANY; 
    int ret = connect(ctl_socket,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
    if(ret < 0) { 
        printf("error in connection!!\n"); 
        return 0;
    } 
    while(1) { 
        char buff[SIZE],name[SIZE];
        printf("Enter any of the FTP commands : RETR, STOR, QUIT\n");
        scanf("%s",buff); 
        send(ctl_socket,buff,sizeof(buff),0);
        if(strcmp(buff,"QUIT") == 0)
            break;
        printf("Enter the filename: \n");
        scanf("%s",name);
        send(ctl_socket,name,sizeof(name),0);
        struct sockaddr_in cliaddr;
        int cli_serv_socket = socket(AF_INET,SOCK_STREAM,0);
        if(setsockopt(cli_serv_socket,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))) { 
            printf("error in setsockopt!!\n");
            return 0;
        } 
        if(cli_serv_socket < 0) { 
            printf("error in data socket creation!!\n");
            return 0;
        } 
        printf("Socket created\n"); 
        cliaddr.sin_family = AF_INET;
        cliaddr.sin_port = ntohs(port2); 
        cliaddr.sin_addr.s_addr = INADDR_ANY; 
        int status = bind(cli_serv_socket,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
        if(status < 0) { 
            printf("error in binding!!\n");
            return 0;
        } 
        if(listen(cli_serv_socket,N) < 0) { 
            printf("error in listening!!\n");
            return 0;
        } 
        if(strcmp(buff,"RETR") == 0) { 
            int data_sock = accept(cli_serv_socket,NULL,NULL);
            if(ret < 0) { 
                printf("error in connection!!\n");
                return 0;
            }
            printf("Connected established\n");
            int err = recvfile(data_sock,name);
            if(err == 1) { 
                printf("error in uploading file!!\n");
            } else { 
                printf("Received file : %s\n",name);
            } 
            sleep(1);
            close(data_sock); 
        } else if(strcmp(buff,"STOR") == 0) { 
            int data_sock = accept(cli_serv_socket,NULL,NULL);
            if(ret < 0) { 
                printf("error in connection!!\n");
                return 0;
            } 
            printf("Connected established\n");
            int err = sendFile(data_sock,name);
            if(err == 1) { 
                printf("error in sending file!!\n");
            } else { 
                printf("File has been successfully sent\n");
            }
            sleep(1);
            close(data_sock); 
        } else {
            printf("Command invalid\n"); 
        } 
        close(cli_serv_socket); 
    } 
    close(ctl_socket); 
    return 0; 
}

