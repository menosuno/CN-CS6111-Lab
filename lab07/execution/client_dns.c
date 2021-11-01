#include "headers.h"

#define DNS_PORT 3071
#define MAX 100 

typedef struct{
    char name[MAX];
    char value[MAX];
    char type[MAX];
    int ttl;
} RR;

typedef struct{
    char header[MAX];
    int n_ques;
    int n_ans;
    int n_auth;
    int n_add;
    RR ques[MAX];
    RR ans[MAX];
    RR auth[MAX];
    RR add[MAX];
} DNS_msg;

void chat(int sockfd,struct sockaddr_in servaddr){
    char host[MAX];
    printf("Enter hostname to get IP address,(QUIT to exit)\n");
    while(1){
        bzero(&host,sizeof(host));
        printf(">>Enter hostname:");
        scanf("%[^\n]",host);
        if(strncmp(host,"QUIT",4)==0){
            printf("Exited..\n");
            return;
        }

        RR ques;
        strcpy(ques.name,host);
        strcpy(ques.type,"A");

        DNS_msg query;
        strcpy(query.header,"DNS query,Recursive\n");
        query.n_ques=1;
        query.ques[0]=ques;
    
        int len=sendto(sockfd,(const char*)host,strlen(host),0,(const struct sockaddr*)&servaddr,sizeof(servaddr));
        if(len<0){
            perror("send error\n");
            exit(-1);
        }
        
        char IP[MAX];
        int size=sizeof(servaddr);
        len=recvfrom(sockfd,(char*)IP,sizeof(IP),0,(struct sockaddr*)&servaddr,&size);
        IP[len]='\0';
        printf("IP address:%s\n",IP);
        getchar();
    }
}

int main(){
   int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
    servaddr.sin_port = htons(DNS_PORT);


    chat(sockfd,servaddr);

}