#include "../header.h" 

int main() { 
    int client_ctl_fd, client_data_fd, server_data_fd, opt = 1; 
    client_ctl_fd = socket(AF_INET, SOCK_STREAM, 0); 
    client_data_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if(client_ctl_fd < 0 || client_data_fd < 0) { 
        perror("socket"); 
        exit(EXIT_FAILURE); 
    } 
    if(setsockopt(client_data_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Sockets created.\n"); 
    struct sockaddr_in address; 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(DATA_PORT); 
    if(bind(client_data_fd, (struct sockaddr*)&address, sizeof(address))) { 
        perror("bind"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_port = htons(CONTROL_PORT); 
    if(connect(client_ctl_fd, (struct sockaddr*)&address, sizeof(address))) { 
        perror("connect"); 
        exit(EXIT_FAILURE); 
    } 
    printf("FTP control TCP connection established.\n"); 

    long int data_len; 
    char request[BUFSIZ]={0}, reply[30], buf[1]; 
    char *data, *arg, cmd[4]; 
    int err_code, req_len; 
    if(listen(client_data_fd, 1) < 0) { 
        perror("client"); 
        exit(EXIT_FAILURE); 
    } 
    while(1) { 
        printf("Enter FTP command:\n"); 
        fgets(request, BUFSIZ, stdin); 
        fflush(stdin); 
        if(request[strlen(request)-1]=='\n') 
            request[strlen(request)-1] = '\0'; 
        arg = getArgFromRequest(request); 
        strncpy(cmd, request, 4); 
        req_len = strlen(request); 
        printf("Sending request...\n"); 
        send(client_ctl_fd, &req_len, sizeof(int), 0); 
        send(client_ctl_fd, request, req_len, 0); 
        memset(request, 0, sizeof(request)); 
        recv(client_ctl_fd, reply, 30, 0); 
        printf("SERVER: %s\n", reply); 
        if(strncmp(reply, "221", 3) == 0) 
            break; 
        if(strncmp(reply, "200", 3) != 0) 
            continue; 
        printf("Waiting for server to establish FTP data TCP connection...\n"); 
        server_data_fd = accept(client_data_fd, NULL, NULL); 
        printf("FTP data TCP connection established.\n"); 
        printf("%s\n",cmd); 
        if(strncmp(cmd, "LIST", 4) == 0) { 
            if(recv(server_data_fd, buf, 1, MSG_PEEK | MSG_DONTWAIT) == 0) { 
                printf("Something went wrong, server disconnected.\n"); 
                close(server_data_fd); 
                continue; 
            } 
            recv(server_data_fd, &data_len, sizeof(long int), 0); 
            data = (char*)malloc(data_len*sizeof(char)); 
            recv(server_data_fd, data, data_len, 0); 
            printf("Directory contents of server:\n%s\n", data); 
            free(data); 
            close(server_data_fd); 
        } else if(strncmp(cmd, "RETR", 4) == 0) { 
            err_code = recvFile(server_data_fd, arg); 
            if(err_code != 0) { 
                printf("RETR.ERROR.CODE=%d\n", err_code); 
            } else { 
                printf("File \"%s\" retrieved from server.\n", arg); 
            } 
            close(server_data_fd); 
        } else { 
            err_code = sendFile(server_data_fd, arg); 
            if(err_code != 0) { 
                printf("STOR.ERROR.CODE=%d\n", err_code); 
            } else { 
                printf("File \"%s\" stored in server.\n",arg); 
            } 
            close(server_data_fd); 
        } 
    } 
    printf("FTP session terminated.\n"); 
    close(client_ctl_fd); 
    close(server_data_fd); 
    close(client_data_fd); 
    return 0; 
}
