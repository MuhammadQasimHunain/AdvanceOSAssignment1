// ***** PRJECT PAK-TUBE ***** //
// ***** SUB-SERVER.c FILE ***** //
/*
* It is implemented as TCP
*
*
*
*/

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

struct Details
{
	char msg[1024];
	char fileName[1024];
	int socket_id;
};


int connectToMainServer();
void sendFileNames(int);
void *process_client(void *);
void *new_thread_send_data_to_main_server(void *);
int myReceive(int socket, char *arr, int length, int flag);
int mySend(int socket, char *arr, int length, int flag);

int main()
{
	pthread_t thread1, thread2;
	int rv, rv1, rv2;
	char myData[1024];
	rv = pthread_create(&thread1, NULL, new_thread_send_data_to_main_server, (void*) myData);
	// connectToMainServer();
	char message1[1024] = "Thread1";
	char message2[1024] = "Thread2";



	// NOW WE ARE CREATING A TCP CONNECTION 
	int s_id;
	char msg[1024];
	int msg_length =1024;
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
	listen(s_id,5); 
	int size = sizeof (struct sockaddr_in);



	int new_sd=0;
	int recv_id=0;
	int send_id=0;
	char data[1024];

	while(1)
	{
		// accepting the client's request and assigning new socket
		new_sd = accept (s_id, (struct sockaddr *) &remote_addr, &size);
		if(new_sd == -1)
		{
			error("Server. Could not accept the request");
			return 0;
		}

		// first of all we will set the scket descriptor of the recently
		// accepted client then we will create a new thread in which
		// this socket descriptor would be passed
		struct Details *myDetails = malloc (sizeof(struct Details));
		myDetails->socket_id = new_sd;
  		rv1 = pthread_create(&thread1, NULL, process_client, (void*) myDetails);


	}


}
// end of MAIN function

//**************************//

void *process_client(void *param)
{

	int send_id;
	char arr[1024];
	// getting the socket of the client that is passed from the main thread
	struct Details *details = (struct Details*)param;
	int new_sd=details->socket_id;

	// receive the name of the video file
	int recv_id = read(new_sd, arr, strlen(arr));

	char reply[1024]="Server Says OK ";
	while(1)
		send_id = write(new_sd, reply ,strlen(arr));

	//now we will send the file in binary mode

	// FILE * input,*output;
	// input = fopen ("ns2.pdf","rb");
	// output = fopen ("ns2_copy.pdf","w+b");
	// char file_buffer[1024] = {0};
	// char *bin_data = file_buffer;
	// int filesize,tmp;

	// if (input!=NULL)
	// {
	// 	/* clear the buffer. */
	// 	memset(bin_data, 0 , 1024);

	// 	/* Calculate the total size of the file. */
	// 	fseek(input, 0, SEEK_END);
	// 	filesize = ftell(input);
	// 	rewind(input);
		
	// 	/* Print the file size. */
	// 	printf("\nsize of file in bytes =%d\n", filesize);
		
	// 	/* Calculate the number of chunks. */
	// 	tmp = filesize / 1024;	
		
	// 	 Print the break down details. 
	// 	printf("\n1024 bytes chunk =%d\n", tmp);
	// 	printf("\nremaining chunk =%d\n", filesize % 1024);

	// 	/* copy the 1024 sized chunks. */
	// 	while(tmp != 0)
	// 	{
	// 		fread(bin_data, 1, 1024, input);

	// 		fwrite(bin_data, 1, 1024, output);
			
	// 		memset(bin_data, 0 , 1024);
				
	// 		tmp--;
	// 	}
		
	// 	/* copy the remaining data. */
	// 	if(filesize % 1024 != 0)
	// 	{
	// 		fread(bin_data, 1, filesize % 1024, input);
			
	// 		fwrite(bin_data, 1, filesize % 1024, output);
	// 	}
		
	// 	fclose (input);
	// 	fclose (output);
	// }

	// else
	// {
	// 	printf("\nfailed to open\n");
	// }


}

//**************************//

void *new_thread_send_data_to_main_server(void *param)
{
	FILE *input;
	input = fopen ("sub file names.txt","r");
	if (input == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];
	char msg[] = "Hassaan Fayyaz Ahmed 11-4584 BSCS FAST LHR";
	int msg_len = strlen(msg);

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
   servaddr.sin_port=htons(8888);

   while(fgets(sendline, 100, input) != NULL)
   {
   		// sending name
		sendto(sockfd, sendline, strlen(sendline),0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		// sending size
		fgets(sendline, 100, input);{
			sendto(sockfd, sendline, strlen(sendline),0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		}
		// sending ip
		fgets(sendline, 100, input);{
			sendto(sockfd, sendline, strlen(sendline),0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		}
		// sending port
		fgets(sendline, 100, input);
		{
			sendto(sockfd, sendline, strlen(sendline),0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		}
   }

}

//**************************//


int myReceive(int socket, char *arr, int length, int flag)
{
	int recv_id = recv (socket, arr, length, flag); 
	if(recv_id == -1 )
	{
		error("Server. Some error occured receiving\n");
		perror("Server. Some error occured receiving\n");
		exit(1); // program hi khatam kr dia
		return -1;
	}

	return recv_id;
}

//**************************//

int mySend(int socket, char *arr, int length, int flag)
{
	int send_id = send(socket, arr, length, flag);
	if(send_id == -1)
	{
		error("Server. Could not send");
		error("Server. Could not send");
		return -1;
	}
	return send_id;
}

//**************************//

int connectToMainServer()
{
	//returns 0 if not succssfully connected
	//returns 1 if successfully connected

	int sock_des=0;
	char msg[] = "Hassaan Fayyaz Ahmed 11-4584 BSCS FAST LHR";
	int msg_len = strlen(msg);
	int rbuff_len = 100;
	char rbuff[100] ;

	// the no of bytes that are actually sent
	int bytes_sent = 0;

	int bytes_received = 0;

    //create a UDP socket
	int UDP_sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(UDP_sock == -1)
	{
        //is k andar ni aya matlab socket ban gya
        error("could not create a UDP socket");
    }
  
	struct sockaddr_in UDP_server_addr;
	int socklen = sizeof(UDP_server_addr);

	UDP_server_addr.sin_family = AF_INET;
	UDP_server_addr.sin_port = htons(5555);
	UDP_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// sending server some data
	sendto(UDP_sock,msg,msg_len,0,(struct sockaddr*)&UDP_server_addr,sizeof(UDP_server_addr));
	if(bytes_sent==-1)
	{
		error("An unknown Error Occured\n");
	}
	close(UDP_sock);
	return 1;
}

//**************************//

void sendFileNames(int sd)
{
	int length = 100; // 100 bytes
	FILE *fileptr = NULL;
	fileptr = fopen("sub file names.txt", "r"); // read mode
	if(!fileptr)
	{
		error("sub file names.txt could not open");
	}
	
	int send_id;
	char arr[256];
	// keep reading from the file until the file is complete
	while(fgets(arr, 256, fileptr) != NULL)
	{
		arr[255]='\0';
		send_id = send(sd, arr ,length,0);
		if(send_id == -1)
		{
			error("SUB-SERVER. Could not send file names");
		}
	}

	arr[0] = '\0';	
	send_id = send(sd, arr ,length,0);
	if(send_id == -1)
	{
		error("SUB-SERVER. Could not send file names");
	}
	fclose(fileptr);	// closing the file
	fileptr = NULL;

}

//**************************//
