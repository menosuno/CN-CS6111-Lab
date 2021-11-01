#include "headers.h"

#define DNS_PORT 3071
#define ROOT     3171
#define MAX      100
#define cache "local_cache.txt"


void DNS_lookup(char* host,char* IP){
    FILE* data=fopen("local_cache.txt","r");

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
    fclose(data);
    return;
}


void fill_cache(char* DNS_record){
    FILE* data=fopen("local_cache.txt","a");

    if(!data){
        perror("file open error\n");
        exit(-1);
    }

    printf("%s\n",DNS_record);
    fseek(data,0,SEEK_SET);
    //fwrite(DNS_record,1,strlen(DNS_record)+1,data);
    fprintf(data,"\n%s",DNS_record);
    printf("Cache updated\n");
    fclose(data);
    return;
}
void chat(int sockfd,struct sockaddr_in cliaddr){

    int size=sizeof(cliaddr);

    while(1){
        int sendb,recvb;
        char host[MAX];
        recvb=recvfrom(sockfd,(char*)host,sizeof(host),0,(struct sockaddr*)&cliaddr,&size);
        host[recvb]='\0';
        printf("Host:%s\n",host);

        char IP[MAX];
        DNS_lookup(host,IP);
        //query root DNS server
        if(strncmp(IP,"NF",strlen(IP))==0){
            printf("\nDoing Recursive search...\n");
            int rsize=0;
            char resp[MAX];

            //rootDNS
            int rootsock;
            struct sockaddr_in rootaddr;
            char TLDip[MAX];
            int TLDport;
            bzero(&resp,sizeof(resp));

            rootsock= socket(AF_INET, SOCK_DGRAM, 0);
            bzero(&rootaddr, sizeof(rootaddr));
            rootaddr.sin_family = AF_INET;
            rootaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
            rootaddr.sin_port = htons(ROOT);

            rsize=sizeof(rootaddr);
            sendb=sendto(rootsock,(const char*)host,strlen(host),0,(const struct sockaddr*)&rootaddr,rsize);
            printf("2.Query sent to ROOT server\n");
            recvb=recvfrom(rootsock,(char*)resp,sizeof(resp),0,NULL,NULL);
            resp[recvb]='\0';
            printf("ROOT Resp:%s\n",resp);

            //fill IP
            strcpy(IP,resp);

            //fill cache
            char DNS_record[MAX];
            sprintf(DNS_record,"%s %s",host,IP);
            fill_cache(DNS_record);
        }
    
        sendto(sockfd,(const char*)IP,strlen(IP),0,(const struct sockaddr*)&cliaddr,size);
        printf("8.IP addr sent to client\n");
    }
    return;
}



int main(){

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
      
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    bzero(&servaddr,sizeof(servaddr));
    bzero(&cliaddr,sizeof(cliaddr));
      
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DNS_PORT);
      
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket binded with PORT %d\nListening..\n",DNS_PORT);

    chat(sockfd,cliaddr);
    close(sockfd);
}