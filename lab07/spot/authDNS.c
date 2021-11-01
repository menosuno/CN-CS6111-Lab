#include "headers.h"
#include <pthread.h>

#define AUTH1 9090
#define AUTH2 9190
#define MAX 100

typedef struct{
    int sock;
    struct sockaddr_in addr;
    int auth;
}arg;

void DNS_lookup(char* host,char* IP,int auth){
    FILE* data;
    if(auth==AUTH1)
         data=fopen("Auth_DNS1.txt","r");
    else
        data=fopen("Auth_DNS2.txt","r");

    char linebuff[MAX];
    char temp[MAX];
    while(fgets(linebuff,MAX,data)!=0){
        strcpy(temp,linebuff);
        char* tok=strtok(temp," ");
       // printf("%s-%s-%s\n",linebuff,tok,host);
        if(strncmp(host,tok,strlen(tok))==0){
            char* iptemp=strtok(NULL,"\n");
            //printf("IP addr of %s:%s\n",host,iptemp);
            strcpy(IP,iptemp);
            return;
        }
    }
    printf("IP not found\n");
    strcpy(IP,"NF");
    return;
}

void* chat(void* argm){

    arg ARG=*(arg*)argm;
    int sockfd=ARG.sock;
    struct sockaddr_in cliaddr=ARG.addr;
    int auth=ARG.auth;

    while(1){
        char query[MAX];
        int sendb,recvb;

        int size=sizeof(cliaddr);
        recvb=recvfrom(sockfd,(char*)query,sizeof(query),0,(struct sockaddr*)&cliaddr,&size);
        query[recvb]='\0';
        
        char host_name[MAX];
        strcpy(host_name,query);
        printf("\nfrom TLD server,host name:%s\n",host_name);

        char IP[MAX];
        DNS_lookup(host_name,IP,auth);
        sendb=sendto(sockfd,(const char*)IP,strlen(IP),0,(const struct sockaddr*)&cliaddr,size);
        printf("5.IP addr sent to Intermediate Name server\n");
    }
    return NULL;
}



int main(){
    int authsock1,authsock2;
    struct sockaddr_in authaddr1,authaddr2,cliaddr;

    bzero(&authaddr1,sizeof(authaddr1));
    bzero(&authaddr2,sizeof(authaddr2));
    bzero(&cliaddr,sizeof(cliaddr));

    //create socket
    authsock1=socket(AF_INET,SOCK_DGRAM,0);
    authsock2=socket(AF_INET,SOCK_DGRAM,0);

    //fill socket addr
    authaddr1.sin_family=AF_INET;
    authaddr1.sin_addr.s_addr=htonl(INADDR_ANY);
    authaddr1.sin_port=htons(AUTH1);


    authaddr2.sin_family=AF_INET;
    authaddr2.sin_addr.s_addr=htonl(INADDR_ANY);
    authaddr2.sin_port=htons(AUTH2);


    if((bind(authsock1,(const struct sockaddr*)&authaddr1,sizeof(authaddr1)))<0){
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    if((bind(authsock2,(const struct sockaddr*)&authaddr2,sizeof(authaddr2)))<0){
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    printf("Authoritative Name DNS server running at PORT %d\n",AUTH1);
    printf("Authoritative Name DNS server running at PORT %d\n",AUTH2);

    pthread_t t1,t2;
    arg ARG1;
    ARG1.sock=authsock1;
    ARG1.addr=cliaddr;
    ARG1.auth=AUTH1;
    pthread_create(&t1,NULL,chat,(void*)&ARG1);
    arg ARG2;
    ARG2.sock=authsock2;
    ARG2.addr=cliaddr;
    ARG2.auth=AUTH2;
    pthread_create(&t2,NULL,chat,(void*)&ARG2);

    //chat(authsock1,cliaddr,AUTH1);
    //chat(authsock2,cliaddr,AUTH2);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    close(authsock1);
    close(authsock2);
}