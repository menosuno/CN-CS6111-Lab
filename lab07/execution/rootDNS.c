#include "headers.h"

#define ROOT 3171
#define MAX 100


void DNS_lookup(char* host,char* IP){
    FILE* data=fopen("root_DNS.txt","r");

    char linebuff[MAX];
    char temp[MAX];
    while(fgets(linebuff,MAX,data)!=0){
        strcpy(temp,linebuff);
        char* tok=strtok(temp," ");
      //  printf("%s-%s-%s\n",linebuff,tok,host);
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

    //recv:localDNS
    //send:TLD
    while(1){
        char query[MAX];
        int sendb,recvb;

        int size=sizeof(cliaddr);
        recvb=recvfrom(sockfd,(char*)query,sizeof(query),0,(struct sockaddr*)&cliaddr,&size);

        //parse to get domain name
        char domain[MAX];
        char temp[MAX];
        strcpy(temp,query);
        char* tok=strtok(temp,".");
        tok=strtok(NULL,".");
        tok=strtok(NULL,".");
        strcpy(domain,tok);
        printf("\nfrom local server,Domain name:%s\n",domain);

        //find TLD server to query
        char resp[MAX];
        DNS_lookup(domain,resp);

        char TLDip[MAX];
        int TLDport;

        //parse resp to get TLDip and TLDport
        tok=strtok(resp,",");
        strcpy(TLDip,tok);
        tok=strtok(NULL,"\0");
        TLDport=strtol(tok,NULL,10);
        printf("TLD IP:%s Port:%d\n",TLDip,TLDport);

        //connect to the TLD server authoritative of the hostname
        int TLDsock;
        struct sockaddr_in TLDaddr;
        bzero(&resp,sizeof(resp));
        TLDsock= socket(AF_INET, SOCK_DGRAM, 0);
        bzero(&TLDaddr, sizeof(TLDaddr));
        TLDaddr.sin_family = AF_INET;
        TLDaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
        TLDaddr.sin_port = htons(TLDport);//TLDport from lookup

        int rsize=sizeof(TLDaddr);
        sendb=sendto(TLDsock,(const char*)query,strlen(query),0,(const struct sockaddr*)&TLDaddr,rsize);
        printf("3.Query sent to Intermediate Name server\n");
        char IP[MAX];
        recvb=recvfrom(TLDsock,(char*)IP,sizeof(IP),0,NULL,NULL);
        IP[recvb]='\0';
        
        //recvied IP sent to localDNS
        sendb=sendto(sockfd,(const char*)IP,strlen(IP),0,(const struct sockaddr*)&cliaddr,size);
        printf("7.IP addr sent to local server\n");
    }
    return;
}



int main(){
    int rootsock;
    struct sockaddr_in rootaddr,cliaddr;

    bzero(&rootaddr,sizeof(rootaddr));
    bzero(&cliaddr,sizeof(cliaddr));

    //create socket
    rootsock=socket(AF_INET,SOCK_DGRAM,0);
    //fill socket addr
    rootaddr.sin_family=AF_INET;
    rootaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    rootaddr.sin_port=htons(ROOT);

    if((bind(rootsock,(const struct sockaddr*)&rootaddr,sizeof(rootaddr)))<0){
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    printf("ROOT DNS server running at PORT %d\n",ROOT);

    chat(rootsock,cliaddr);
    close(rootsock);
}