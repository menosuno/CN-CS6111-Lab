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
#define N 5

int client_sockets[N + 1];

void *manageClients(void *client_socket)
{
    int c_socket = *((int *)client_socket);
    printf("Thread for client_id-%d created\n", c_socket);

    int cur_index = 0;
    for (int i = 0; i <= N; ++i)
    {
        if (c_socket == client_sockets[i])
        {
            cur_index = i;
            break;
        }
    }

    char buf[MAX] = "__SERVER__: Connection established\n";
    send(c_socket, buf, sizeof(buf), 0);

    while (1)
    {
        int len = recv(c_socket, buf, sizeof(buf), 0);
        buf[len] = '\0';

        if (strlen(buf) > 0)
        {

            printf("RECEIVED MSG: %s\n", buf);

            // If client leaves
            if (strcmp(buf, "EXIT") == 0)
            {
                send(c_socket, "TERM", sizeof("TERM"), 0);

                printf("socket fd to be closed: %d\n", c_socket);

                close(c_socket);
                *(int *)client_socket = 0;

                printf("Remaining connections:\n");
                for (int i = 0; i <= N; ++i)
                {
                    printf("client-%d: %d\n", i, client_sockets[i]);
                }

                printf("Client disconnected\n\n");
                return NULL;
            }
            else
            {
                /*
                 * Transmmit message to all other clients
                 */
                for (int i = 0; i <= N; ++i)
                {
                    if (client_sockets[i] != 0)
                    {
                        char msg[MAX];
                        msg[0] = '\0';
                        if (i == cur_index)
                        {
                            sprintf(msg, "SENT: %s", buf);
                            send(client_sockets[i], msg, strlen(msg), 0);
                            continue;
                        }
                        sprintf(msg, "MSG FROM CLIENT-%d: %s", cur_index, buf);

                        printf("Msg sent to %d: \"%s\" \n", i, msg);
                        send(client_sockets[i], msg, strlen(msg), 0);
                    }
                }
            }
        }
        buf[0] = '\0';
    }

    return NULL;
}

int main()
{
    pthread_t client_threads[N];

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Error in creating socket\n\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int status = bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (status < 0)
    {
        printf("Error in establishing server\n\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= N; ++i)
    {
        client_sockets[i] = 0;
    }

    if (listen(server_socket, N) < 0)
    {
        printf("Error while listening\n\n");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", PORT);

    int index = 0;
    while (1)
    {

        index = N;
        //Find a free socket from client pool
        for (int i = 0; i < N; ++i)
        {
            if (client_sockets[i] == 0)
            {
                index = i;
                break;
            }
        }

        client_sockets[index] = accept(server_socket, NULL, NULL);
        printf("INDEX: %d\n", index);

        printf("Current Connections:\n");
        for (int i = 0; i <= N; ++i)
        {
            printf("poolIndex_%d: %d\n", i, client_sockets[i]);
        }

        pthread_create(&client_threads[index], NULL, manageClients, (void *)&client_sockets[index]);
    }

    for (int i = 0; i < N; ++i)
    {
        pthread_join(client_threads[i], NULL);
    }

    return 0;
}
