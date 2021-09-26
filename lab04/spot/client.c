#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>

#define N 10
#define port 6071
#define SIZE 300
#define n 50

typedef struct
{
        int roll;
        int sem;
        char name[50];
        char gender;
        int marks[5];
}
student;

typedef struct
{
        char method[n];
        char version[n];
        char accept[n];
        char connection[n];
        char useragent[n];
        student stu;
}
request;

typedef struct
{
        int status;
        char statusmsg[n];
        char version[n];
        char datetime[n];
        char connection[n];
        char content[n];
        student stu;
}
response;

int main()
{
                struct sockaddr_in server_addr;
                int cli_sock = socket(AF_INET, SOCK_STREAM, 0);
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                server_addr.sin_addr.s_addr = INADDR_ANY;
                int ret = connect(cli_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

                if (ret < 0)
                {
                        printf("error in connection\n");
                        return 0;
                }
                printf("client connected to server\n");
                int reg;
                int semno;
                int c = 0;

                while(1)
                {
                        request req = { "GET", "HTTP/1.1", "TEXT", "Keep-Alive","Mozilla"};
                        printf("Enter the register number and semester number to get details and enter (-999 to stop \n");
                        scanf("%d", &req.stu.roll);
                        reg = req.stu.roll;

                        if (reg == -999)
                        {
                                break;
                        }
                        scanf("%d", &req.stu.sem);
                        c++;
                        send(cli_sock, &req,sizeof(req), 0);
                }

                while(c--)
                {
                        response resp;
                        int recieve = recv(cli_sock, &resp, sizeof(resp),0);
                        if (resp.stu.roll == -1000)
                        {
                                printf("SERVER RESPONSE DETAILS\n");
                                printf("%s\t%d\t%s\n", resp.version, resp.status, resp.statusmsg);
                                printf("Date : %s", resp.datetime);
                                printf("Content-type : %s\n", resp.content);
                                printf("Connection : %s\n\n", resp.connection);
                                printf("roll number not found\n");
                                continue;
                        }
                        else
                        {
                                printf("SERVER RESPONSE DETAILS\n");
                                printf("%s\t%d\t%s\n", resp.version, resp.status, resp.statusmsg);
                                printf("Date : %s", resp.datetime);
                                printf("Content-type : %s\n", resp.content);
                                printf("Connection : %s\n\n", resp.connection);
                                printf("Name\trollno\tSem\tGender\tmark1\tmark2\tmark3\tmark4\tmark5\n");
                                printf("%s\t%d\t%d\t%c\t%d\t%d\t%d\t%d\t%d\n", resp.stu.name, resp.stu.roll, resp.stu.sem, resp.stu.gender, resp.stu.marks[0], resp.stu.marks[1], resp.stu.marks[2], resp.stu.marks[3], resp.stu.marks[4]);
                                printf("\n");
                        }
                }

                close(cli_sock);
                printf("client disconnected from server\n\n");

                return 0;
}
