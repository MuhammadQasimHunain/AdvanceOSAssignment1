//
//  RequestHandlers.h
//  PS
//
//  Created by Apple PC on 10/04/2017.
//  Copyright © 2017 ArhamSoft. All rights reserved.
//

#ifndef RequestHandlers_h
#define RequestHandlers_h

#include <time.h>


//HANDLE GET REQUEST Via SHARED MEMORY function //

static void handle_get_with_shared_memory (int connection_fd, const char* proxyBaseUrl, const char* page)
{
    char data_to_send[BYTES];
    int fd;
    long bytes_read;
    // The file will start with single slash when seprated from server host address
    if (*page == '/' && strchr (page + 1, '/') == NULL) {
        char file_name[1024];
        
        
        if ( strncmp(page, "/\0", 2)==0 ) {
            snprintf (file_name, sizeof ("index.html"), "index.html");
            //Index.html is by default
        }
        else {
            snprintf (file_name, sizeof (file_name), "%s", page);
            // removing the "/" on the first index
            memmove (file_name, file_name+1, strlen (file_name+1) + 1);
        }
        
        if ( (fd=open(file_name, O_RDONLY))!=-1 )    //FILE FOUND
        {
            write (connection_fd, ok_response, strlen (ok_response));
            while ( (bytes_read=read(fd, data_to_send, BYTES-1))>0 )
                write (connection_fd, data_to_send, bytes_read);
        }
        else {
            // return 404 if the file is not found or file is corrupt.
            char response[1024];
            
            //Creating response message
            snprintf (response, sizeof (response), not_found_response_template, page);
            key_t key;
            int   shmid;
            struct SharedMemory* segptr;
            // Creating unique key via File token request
            key = ftok("/Users/ArhamSoft/Desktop/FileToken.txt", connection_fd);
            //            key = ftok(FTOK_KEY, 'S');
            
            if((shmid = shmget(key, SegmentLength, IPC_CREAT|IPC_EXCL|0666)) == -1) {
                printf("Duplicating segment trying as a client\n");
                // Duplicating segment try as a client.
                if((shmid = shmget(key, SegmentLength, 0)) == -1) {
                    perror("Throw shmget exception");
                    exit(1);
                }
            }
            else {
                printf("Creating new shared memory segment\n");
            }
            //Connect shared memeory to current runnning thread.
            (segptr = (struct SharedMemory *)shmat(shmid, 0, 0));
            if( segptr == (struct SharedMemory*)-1) {
                perror("Throw shmat() attaching Exception");
                exit(1);
            }
            
            if (pthread_mutexattr_init(&segptr->MutexAttribute) != 0) {
                perror("Throw pthread_mutexattr_init() Exception");
            }
            if (pthread_mutexattr_setpshared(&segptr->MutexAttribute, PTHREAD_PROCESS_SHARED) != 0) {
                perror("Throw pthread_mutexattr_setpshared() Exception");
            }
            if (pthread_condattr_init(&segptr->ConditionAttribute) != 0) {
                perror("Throw pthread_condattr_init Exception ");
            }
            if (pthread_condattr_setpshared(&segptr->ConditionAttribute, PTHREAD_PROCESS_SHARED) != 0) {
                perror("Throw pthread_condattr_setpshared() Exception");
            }
            if (pthread_cond_init(&segptr->Condition, &segptr->ConditionAttribute) != 0) {
                perror("Throw pthread_cond_init() Exception");
            }
            if (pthread_mutex_init(&segptr->Mutex, &segptr->MutexAttribute) != 0) {
                perror("Throw pthread_mutexattr_init() Exception");
            }
            if (pthread_mutex_lock(&segptr->Mutex) != 0) {
                perror("Throw pthread_mutex_lock() Exception");
            }
            
            const char *hostname = proxyBaseUrl;
            struct hostent *hp;
            struct sockaddr_in addr;
            int on = 1;
            int sd; // Socket to connect with the descriptior
            
            if((hp = gethostbyname(hostname)) == NULL){
                herror("Throw gethostbyname Exception");
                exit(1);
            }
            printf("%s\n",hp->h_name );
            bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
            addr.sin_port = htons(ServerPortNo);
            addr.sin_family = AF_INET;
            
            sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
            
            if(sd == -1){
                perror("Throw setsockopt Exception");
                return;
            }
            if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
                perror("error on connect... \nSkipping");
                shutdown(sd, SHUT_RDWR);
                close(sd);
                return;
            }
            
            char buffer[BufferLength];
            char completeRequest[1024] = "LOCAL-GET /index.html HTTP/1.0";
            
            strcat(completeRequest, "\r\n&key=");
            char stringId[20];
            snprintf(stringId, 20, "%d", key);
            
            strcat(completeRequest, stringId);
            strcat(completeRequest, "\r\n\r\n");
            
            write(sd, completeRequest, strlen(completeRequest));
            bzero(buffer, BufferLength);
            shutdown(sd, SHUT_RDWR);
            close(sd);
            
            struct timeval TimeValue;
            struct timespec TimeSpec;
            gettimeofday(&TimeValue, NULL);
            TimeSpec.tv_sec = TimeValue.tv_sec + 2;
            TimeSpec.tv_nsec = 0;
            if (pthread_cond_timedwait(&segptr->Condition, &segptr->Mutex, &TimeSpec) != 0) {
                perror("Throw pthread_cond_timedwait() Exception");
            }
            send(connection_fd, segptr->data, strlen(segptr->data), 0); // read once the data is written
            
            pthread_mutex_unlock(&segptr->Mutex);
            struct shmid_ds buff;
            if (shmctl(shmid, IPC_STAT, &buff) == -1) {
                perror("shmctl() error with IPC_STAT");
            }
            if (shmctl(shmid, IPC_RMID, &buff) == -1)
            {
                // remove the shared memory segment
                perror("Throw shmctl() Exception");
            }
            if (shmdt(segptr) == -1) {
                perror("Throw shmdt() Exception");
            }
            else {
                printf("Discard shared Memory\n");
            }
            
            
            //**********
        }
        
    }
    
}
//****************** HANDLE GET REQUEST function ******************//

static void handle_get_with_sockets (int connection_fd, const char* proxyBaseUrl, const char* page)
{
    char data_to_send[BYTES];
    int fd;
    long bytes_read;
    /* Make sure the requested page begins with a slash and does not
     contain any additional slashes -- we don't support any
     subdirectories.  */
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
        //        strcpy(path, ROOT);
        //        strcpy(&path[strlen(ROOT)], page);
        
        if ( (fd=open(file_name, O_RDONLY))!=-1 )    //FILE FOUND
        {
            write (connection_fd, ok_response, strlen (ok_response));
            while ( (bytes_read=read(fd, data_to_send, BYTES-1))>0 )
                write (connection_fd, data_to_send, bytes_read);
        }
        else {
            //Return 404 if the file is corupted or not found.
            char response[1024];
            
            /* Generate the response message.  */
            snprintf (response, sizeof (response), not_found_response_template, page);
            
            const char *hostname = proxyBaseUrl;
            struct hostent *hp;
            struct sockaddr_in addr;
            int on = 1;
            int sd; // Socket descriptor that would be used to communicate with the server
            
            if((hp = gethostbyname(hostname)) == NULL){
                herror("gethostbyname");
                exit(1);
            }
            printf("%s\n",hp->h_name );
            bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
            addr.sin_port = htons(ServerPortNo);
            addr.sin_family = AF_INET;
            
            sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
            
            if(sd == -1){
                perror("setsockopt");
                return;
            }
            if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
                perror("error on connect... \nSkipping");
                shutdown(sd, SHUT_RDWR);
                close(sd);
                return;
            }
            
            char buffer[BufferLength];
            char completeRequest[1024] = "GET /index.html HTTP/1.0\r\n\r\n";
            
            write(sd, completeRequest, strlen(completeRequest));
            bzero(buffer, BufferLength);
            
            long totalBytesRead = 0;
            long bytesRead = read(sd, buffer, BufferLength - 1);
            while(bytesRead != 0 && bytesRead != -1){
                write (connection_fd, buffer, BufferLength - 1); // send data back to client
                totalBytesRead += bytesRead;
                //                    fprintf(stderr, "%s", buffer);
                bzero(buffer, BufferLength);
                bytesRead = read(sd, buffer, BufferLength - 1);
            }
            
            printf("Total bytes received : %ld\n", totalBytesRead);
            
            shutdown(sd, SHUT_RDWR);
            close(sd);
            
            
            //**********
        }
        
    }
    
}

//WORKER THREAD handle_request function
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
            char proxyAddress[sizeof (buffer)];
            
            /* Some data was read successfully.  NUL-terminate the buffer so
             we can use string operations on it.  */
            buffer[bytes_read] = '\0';
            /* The first line the client sends is the HTTP request, which is
             composed of a method, the requested page, and the protocol
             version.  */
            sscanf (buffer, "%s %s %s %s", method, url, proxyAddress, protocol);
            /* The client may send various header information following the
             request.  For this HTTP implementation, we don't care about it.
             However, we need to read any data the client tries to send.  Keep
             on reading data until we get to the end of the header, which is
             delimited by a blank line.  HTTP specifies CR/LF as the line
             delimiter.  */
            while (strstr (buffer, "\r\n\r\n") == NULL)
                bytes_read = read (new_sd, buffer, sizeof (buffer));
            /* Make sure the last read didn't fail.  If it did, there's a
             problem with the connection, so give up.  */
            if (bytes_read == -1) {
                close (new_sd);
                return NULL;
            }
            /* Check the protocol field.  We understand HTTP versions 1.0 and
             1.1.  */
            if ((strcmp (protocol, "HTTP/1.0") && strcmp (protocol, "HTTP/1.1")) &&
                (strcmp (proxyAddress, "HTTP/1.0") && strcmp (proxyAddress, "HTTP/1.1"))) {
                /* We don't understand this protocol.  Report a bad response.  */
                char response[1024];
                
                /* Generate the response message.  */
                snprintf (response, sizeof (response), bad_request_response, url);
                /* Send it to the client.  */
                //            write (connection_fd, response, strlen (response));
                write (new_sd, response,
                       sizeof(response));
            }
            else if (strcmp (method, "GET")) {
                /* This server only implements the GET method.  The client
                 specified some other method, so report the failure.  */
                char response[1024];
                
                snprintf (response, sizeof (response),
                          bad_method_response_template, method);
                write (new_sd, response, strlen (response));
            }
            else {
                /* A valid request.  Process it.  */
                char baseURL[256] = "127.0.0.1";
                char filepath[256] = "/index.html";
                if (SHOULD_USE_SHARED_MEMORY) {
                    if (strcmp (protocol, "HTTP/1.0") && strcmp (protocol, "HTTP/1.1")) {
                        handle_get_with_shared_memory(new_sd, baseURL, url);
                    }
                    else {
                        sscanf (proxyAddress, "%254[^'/']%s", baseURL, filepath);
                        handle_get_with_shared_memory(new_sd, baseURL, filepath);
                    }
                }
                else {
                    if (strcmp (protocol, "HTTP/1.0") && strcmp (protocol, "HTTP/1.1")) {
                        handle_get_with_sockets(new_sd, baseURL, url);
                    }
                    else {
                        sscanf (proxyAddress, "%254[^'/']%s", baseURL, filepath);
                        handle_get_with_sockets(new_sd, baseURL, filepath);
                    }
                }
            }
        }
        else if (bytes_read == 0)
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
                perror("Throw pthread_mutex_init() Exception");
            }
            if (pthread_cond_init(&Condition[threadDetails->ThreadNo], NULL) != 0) {
                perror("Throw pthread_cond_init() Exception");
            }
            if (pthread_mutex_lock(&Mutex[threadDetails->ThreadNo]) != 0) {
                perror("Throw pthread_mutex_lock() Exception");
            }
        }
        
        
    }
    
    printf("Terminating worker thread no : %d\n\n", threadDetails->ThreadNo);
    return NULL;
}


#endif /* RequestHandlers_h */
