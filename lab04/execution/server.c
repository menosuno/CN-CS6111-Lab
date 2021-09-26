#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
#define N 7
#define SIZE 300
#define port 6071
#define x 50
int cli_count = 0;
typedef struct
{
 int index;
 int sockid;
 struct sockaddr_in cli_addr;
 int clilen;
}client;
client clients[N+1];
pthread_t tid[N+1];
typedef struct
{
 int roll;
 int sem;
 char name[x];
 int marks;
}student;
typedef struct
{
 char method[x];
 char version[x];
 char accept[x];
 char connection[x];
 char useragent[x];
 student stu;
}request;
typedef struct
{
 int status;
 char statusmsg[x];
 char version[x];
 char connection[x];
 char content[x];
 student stu;
}response;
int n = 0;
student stud[SIZE];
void * network(void* clidetail)
{
 client *clidet = (client*)clidetail;
 int id = clidet->index;
 int cli_sock = clidet->sockid;
 request req;
 printf("Client is connected to server\n");
 while(1){
int check = 0;
int recieve = recv(cli_sock,&req,sizeof(req),0);
if( check == 0)
{
printf("Client request headers\n\n");
printf("%s\t%s\n",req.method,req.version);
printf("useragent - %s\n",req.useragent);
printf("accept - %s\n",req.accept);
printf("connection - %s\n",req.connection);
printf("\n");
check = 1;
}
response resp ={200,"ok","HTTP/1.1"};
strcpy(resp.connection,"Keep alive");
strcpy(resp.content,"Text/Html");
int reg = req.stu.roll;
int semno = req.stu.sem;
if(reg == -1)
{
resp.stu.roll = -1;
send(cli_sock,&resp,sizeof(resp),0);
printf("Client disconnected\n");
close(cli_sock);
((client*)clidetail)->sockid = 0;
printf("Socket disconnected\n");
printf("\n");
return NULL;
}
int flag = 0;
for(int i = 0;i < n;i++)
{
if(stud[i].roll == reg && stud[i].sem == semno)
{
flag = 1;
resp.stu = stud[i];
send(cli_sock,&resp,sizeof(resp),0);
}
}
if(flag == 0)
{
resp.stu.roll = -1000;
send(cli_sock,&resp,sizeof(resp),0);
}
 }
 return NULL;
}
int main()
{
 struct sockaddr_in serveraddr;
 int server_socket = socket(AF_INET,SOCK_STREAM,0);
 if(server_socket < 0)
 {
 printf("error in socket creation\n");
 return 0;
 }
 printf("Socket created\n");
 serveraddr.sin_family = AF_INET;
 serveraddr.sin_port = ntohs(port);
 serveraddr.sin_addr.s_addr=INADDR_ANY;
 int ret = bind(server_socket,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
 if(ret < 0)
 {
 printf("error in binding\n");
 return 0;
 }
 printf("Binding succesful\n");
for(int i = 0;i < N+1;i++)
 {
 clients[i].sockid = 0;
 }
 printf("Student details for 3 semesters:\n");
 printf("Enter the number of students:\n");
 scanf("%d",&n);
 n = 3*n;
 for(int i = 0;i < n;i++)
 {
 printf("Enter roll no\n");
 scanf("%d",&stud[i].roll);
 printf("Enter sem number\n");
 scanf("%d",&stud[i].sem);
 printf("Enter name\n");
 scanf("%s",stud[i].name);
 printf("Enter marks\n");
 scanf("%d",&stud[i].marks);
 }
 if(listen(server_socket,N) < 0)
 {
 printf("error in listening\n");
 return 0;
 }
 printf("Listening...\n");
 int ind = 0;
 while(1)
 {
 for(int i = 0;i<N+1;i++)
 {
 if(clients[i].sockid == 0)
 {
 ind = i;
 break;
 }
 }
 clients[ind].sockid = accept(server_socket,(struct sockaddr*)&clients[ind].cli_addr,&clients[ind].clilen);
 clients[ind].index = ind;
 pthread_create(&tid[ind],NULL,network,(void*)&clients[ind]);
 cli_count++;
 }
 for(int i = 0;i < N;i++)
 {
 pthread_join(tid[i],NULL);
 }
 close(server_socket);
 return 0;
}
