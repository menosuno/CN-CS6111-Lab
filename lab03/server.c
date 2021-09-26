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

#define port 4071

#define x 50

// global variable

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
    char gender;

    int marks[3];

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

    char datetime[x];

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

    printf("client is connected to server\n");

    int check = 0;


    int recieve = recv(cli_sock,&req,sizeof(req),0);

    if( check == 0)

    {

            printf("client request headers\n\n");

            printf("%s\t%s\n",req.method,req.version);

            printf("useragent - %s\n",req.useragent);

            printf("accept - %s\n",req.accept);

            printf("connection - %s\n",req.connection);

            printf("\n");

            check = 1;

    }

    time_t t = time(NULL);

    struct tm *tm = localtime(&t);


    response resp ={200,"ok","HTTP/1.1"};

    strcpy(resp.datetime,asctime(tm));

    strcpy(resp.connection,"Close");

    strcpy(resp.content,"Text/Html");

    int reg = req.stu.roll;

    int semno = req.stu.sem;

    if(reg == -999)

    {

            resp.stu.roll = -999;

            send(cli_sock,&resp,sizeof(resp),0);

            printf("client is disconnected from server\n");

            close(cli_sock);

            ((client*)clidetail) -> sockid = 0;

            printf("socket disconnected\n");

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

            break;

        }

    }

    if(flag == 0)

    {

        resp.stu.roll = -1000;

        send(cli_sock,&resp,sizeof(resp),0);

    }

    printf("client is disconnected from server\n");

    close(cli_sock);

    ((client*)clidetail)->sockid = 0;

    printf("socket disconnected\n");

    printf("\n");

    return  NULL;


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

    printf("socket created\n");



    serveraddr.sin_family = AF_INET;

    serveraddr.sin_port = ntohs(port);

    serveraddr.sin_addr.s_addr = INADDR_ANY;



    int ret = bind(server_socket,(struct sockaddr*)&serveraddr,sizeof(serveraddr));


    if(ret < 0)

    {

        printf("error in binding\n");

        return 0;

    }

    printf("binding succesful\n");



    for(int i = 0;i < N+1;i++)

    {

        clients[i].sockid = 0;

    }

    printf("enter student details\n");

    printf("enter the number of student(1 student - 3 sem details) details\n");

    scanf("%d",&n);

    n = 3*n;

    for(int i = 0;i < n;i++)

    {

        printf("enter roll no\n");

        scanf("%d",&stud[i].roll);

        printf("enter sem number\n");

        scanf("%d",&stud[i].sem);

        printf("enter name\n");

        scanf("%s",stud[i].name);

        printf("enter gender(f or m)\n");

        getchar();

        scanf("%c",&stud[i].gender);

        printf("enter 3 marks\n");

        for(int j = 0;j < 3;j++)

                scanf("%d",&stud[i].marks[j]);

    }



    if(listen(server_socket,N) < 0)

    {

        printf("error in listening\n");

        return 0;

    }

    printf("listening to port %d\n\n",port);

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



    for(int  i = 0;i < N;i++)

    {

        pthread_join(tid[i],NULL);

    }

    close(server_socket);

    return 0;

}

