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
                                                                                                                                                                                           
#define SIZE 300                                                                                                                                                                           
#define server_port 7333                                                                                                                                                                  
#define client_port 8333                                                                                                                                                                   
#define x 200                                                                                                                                                                              
#define N 5                                                                                                                                                                                
                                                                                                                                                                                           
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
                                                                                                                                                                                           

                                                                                                                                                                                           
student stud[SIZE];                                                                                                                                                                        
int main()                                                                                                                                                                                 
{                                                                                                                                                                                          
        //proxy as a server part                                                                                                                                                           
        struct sockaddr_in serveraddr;                                                                                                                                                     
        int proxyserver_sockfd = socket(AF_INET,SOCK_STREAM,0);                                                                                                                            
        if(proxyserver_sockfd < 0)                                                                                                                                                         
        {                                                                                                                                                                                  
                printf("error in socket creation\n");                                                                                                                                      
                return 0;                                                                                                                                                                  
        }                                                                                                                                                                                  
        printf("socket created\n");                                                                                                                                                        
                                                                                                                                                                                           
        serveraddr.sin_family = AF_INET;                                                                                                                                                   
        serveraddr.sin_port = ntohs(client_port);                                                                                                                                          
        serveraddr.sin_addr.s_addr = INADDR_ANY;                                                                                                                                           
                                                                                                                                                                                           
        int ret = bind(proxyserver_sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));                                                                                               
        if(ret < 0)                                                                                                                                                                        
        {                                                                                                                                                                                  
                printf("error in binding\n");                                                                                                                                              
                return 0;                                                                                                                                                                  
        }                                                                                                                                                                                  
                                                                                                                                                                                           
        printf("binding succesful\n");                                                                                                                                                     

       if(listen(proxyserver_sockfd,N)<0)                                                                                                                                                  
       {                                                                                                                                                                                   
               printf("ERROR IN LISTENING \n");                                                                                                                                            
               return 0;                                                                                                                                                                   
       }                                                                                                                                                                                   
                                                                                                                                                                                           
       printf("LISTENING ON PORT %d\n",client_port);                                                                                                                                       
        struct sockaddr_in dummy;                                                                                                                                                          
       int proxylen;                                                                                                                                                                       
       int proxy_as_server_sockfd = accept(proxyserver_sockfd,(struct sockaddr*)&dummy,&proxylen);                                                                                         
       int k = 0;                                                                                                                                                                          
       //proxy as a client to the main server                                                                                                                                              
       struct sockaddr_in cli;                                                                                                                                                             
       int proxy_as_client_sockfd = socket(AF_INET,SOCK_STREAM,0);                                                                                                                         

       cli.sin_family = AF_INET;                                                                                                                                                           
       cli.sin_port = ntohs(server_port);                                                                                                                                                  
       cli.sin_addr.s_addr = INADDR_ANY;                                                                                                                                                   

       if(connect(proxy_as_client_sockfd,(struct sockaddr*)&cli,sizeof(cli))<0)                                                                                                            
       {                                                                                                                                                                                   
               printf("ERROR IN CONNECTING TO THE CLIENT\n");                                                                                                                              
               return -1;                                                                                                                                                                  
       }                                                                                                                                                                                   
       printf("SUCCESSFULLY CONNECTED TO THE CLIENT\n");                                                                                                                                   

       int check = 0,temp=0;                                                                                                                                                               
       while(1)                                                                                                                                                                            
       {                                                                                                                                                                                   
               request req;                                                                                                                                                                
               int receive = recv(proxy_as_server_sockfd,&req,sizeof(req),0);                                                                                                              
               if(check == 0)                                                                                                                                                              
               {                                                                                                                                                                           
                       printf("%s\t%s\n",req.method,req.version);                                                                                                                          
                       printf("ACCEPT -  %s\n",req.accept);
                       printf("CONNECTION - %s\n",req.connection);                                                                                                                         
                       printf("USERAGENT - %s\n",req.useragent);                                                                                                                           
                       check = 1;                                                                                                                                                          
               }                                                                                                                                                                           
                                                                                                                                                                                           

               int flag = 0;                                                                                                                                                               
               time_t t = time(NULL);                                                                                                                                                      
               struct tm *d = localtime(&t);                                                                                                                                               
               response resp = {200,"OK","HTTP/1.1"};                                                                                                                                      
               strcpy(resp.datetime,asctime(d));                                                                                                                                           
               strcpy(resp.connection,"KEEP ALIVE");                                                                                                                                       
               strcpy(resp.content,"TEXT/Html");                                                                                                                                           
                                                                                                                                                                                           
               if(req.stu.roll==-999)                                                                                                                                                      
                       break;                                                                                                                                                              
                                                                                                                                                                                           

               for(int  i = 0;i<k;i++)                                                                                                                                                     
               {                                                                                                                                                                           
                       if( req.stu.roll == stud[i].roll&& req.stu.sem==stud[i].sem)                                                                                                        
                       {                                                                                                                                                                   
                               flag = 1;                                                                                                                                                   
                               resp.stu = stud[i];                                                                                                                                         
                               send(proxy_as_server_sockfd,&resp,sizeof(resp),0);                                                                                                          
                               break;                                                                                                                                                      
                       }                                                                                                                                                                   
               }                                                                                                                                                                           

               if(flag == 0)                                                                                                                                                               
               {                                                                                                                                                                           
                       printf("PROXY SERVER CONNECTED TO MAIN SERVER TO FETCH INFORMATION\n");                                                                                             

                       send(proxy_as_client_sockfd,&req,sizeof(req),0);                                                                                                                    
                       recv(proxy_as_client_sockfd,&resp,sizeof(resp),0);                                                                                                                  
                       if(temp == 0)                                                                                                                                                       
                       {                                                                                                                                                                   
                               printf("%d\t%s\t%s\n",resp.status,resp.statusmsg,resp.version);                                                                                             
                               printf("CONNECTION - %s\n",resp.connection);                                                                                                                
                               printf("CONTENT TYPE - %s\n",resp.content);                                                                                                                 
                               printf("DATE - %s\n",resp.datetime);                                                                                                                        
                               temp=1;                                                                                                                                                     
                       }                                                                                                                                                                   
                               stud[k] = resp.stu;                                                                                                                                         
                               k++;                                                                                                                                                        
                               send(proxy_as_server_sockfd,&resp,sizeof(resp),0);                                                                                                          
                }                                                                                                                                                                          
       }                                                                                                                                                                                   
       close(proxy_as_client_sockfd);                                                                                                                                                      
       close(proxyserver_sockfd);                                                                                                                                                          
                                                                                                                                                                                           
       return 0;                                                                                                                                                                           
}             

