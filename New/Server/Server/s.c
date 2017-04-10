// For creating the response, some help and code has been taken
// from the following link : advancedlinuxprogramming.com/listings/chapter-11/server.c


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
#include<fcntl.h>
#include<signal.h>
#include <netinet/tcp.h>
#include<netdb.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/ipc.h>
#include <sys/shm.h>
#include "LinkedDataStructures.h"
#include "HeaderResponse.h"
#include "queue.h"



int createThreadPool(struct ThreadPoolManager *manager);
int getNextEmptyThreadNumber(struct ThreadPoolManager* poolManager, int nextThreadNumber, bool force) {
    
    if (poolManager->ThreadArray[nextThreadNumber].IsFree == true) {
        return nextThreadNumber; // the next thread is free
    }
    for (int i=(nextThreadNumber+1)%MaxNumberOfThred ; i != nextThreadNumber ; i = (i+1)%MaxNumberOfThred) {
        if (poolManager->ThreadArray[i].IsFree == true) {
            return i;
        }
    }
    sleep(1);
    if (force) {
        return getNextEmptyThreadNumber(poolManager, 1, false);
    }
    else {
        return -1;
    }
    
}


// MARK: - Main Function
//****************** MAIN FUNCTION ******************//

int main()
{
    waitingRequestsQueue = newQueue();
    int threadCount = 0;
    struct ThreadPoolManager tm;
    ROOT = getenv("PWD");
    int s_id;
    struct sockaddr_in my_addr, remote_addr;
    s_id = socket(PF_INET, SOCK_STREAM, 0);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(ServerPortNo);	//assigning the port
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//assigning the self ip address
    
    if(bind(s_id, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1) {
        error("Server failed to bind\n");
        exit(2);
    }
    if (listen(s_id,2) == -1) {
        perror("Server. Could not listen");
    }
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

// MARK: - Handle Local Get
//****************** HANDLE LOCAL GET REQUEST function ******************//

static void handle_local_get (int connection_fd, const char* page, const char* key)
{
    char data_to_send[BYTES];
    int fd;
    long bytes_read;
    // when seprating file name from response the file should contain / before the file name.
    if (*page == '/' && strchr (page + 1, '/') == NULL) {
        char file_name[1024];
        
        
        if ( strncmp(page, "/\0", 2)==0 ) {
            snprintf (file_name, sizeof ("index.html"), "index.html");
            //Because if no file is specified, index.html will be opened by default
        }
        else {
            snprintf (file_name, sizeof (file_name), "%s", page);
            // removing the "/" on the first index
            memmove (file_name, file_name+1, strlen (file_name+1) + 1);
        }
        
        if ( (fd=open(file_name, O_RDONLY))!=-1 )    //FILE FOUND
        {
            write (connection_fd, ok_response, strlen (ok_response));
            while ( (bytes_read=read(fd, data_to_send, BYTES-1))>0 ) {
                write (connection_fd, data_to_send, bytes_read);
            }
        }
        else {
            //send 404 if the file is corupted or not found.
            char response[1024];
            
            /* Generate the response message.  */
            snprintf (response, sizeof (response), not_found_response_template, page);
            // Sendng to client
            printf("Total Sent Byte : %ld\n", strlen(response));
            
            //            key_t key;
            int   shmid;
            struct SharedMemory* segptr;
            
            char charId[30];
            sprintf(charId, "%d", connection_fd+1000);
            //segment dublicating try again.
            if((shmid = shmget(atoi(key), SegmentLength, 0)) == -1) {
                perror("shmget");
                if (pthread_cond_signal(&segptr->Condition) != 0) {
                    perror("pthread_cond_signal() error");
                }
                return;
            }
            
            //attaching shared memory with current thread.
            (segptr = (struct SharedMemory *)shmat(shmid, 0, 0));
            if( segptr == (struct SharedMemory*)-1) {
                perror("shmat");
                exit(1);
            }
            
            strcpy(segptr->data, response);
            //            sleep(1);
            if (pthread_cond_signal(&segptr->Condition) != 0) {
                perror("pthread_cond_signal() error");
            }
            
            shmdt(segptr);
            

        }
        
    }
    
}


// Handling get request
static void handle_get (int connection_fd, const char* page)
{
    printf("\nServer. Transmitting data via sockets.");
    char data_to_send[BYTES];
    int fd;
    long bytes_read;
    // When seprating file name from complete url it will give / in start of file name. remving that first.
    if (*page == '/' && strchr (page + 1, '/') == NULL) {
        char file_name[1024];
        
        
        if ( strncmp(page, "/\0", 2)==0 ) {
            snprintf (file_name, sizeof ("index.html"), "index.html");
            //Because if no file is specified, index.html will be opened by default
        }
        else {
            snprintf (file_name, sizeof (file_name), "%s", page);
            // removing the "/" on the first index
            memmove (file_name, file_name+1, strlen (file_name+1) + 1);
        }
        
        if ( (fd=open(file_name, O_RDONLY))!=-1 )    //FILE FOUND
        {
            write (connection_fd, ok_response, strlen (ok_response));
            while ( (bytes_read=read(fd, data_to_send, BYTES-1))>0 ) {
                write (connection_fd, data_to_send, bytes_read);
            }
        }
        else {
            // Return response if the page cant open or is corupted.
            char response[1024];
            
            // Creaet response
            snprintf (response, sizeof (response), not_found_response_template, page);
            // Sending to client.
            write (connection_fd, response, strlen (response));
            printf("Total Response sent : %ld\n", strlen(response));
            
        }
        // Open Module
    }
    
}

//  handle request function run on WORKER THREAD

void *handle_request(void *param)
{
    struct Thread *threadDetails = (struct Thread*)param;
    if(threadDetails == NULL) {
        printf("\nTerminating worker thread \n");
        return NULL;
    }
    
    while (true) {
        threadDetails->IsFree = true;
        if (pthread_cond_wait(&Condition[threadDetails->ThreadNo], &Mutex[threadDetails->ThreadNo]) != 0) {
            perror("pthread_cond_timedwait() error");
        }
    ReloadPendingRequest:
        threadDetails->IsFree = false;
        
        int new_sd=threadDetails->SocketId;
        char welcome[1024] = "***** Worker thread number : ";
        printf("%s %d  *****\n", welcome, threadDetails->ThreadNo);
        
        
        ///*******************
        
        char buffer[256];
        ssize_t bytes_read;
        
        /* Read some data from the client.  */
        bytes_read = read (new_sd, buffer, sizeof (buffer) - 1);
        if (bytes_read > 0) {
            char method[sizeof (buffer)];
            char url[sizeof (buffer)];
            char protocol[sizeof (buffer)];
            
            buffer[bytes_read] = '\0';
            // scrting with patteren of method then url and then protocol.
            sscanf (buffer, "%s %s %s", method, url, protocol);
            // Read till the header's end and try again if it doesnt match.
            while (strstr (buffer, "\r\n\r\n") == NULL)
                bytes_read = read (new_sd, buffer, sizeof (buffer));
            // Last read files should not return errror.
            if (bytes_read == -1) {
                close (new_sd);
                return NULL;
            }
            /* Check the protocol field.  We understand HTTP versions 1.0 and
             1.1.  */
            if (strcmp (protocol, "HTTP/1.0") && strcmp (protocol, "HTTP/1.1")) {
                // un-Understand able protocol should report to bad response
                write (new_sd, bad_request_response,sizeof (bad_request_response));
            }
            else if (strcmp (method, "GET") && strcmp(method, "LOCAL-GET")) {
                char response[1024];
                
                snprintf (response, sizeof (response),bad_method_response_template, method);
                write (new_sd, response, strlen (response));
            }
            else {
                /* A valid request.  Process it.  */
                char temp[256] = "127.0.0.1";
                char key[256] ;
                if (strcmp(method, "LOCAL-GET") == 0) {
                    // if its a request from a local proxy server, we ll use shared memory to pass data between the two processses.
                    // extracting key from the recceived request
                    sscanf (buffer, "%254[^'=']=%s", temp, key);
                    handle_local_get(new_sd, url, key);
                }
                else {
                    handle_get (new_sd, url);
                }
            }
        }
        else if (bytes_read == 0)
        /* The client closed the connection before sending any data.
         Nothing to do.  */
            ;
        else
        /* The call to read failed.  */
            perror ("read");
        
        // *******************
        shutdown(new_sd, SHUT_RDWR);
        close(new_sd);
        
        if (!queue_isEmpty(waitingRequestsQueue)) {
            int *socketID = (int*) queue_poll(waitingRequestsQueue);
            threadDetails->SocketId = *socketID;
            goto ReloadPendingRequest;
        }
        else {
            if (pthread_mutex_init(&Mutex[threadDetails->ThreadNo], NULL) != 0) {
                perror("pthread_mutex_init() error");
            }
            if (pthread_cond_init(&Condition[threadDetails->ThreadNo], NULL) != 0) {
                perror("pthread_cond_init() error");
            }
            if (pthread_mutex_lock(&Mutex[threadDetails->ThreadNo]) != 0) {
                perror("pthread_mutex_lock() error");
            }
        }
        
    }
    
    
    printf("Terminating worker thread no : %d\n\n", threadDetails->ThreadNo);
    return NULL;
}

//****************** THREAD MANAGEMENT ******************//

int createThreadPool(struct ThreadPoolManager *manager)
{
    for(int i=0 ; i < MaxNumberOfThred ; i++)
    {
        if (pthread_mutex_init(&Mutex[i], NULL) != 0) {
            perror("pthread_mutex_init() error");
        }
        if (pthread_cond_init(&Condition[i], NULL) != 0) {
            perror("pthread_cond_init() error");
        }
        if (pthread_mutex_lock(&Mutex[i]) != 0) {
            perror("pthread_mutex_lock() error");
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


