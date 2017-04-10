//
//  c.h
//  Client
//
//  Created by Apple PC on 10/04/2017.
//  Copyright © 2017 ArhamSoft. All rights reserved.
//

#ifdef __APPLE__
#  define error printf
#endif

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "LinkedQueueDatastructures.h"
#include "ResponseHandlers.h"


struct Details
{
    int socket_id;
    int requests_per_thread;
    int thread_number;
};



void parse_command_line_arguments(int argc, char* argv[], int*, int* );
void create_required_threads();
void *make_request(void *param);
int socket_connect(char *host, in_port_t port);


// Main

int main(int argc, char* argv[] )
{
    system("clear");
    
    int totalThreads = 0;
    int requestsPerThread = 0;
    //
    parse_command_line_arguments(argc, argv, &totalThreads, &requestsPerThread);
    printf("No of threads : %d\n", totalThreads);
    printf("Request per thread : %d\n", requestsPerThread);
    
    // fd = socket_connect(argv[1], atoi(argv[2]));
    char hostname[1024] = ProxyIpAddress;
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1;
    int sd; // Socket descriptor that would be used to communicate with the server
    
    if((hp = gethostbyname(hostname)) == NULL){
         // Host name
        herror("Host name Error at gethostname");
        exit(1);
    }
    printf("%s\n",hp->h_name );
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(ProxyPortNo);
    addr.sin_family = AF_INET;
    
    pthread_t *allThreads = malloc(sizeof(pthread_t)*totalThreads);
    for (int i=0; i<totalThreads; i++) {
        sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        
        if(sd == -1){
            perror("setsockopt : Setting Socke operation");
            continue;
        }
        if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
            perror("error on connect.. - Connection error -. \nSkipping");
            continue;
        }
        
        struct Details *myDetails = malloc (sizeof(struct Details));
        myDetails->requests_per_thread = requestsPerThread;
        myDetails->socket_id = sd;
        myDetails->thread_number = i;
        pthread_create(&allThreads[i], NULL, make_request, (void*) myDetails);
        
    }
    
    for (int i=0; i<totalThreads; i++) {
        pthread_join(allThreads[i], NULL);
    }
    
    printf("Main thread is exiting");
    
    return 0;
}


void parse_command_line_arguments(int argc, char* argv[], int *totalThreads, int *requestsPerThread )
{
    
    //Parsing the command line arguments
    if( argc == 3 ) {
        *totalThreads = atoi(argv[1]);
        *requestsPerThread = atoi(argv[2]);
    }
    else {
        fprintf(stderr, "Use : %s <Total-number-threads> <Number-Requests-per-thread>\n", argv[0]);
        exit(1);
    }
}


//HELPER FUNCTIONS

void create_required_threads()
{
    int s_id;
    struct sockaddr_in serv_addr;
    s_id = socket (PF_INET,SOCK_STREAM,0);
    if(s_id == -1)
    {
        perror("Cant assign sockets");
        // return 0;
    }
    
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port = htons (ProxyPortNo);
    serv_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
    
    // Connect Using the accept function
    int connect_id=connect(s_id,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
    if(connect_id == -1)
    {
        error("Client. Could not Connect ");
        // return 0;
    }
    
}

//Making a request
void *make_request(void *param)
{
    struct Details *details = (struct Details*)param;
    if (details == NULL) {
        return NULL;
    }
    int sd = details->socket_id;
    char buffer[BufferLength];
    
    char completeRequest[1024] = "GET ";
    strcat(completeRequest, ProxyIpAddress);
    strcat(completeRequest, " 127.0.0.1/index.html");
    strcat(completeRequest, " HTTP/1.0");
    
    
    strcat(completeRequest, "\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11.6; rv:52.0) Gecko/20100101 Firefox/52.0");
    strcat(completeRequest, "\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    strcat(completeRequest, "\r\nConnection: keep-alive");
    strcat(completeRequest, "\r\n\r\n");
    for (int i=0 ; i<details->requests_per_thread ; i++) {
        
        write(sd, completeRequest, strlen(completeRequest));
        bzero(buffer, BufferLength);
        
        long totalBytesRead = 0;
        long bytesRead = read(sd, buffer, BufferLength - 1);
        while(bytesRead != 0 && bytesRead != -1){
            totalBytesRead += bytesRead;
            fprintf(stderr, "%s", buffer);
            bzero(buffer, BufferLength);
            bytesRead = read(sd, buffer, BufferLength - 1);
        }
        
        printf("Thread : %d   Request : %d    Total bytes received : %ld\n", details->thread_number, i, totalBytesRead);
        
    }
    shutdown(sd, SHUT_RDWR);
    close(sd);
    
    return NULL;
}

