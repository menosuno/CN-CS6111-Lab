#include "../header.h" 

int main() { 
    int server_ctl_fd, client_ctl_fd, opt = 1; 
    server_ctl_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if(server_ctl_fd < 0) { 
        perror("control socket"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Socket created.\n"); 
    if(setsockopt(server_ctl_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    struct sockaddr_in address; 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(CONTROL_PORT); 
    if(bind(server_ctl_fd, (struct sockaddr*)&address, sizeof(address))) { 
        perror("bind"); 
        exit(EXIT_FAILURE); 
    } 
    if(listen(server_ctl_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    printf("FTP control socket listening on port %d.\n", CONTROL_PORT); 
    client_ctl_fd = accept(server_ctl_fd, NULL, NULL); 
    if(client_ctl_fd < 0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    printf("FTP control TCP connection established.\n"); 
    char buf[1], reply[30]; 
    char *req, *arg; 
    int req_len, cmd_type, err_code; 
    address.sin_port = htons(DATA_PORT); 
    while(1) { 
        if(recv(client_ctl_fd, buf, 1, MSG_PEEK | MSG_DONTWAIT) == 0) { 
        printf("Error: Client disconnected.\n"); 
        break; 
    } 
    printf("Waiting for FTP request...\n"); 
    recv(client_ctl_fd, &req_len, sizeof(int), 0); 
    req = (char*)malloc(req_len*sizeof(char)); 
    recv(client_ctl_fd, req, req_len, 0); 
    printf("RECEIVED REQUEST: %s\n", req); 
    memset(reply, 0, 30); 
    strcpy(reply, "200 Command okay"); 
    arg = getArgFromRequest(req); 

    if(strncmp(req, "LIST", 4)==0) { 
        cmd_type = 1; 
    } else if((strncmp(req, "RETR", 4)==0)&&(arg)) { 
        cmd_type = 2; 
    } else if((strncmp(req, "STOR", 4)==0)&&(arg)) { 
        cmd_type = 3; 
    } else if(strncmp(req, "QUIT", 4)==0) { 
        cmd_type = 4; 
    strcpy(reply, "221 Closing connection"); 
    } else { 
        cmd_type = 5; 
        printf("COMMAND NOT IMPLEMENTED: %.*s\n", 4, req); 
        strcpy(reply, "502 Command not implemented"); 
    } 
    strcat(reply, "\0"); 
    send(client_ctl_fd, reply, 30, 0); 
    printf("Response sent\n"); 
    free(req); 

    if(cmd_type > 4) 
        continue; 
    if(cmd_type < 4) { 
        printf("Waiting for client to setup TCP connection...\n"); 
    } 
    int server_data_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if(server_data_fd < 0) { 
        perror("data socket"); 
        exit(EXIT_FAILURE); 
    } 
    switch(cmd_type){ 
        case 1: { 
            if(connect(server_data_fd, (struct sockaddr*)&address, sizeof(address))) { 
                perror("connect"); 
                break; 
            } 
            printf("FTP data TCP connection established\n"); 
            err_code = sendDirConList(server_data_fd); 
            if(err_code != 0) { 
                printf("\nLIST.ERROR.CODE=%d\n", err_code); 
            } 
            else { 
                printf("List of contents sent to client.\n"); 
            } 
            sleep(1); 
            close(server_data_fd); 
            server_data_fd = -1; 
            break; 
        } 
        case 2: { 
            if(connect(server_data_fd, (struct sockaddr*)&address, sizeof(address))) { 
                perror("connect"); 
                break; 
            } 
            printf("FTP data TCP connection established.\n"); 
            err_code = sendFile(server_data_fd, arg); 
            if(err_code != 0) { 
                printf("\nRETR.ERROR.CODE=%d\n", err_code); 
            } else { 
                printf("File \"%s\" sent to client.\n", arg); 
            } 
            sleep(1); 
            close(server_data_fd); 
            server_data_fd = -1; 
            break; 
        } 
        case 3: { 
            if(connect(server_data_fd, (struct sockaddr*)&address, sizeof(address))) { 
                perror("connect"); 
                break; 
            } 
            printf("FTP data TCP connection established.\n"); 
            err_code = recvFile(server_data_fd,arg); 
            if(err_code != 0) { 
                printf("\nSTOR.ERROR.CODE=%d\n", err_code); 
            } else { 
                printf("File \"%s\" received from client.\n", arg); 
            } 
            sleep(1); 
            close(server_data_fd); 
            server_data_fd = -1; 
            break; 
        } 
        case 4: { 
            close(server_data_fd); 
            close(client_ctl_fd); 
            close(server_ctl_fd); 
            return 0; 
        } 
    } 
    } 
    free(req); 
    return 1; 
} 
