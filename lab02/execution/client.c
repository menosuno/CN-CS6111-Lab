#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 3071
#define MAX 256

int client_socket;

void *receiveMessage(void *client_socket)
{
    int *temp = (int *)client_socket;
    int socket = *temp;

    char buf[MAX];
    buf[0] = '\0';

    while (1)
    {

        int len = recv(socket, buf, sizeof(buf), 0);
        buf[len] = '\0';

        if (strlen(buf) > 0)
        {
            // If server has agreed to disconnect
            if (strcmp(buf, "TERM") == 0)
            {
                break;
            }

            printf("%s\n", buf);
        }

        buf[0] = '\0';
    }

    return NULL;
}

int main()
{
    pthread_t receive_thread;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Error in creating socket\n\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket created-%d\n", socket);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int status = connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (status < 0)
    {
        printf("Error in connecting to server\n\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected to chat server!\n\n");

    printf("Type in any message and press enter to send\n");

    pthread_create(&receive_thread, NULL, receiveMessage, (void *)&client_socket);

    /*
     * Send message part
     */
    char msg[MAX];
    while (1)
    {
        scanf("%[^\n]%*c", msg);
        if (strlen(msg) > 0)
        {
            send(client_socket, msg, strlen(msg), 0);

            if (strcmp(msg, "EXIT") == 0)
            {
                break;
            }
        }
        msg[0] = '\0';
    }

    pthread_join(receive_thread, NULL);
    printf("Closed thread\n");
    return 0;
}
