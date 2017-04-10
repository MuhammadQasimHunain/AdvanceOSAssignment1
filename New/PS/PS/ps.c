

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
#include <fcntl.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <sys/ipc.h>
#include <sys/shm.h>
#include "HeaderResponse.h"
#include "Helper.h"
#include "LinkedDataStructures.h"
#include "RequestHandlers.h"
#include "queue.h"


int createThreadPool(struct ThreadPoolManager *manager);

// Main
int main(int argc, char* argv[] )
{
    waitingRequestsQueue = newQueue();
    parse_command_line_arguments(argc, argv);
    int threadCount = 0;
    struct ThreadPoolManager tm;
    ROOT = getenv("PWD");
    int s_id;
    struct sockaddr_in my_addr, remote_addr;
    s_id = socket(PF_INET, SOCK_STREAM, 0);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(ProxyPortNo);
    my_addr.sin_addr.s_addr = inet_addr(ProxyIpAddress);
    if(bind(s_id, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1) {
        error("Server failed to bind\n");
        exit(2);
    }
    listen(s_id,2);
    unsigned int size = sizeof (struct sockaddr_in);
    
    int new_sd=0;
    createThreadPool(&tm);
    
    while(1)
    {
        // accepting the client's request and assigning new socket
        new_sd = accept (s_id, (struct sockaddr *) &remote_addr, &size);
        if(new_sd == -1) {
            error("Server. Could not accept the request");
            continue;
        }
        if ((threadCount = getNextEmptyThreadNumber(&tm, threadCount, true)) == -1) {
            perror("Unable to accept request");
            queue_offer(waitingRequestsQueue, &new_sd);
            continue;
        }
        tm.ThreadArray[threadCount].SocketId = new_sd;
        if (pthread_cond_signal(&Condition[threadCount]) != 0) {
            perror("pthread_cond_signal() error");
        }
        threadCount++;
        if (threadCount >= MaxNumberOfThred) {
            threadCount = 0;
        }
    }
    
    return 0;
}

//Creating Thread Pool


int createThreadPool(struct ThreadPoolManager *manager)
{
    for(int i=0 ; i < MaxNumberOfThred ; i++)
    {
        if (pthread_mutex_init(&Mutex[i], NULL) != 0) {
            perror("Throw pthread_mutex_init() Exception");
        }
        if (pthread_cond_init(&Condition[i], NULL) != 0) {
            perror("Throw pthread_cond_init() Exception");
        }
        if (pthread_mutex_lock(&Mutex[i]) != 0) {
            perror("Throw pthread_mutex_lock() Exception");
        }
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Required!!!
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    struct Thread *threadPool = manager->ThreadArray;
    threadPool[0].ThreadID = 0;
    
    for(int i=0 ; i < MaxNumberOfThred ; i++) {
        threadPool[i].ThreadNo = i;
        if (pthread_create(&threadPool[i].ThreadID, &attr, handle_request, (void*) &threadPool[i]) != 0) {
            printf("Cannot create %d th thread\n", i+1);
        }
        else {
            printf("Created %d th thread\n", i+1);
        }
    }
    
    return 0;
    
}


