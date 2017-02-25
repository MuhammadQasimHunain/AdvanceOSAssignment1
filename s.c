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

#define QUIT 							"QUIT"
#define NEW_CONNECTION			 		"NEW-CONNECTION"
#define FILE_SUCCESSFULLY_RECEIVED		"FILE-SUCCESSFULLY-RECEIVED"


pthread_cond_t cond[1024];
pthread_mutex_t mutex[1024];


struct Details
{
	char msg[1024];
	int recv_id;
	int send_id;
	int thread_number;
};

struct Thread
{
	pthread_t threadId;
	int threadNumber;
	bool isFree;
	int socketId;
};

struct ThreadPool
{
	struct Thread threadArr[1024];
	int totalThreadCount; 
};

void *handle_request(void *param);
void send_file(char *fileName, int new_sd);
int createThreadPool(struct Thread *pool);

int myReceive(int socket, char *arr, int length, int flag);
int mySend(int socket, char *arr, int length, int flag);



//****************** MAIN FUNCTION ******************//

int main()
{
	pthread_t thread1, thread2;
	int rv1, rv2;
	pthread_t threads[1024];
	int threadCount = 0;
	char message1[1024] = "Thread1";
	char message2[1024] = "Thread2";
	struct Thread threadPool[1024];

	int s_id;
	char msg[1024];
	int msg_length = 1024;
	struct sockaddr_in my_addr, remote_addr;
	s_id = socket(PF_INET, SOCK_STREAM, 0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5555);	//assigning the port
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//assigning the self ip address

	if(bind(s_id, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1)
	{
		error("Server failed to bind\n");
		perror("Error");
	}
	listen(s_id,2); 
	unsigned int size = sizeof (struct sockaddr_in);

	int new_sd=0;
	int recv_id=0;
	int send_id=0;
	char data[1024];

	createThreadPool(threadPool);

		// pthread_create(&threads[0], NULL, handle_request, (void*) NULL);
		// {
		// 	printf("Cannot create %d th thread\n", 1);
		// }
		// else 
		// {
		// 	printf("Created %d th thread\n", 1);
		// }

	while(1)
	{
		// accepting the client's request and assigning new socket
		new_sd = accept (s_id, (struct sockaddr *) &remote_addr, &size);
		if(new_sd == -1)
		{
			error("Server. Could not accept the request");
			return 0;
		}
			// for(int i=0 ; i < 10 ; i++) 
	// {
		threadPool[threadCount].socketId = new_sd;
		if (pthread_cond_signal(&cond[threadCount]) != 0) {
		perror("pthread_cond_signal() error");
		// exit(4);
		}
		else {
			printf("Sending Signal\n");
		}
		threadCount++;
	// }


		// struct Details *myDetails = malloc (sizeof(struct Details));
		// myDetails->send_id = new_sd;
		// myDetails->thread_number = threadCount;
		// rv1 = pthread_create(&threads[threadCount++], NULL, handle_request, (void*) myDetails);
	}

	return 0;
}


//****************** WORKER THREAD handle_request function ******************//

void *handle_request(void *param)
{
	struct Thread *threadDetails = (struct Thread*)param;
	if(threadDetails == NULL) {
	printf("\nTerminating worker thread\n");
		return NULL;
	}

	if (pthread_cond_wait(&cond[threadDetails->threadNumber], &mutex[threadDetails->threadNumber]) != 0) {
    perror("pthread_cond_timedwait() error");
    // exit(7);
	}
	else {
		printf("Conditional wait\n");
	}

	int new_sd=threadDetails->socketId;
	int recv_id=0;
	int send_id=0;
	char fileAddress[1024];
	char welcome[1024] = "\n***** Worker thread number : ";
	printf("%s %d  *****\n", welcome, threadDetails->threadNumber);
	

	// myReceive(new_sd, fileAddress, 1000, 0); // reveive fileAddress
	// printf("The file address is : %s\n", fileAddress);

	// send_file(fileAddress, new_sd);

	// send_id = mySend(new_sd, QUIT ,100, 0);
	// close(new_sd);
	printf("\nTerminating worker thread\n");
	return NULL;
}





//****************** THREAD MANAGEMENT ******************//

int createThreadPool(struct Thread *pool)
{


	for(int i=0 ; i < 102 ; i++) 
	{
		if (pthread_mutex_init(&mutex[i], NULL) != 0) {
	    perror("pthread_mutex_init() error");
	    // exit(1);
	  }
		if (pthread_cond_init(&cond[i], NULL) != 0) {
		perror("pthread_cond_init() error");
		// exit(2);
		}
		if (pthread_mutex_lock(&mutex[i]) != 0) {
		perror("pthread_mutex_lock() error");
		// exit(6);
		}
	}
  	pthread_attr_t attr;
	pthread_attr_init(&attr); // Required!!!
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	// pthread_create(&tid, &attr, foo, NULL);

	struct Thread *threadPool = pool;
	threadPool[0].threadId = 0;

	for(int i=0 ; i < 10 ; i++) 
	{
		struct Details *myDetails = malloc (sizeof(struct Details));
		myDetails->thread_number = i;
		threadPool[i].threadNumber = i;
		if (pthread_create(&threadPool[i].threadId, &attr, handle_request, (void*) &threadPool[i]) != 0)
		{
			printf("Cannot create %d th thread\n", i+1);
		}
		else 
		{
			printf("Created %d th thread\n", i+1);
		}
	}


	return 0;

}


//****************** SENDING FILE send_file function ******************//

void send_file(char *fileName, int new_sd)
{
    char data[1024] ;
    // char fileName[1024] ;
    
    char temp[1024];
    
    
    int send_id;
    FILE *fileptr = NULL;
    fileptr = fopen(fileName, "rb"); // read binary mode
    if(!fileptr)
    {
        error("test.pdf could not open in binary mode");
        return ;
    }
    
    // keep reading from the file until the file is complete
    while(fread(data, 1, 1000, fileptr) != NULL)
    {
        data[1023]='\0';
        send_id = write(new_sd, data ,1000);
        if(send_id == -1)
        {
            printf("Server. Could not send");
            perror("Server. Could not send");
            //			return 0;
        }
        
    }
    int i=0;
    for(i=0 ; i<1024 ; i++)
        data[i] = '\0';
    send_id = write(new_sd, data ,1024);
    if(send_id == -1)
    {
        error("Server. Could not send");
        return ;
    }
    else
        printf("%s File Successfully Sent\n", fileName);
    
    
    fclose(fileptr);	// closing the file
    
    
    // close(s_id);	//closing the socket
}






//****************** HELPER FUNCTIONS ******************//


int myReceive(int socket, char *arr, int length, int flag)
{
	int recv_id = recv (socket, arr, length, flag); 
	if(recv_id == -1 )
	{
		error("Server. Some error occured receiving\n");
		perror("Server. Some error occured receiving\n");
	}
	return recv_id;
}

int mySend(int socket, char *arr, int length, int flag)
{
	int send_id = send(socket, arr, length, flag);
	if(send_id == -1)
	{
		error("Server. Could not send");
		perror("Server. Could not send");
	}
	return send_id;
}

