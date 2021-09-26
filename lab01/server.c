#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
        char msg[200] = "Hello World\n";
        char buf[200];

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
        {
                printf("ERROR in socket creation\n");
        }

        printf("Socket established\n\n");

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(3005);
        server_address.sin_addr.s_addr = INADDR_ANY;

        int status = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

        if (status < 0)
        {
                printf("Error in establishing server\n\n");
                return -1;
        }

        printf("SERVER ESTABLISHED\n");
        printf("Listening on port %d\n\n", ntohs(server_address.sin_port));

        listen(server_socket, 1);

        int client_socket = accept(server_socket, NULL, NULL);

        send(client_socket, msg, sizeof(msg), 0);
        recv(client_socket, buf, sizeof(buf), 0);

        if (sizeof(buf) > 0)
        {
                printf("Message from client: %s\n", buf);
        }

        close(server_socket);

        return 0;
}
