#include "headers.h"

#define TLD 7000
#define MAX 100


void DNS_lookup(char* host,char* IP){
    FILE* data=fopen("tld_DNS.txt","r");

    char linebuff[MAX];
    char temp[MAX];
    while(fgets(linebuff,MAX,data)!=0){
        strcpy(temp,linebuff);
        char* tok=strtok(temp," ");
        //printf("%s-%s-%s\n",linebuff,tok,host);
        if(strncmp(host,tok,strlen(tok))==0){
            char* iptemp=strtok(NULL,"\n");
            printf("IP addr of %s:%s\n",host,iptemp);
            strcpy(IP,iptemp);
            return;
        }
    }
    printf("IP not found\n");
    strcpy(IP,"NF");
    return;
}


void chat(int sockfd,struct sockaddr_in cliaddr){

    //recv:TLD
    //send:Auth
    while(1){
        char query[MAX];
        int sendb,recvb;
        int flag=0;

        int size=sizeof(cliaddr);
        recvb=recvfrom(sockfd,(char*)query,sizeof(query),0,(struct sockaddr*)&cliaddr,&size);
        query[recvb]='\0';

        //get hostname
        char host_name[MAX];
        char alias_name[MAX];
        char temp[MAX];
        strcpy(temp,query);
        char* tok=strtok(temp,".");
        tok=strtok(NULL,"\0");
        strcpy(host_name,tok);
        int len=strlen(host_name);
        host_name[len]='\0';
        printf("\nfrom ROOT server host name:%s\n",host_name);

        //find Name server to lookup
        char resp[MAX];
        DNS_lookup(host_name,resp);

        char Authip[MAX];
        int Authport;
        tok=strtok(resp,",");
        if(strncmp(tok,"CN",strlen(tok))==0){
            //alias name
            tok=strtok(NULL,"\0");
            printf("Alias of %s:%s\n",host_name,tok);
            strcpy(alias_name,tok);
            //lookup again
            bzero(&resp,sizeof(resp));
            DNS_lookup(alias_name,resp);
            tok=strtok(resp,",");
            flag=1;
          
        }
        strcpy(Authip,tok);
        tok=strtok(NULL,"\0");
        Authport=strtol(tok,NULL,10);
        printf("Auth IP:%s Port:%d\n",Authip,Authport);


        if(flag){
            char temp1[MAX];
            strcpy(temp1,query);
            query[0]='\0';
            char* tok=strtok(temp1,".");
            strcat(query,tok);
            tok=strtok(temp1,"\0");
            strcat(query,".");
            strcat(query,alias_name);

            if(strncmp(tok,host_name,strlen(tok))==0){
                strcat(query,alias_name);
            }
            printf("Alias query:%s\n",query);
        }

        //connect to the Auth server
        int Authsock;
        struct sockaddr_in Authaddr;
        Authsock= socket(AF_INET, SOCK_DGRAM, 0);
        bzero(&Authaddr, sizeof(Authaddr));
        Authaddr.sin_family = AF_INET;
        Authaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
        Authaddr.sin_port = htons(Authport);

        int rsize=sizeof(Authaddr);
        sendb=sendto(Authsock,(const char*)query,strlen(query),0,(const struct sockaddr*)&Authaddr,rsize);
        printf("4.Query sent to Auth server\n");
        char IP[MAX];
        recvb=recvfrom(Authsock,(char*)IP,sizeof(IP),0,NULL,NULL);
        IP[recvb]='\0';

        sendb=sendto(sockfd,(const char*)IP,strlen(IP),0,(const struct sockaddr*)&cliaddr,size);
        printf("6.IP addr sent to ROOT\n");
        close(Authsock);
    }
    return;
}



int main(){
    int tldsock;
    struct sockaddr_in tldaddr,cliaddr;

    bzero(&tldaddr,sizeof(tldaddr));
    bzero(&cliaddr,sizeof(cliaddr));

    //create socket
    tldsock=socket(AF_INET,SOCK_DGRAM,0);
    //fill socket addr
    tldaddr.sin_family=AF_INET;
    tldaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    tldaddr.sin_port=htons(TLD);

    if((bind(tldsock,(const struct sockaddr*)&tldaddr,sizeof(tldaddr)))<0){
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    printf("Intermediate  Name DNS server running at PORT %d\n",TLD);

    chat(tldsock,cliaddr);
    close(tldsock);
}