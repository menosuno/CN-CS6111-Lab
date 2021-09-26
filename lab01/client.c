#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(3005);
        server_address.sin_addr.s_addr = INADDR_ANY;

        int connection_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
        if (connection_status < 0)
        {
                printf("ERROR in connecting\n");
                return -1;
        }

        char buf[200];
        recv(client_socket, &buf, sizeof(buf), 0);

        if (sizeof(buf) > 0)
        {
                printf("Received message: %s", buf);
                char response[200] = "Message received\n";
                send(client_socket, response, sizeof(response), 0);
        }

        close(client_socket);

        return 0;
}
