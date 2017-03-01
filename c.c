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
#include <netdb.h>// #include <conio.h>


#define PORT_NO							5560
#define QUIT_CONNECTION 				"QUIT"
#define NEW_CONNECTION 					"NEW-CONNECTION"
#define FILE_SUCCESSFULLY_RECEIVED		"FILE-SUCCESSFULLY-RECEIVED"

#define BUFFER_SIZE 1024
#define MAXLINE 1024


int myReceive(int socket, char *arr, int length, int flag);
int mySend(int socket, char *arr, int length, int flag);
void parse_command_line_arguments(int argc, char* argv[], int*, int* );
void create_required_threads();
int socket_connect(char *host, in_port_t port);



//****************** MAIN FUNCTION ******************//

    // #MARK: - hello
int main(int argc, char* argv[] )
{
	// system("clear");

	// int totalThreads = 0;
	// int requestsPerThread = 0;

	// int s_id;
	// char msg[100] =""; 
	// int msg_length = 100;
	// struct sockaddr_in serv_addr;
	// s_id = socket (PF_INET,SOCK_STREAM,0);
	// if(s_id == -1)
	// {	
	// 	perror("Client. Could not Assign Socket\n");
	// 	return 0;
	// }		

	// // parse_command_line_arguments(argc, argv, &totalThreads, &requestsPerThread);
 // //    printf("The number of threads : %d\n", totalThreads);
	// // printf("The number of requests per thread : %d\n", requestsPerThread);

	// // create_required_threads();

	// struct hostent *hp;
	// int on = 0;
	// if((hp = gethostbyname("www.google.com")) == NULL){
	// 	herror("gethostbyname");
	// 	exit(1);
	// }
	// bcopy(hp->h_addr, &serv_addr.sin_addr, hp->h_length);

	// serv_addr.sin_family=AF_INET;
	// serv_addr.sin_port = htons (5555);
	// // serv_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
	// // serv_addr.sin_addr.s_addr = inet_addr ("www.google.com");
	// s_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	// setsockopt(s_id, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	// // for(int i=0 ; i < 10 ; i++) 
	// {
	// 	// requesting to connect using the accept function
	// 	int connect_id=connect(s_id,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
	// 	if(connect_id == -1)
	// 	{
	// 		error("Client. Could not Connect. \n");
	// 		return 0;
	// 	}
	// 	// s_id = socket (PF_INET,SOCK_STREAM,0);
	// 	// if(s_id == -1)
	// 	// {	
	// 	// 	perror("Client. Could not Assign Socket.\n");
	// 	// 	return 0;
	// 	// }		

	// 	// sleep(1);
	// }

	// // int fd;
	// // char buffer[BUFFER_SIZE];

	// // if(argc < 3){
	// // 	fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
	// // 	exit(1); 
	// // }
       
	// // fd = socket_connect(argv[1], atoi(argv[2])); 

	// //connection has been established till here



	// // char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	// // char* ptr;
	// // char poststr[MAXLINE];
	// // size_t n;

	// // /// Form request
	// // snprintf(sendline, MAXSUB, 
	// //      "GET %s HTTP/1.0\r\n"  // POST or GET, both tested and works. Both HTTP 1.0 HTTP 1.1 works, 
	// //      "Host: %s\r\n"     // but sometimes HTTP 1.0 works better in localhost type
	// //      "Content-type: application/x-www-form-urlencoded\r\n"
	// //      "Content-length: %d\r\n\r\n"
	// //      "%s\r\n", page, host, (unsigned int)strlen(poststr), poststr);

	// // /// Write the request
	// // if (write(sockfd, sendline, strlen(sendline))>= 0) 
	// // {
	// //     /// Read the response
	// //     while ((n = read(sockfd, recvline, MAXLINE)) > 0) 
	// //     {
	// //         recvline[n] = '\0';

	// //         if(fputs(recvline,stdout) == EOF) { cout << ("fputs erros"); }
	// //         /// Remove the trailing chars
	// //         ptr = strstr(recvline, "\r\n\r\n");

	// //         // check len for OutResponse here ?
	// //         snprintf(OutResponse, MAXRESPONSE,"%s", ptr);
	// //     }          
	// // }

	// char buffer[BUFFER_SIZE];
	// char requestStr[BUFFER_SIZE] = "GET /www.google.com \r\n";
	// write(s_id, requestStr, strlen(requestStr)); // write(fd, char[]*, len);  
	// bzero(buffer, BUFFER_SIZE);
	
	// while(read(s_id, buffer, BUFFER_SIZE - 1) != 0){
	// 	fprintf(stderr, "%s", buffer);
	// 	bzero(buffer, BUFFER_SIZE);
	// }


	// int termination=0;
	// int send_id=0;
	// int recv_id=0;
	// char s_data[1024]="";
	// char r_data[1024]="";

	// char fileAddress[1024];
	// char welcome[1024];

	// {
	// 	// while(1)
	// 	{
	// 		// s_data[0] = '\0';
	// 		// printf("Enter file address : ");
	// 		// scanf("%s", fileAddress);
	// 		// send_id = mySend(s_id, fileAddress ,1000, 0); //send user name

	// 		// printf("Enter password : ");
	// 		// scanf("%s", pwd);
	// 		// send_id = mySend(s_id, pwd ,1000, 0); //send password

	// 		// recv_id = myReceive (s_id,confirmation,1000, 0);
	// 		// if(strcmp(confirmation,QUIT_CONNECTION) == 0) { // same strings
	// 		// 	printf("\nServer terminated connection\n");
	// 			// break;
	// 		// }
	// 	}
	// // char data[1024] ;

	// // char newOutputFileName[1024]= "myOutput";
	// // strcat(newOutputFileName, fileAddress);

	// // FILE *fileptr = fopen(newOutputFileName, "wb"); // write binary mode
	// // if(!fileptr)
	// // {
	// // 	error("%s could not open in binary mode",newOutputFileName);
	// // 	perror(" could not open file in binary mode");
	// // 	return 0;
	// // }
	// //  // else
	// //  // 	printf("%s Successfully Opened in binary mode\n", outputFile);
	// // bool isDataLeft=false;

	// // while(1)
	// // {
	// // 	// receiving messages from the server
	// // 	recv_id = read (s_id,data,1000);

	// // 	if(recv_id == -1 )
	// // 	{
	// // 		error("Client. Some error occured while receiving file\n");
	// // 	//	return 0;
	// // 	}
	// // 	// check if more data has to be received
	// // 	int i=0;
	// // 	for(i=0 ; i<1024 ; i++) {
	// // 		if(data[i] != '\0') {
	// // 			isDataLeft = true;
	// // 			break;
	// // 		}
	// // 	}
	// // 	if(isDataLeft == false) {
	// // 		printf("\n***** File successfully received. *****\n");
	// // 		break;
	// // 	}

	// // 	termination++;
	// // //	printf("%s \n", input);
	// // 	if(recv_id <= 0)
	// // 		break;
	// // 	else
	// // 		fwrite(data, 1, 1000, fileptr);
	// // }

	// // fclose(fileptr);


	// // 	r_data[0] = '\0';


	// }

	// close(s_id);
	// return 0;

		int fd;
	char buffer[BUFFER_SIZE];

	// if(argc < 3){
	// 	fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
	// 	exit(1); 
	// }
       
	// fd = socket_connect(argv[1], atoi(argv[2])); 
	char hostname[1024] = "127.0.0.1";
	char completeRequest[1024] = "GET / HTTP/1.0\r\n\r\n";
	fd = socket_connect(hostname, PORT_NO); 
	write(fd, completeRequest, strlen(completeRequest)); // write(fd, char[]*, len);  
	bzero(buffer, BUFFER_SIZE);
	
    long totalBytesRead = 0;
    long bytesRead = read(fd, buffer, BUFFER_SIZE - 1);
	while(bytesRead != 0 && bytesRead != -1){
        totalBytesRead += bytesRead;
		fprintf(stderr, "%s", buffer);
		bzero(buffer, BUFFER_SIZE);
        bytesRead = read(fd, buffer, BUFFER_SIZE - 1);
	}
    
    printf("Total bytes received from server : %ld\n", totalBytesRead);

	shutdown(fd, SHUT_RDWR); 
	close(fd); 

    return 0;
}


//****************** MAKE CONNECTION ******************//

int socket_connect(char *host, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;     

	if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	printf("%s\n",hp->h_name );
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(sock == -1){
		perror("setsockopt");
		exit(1);
	}
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("error on connect");
		exit(1);

	}
	return sock;
}
 
//****************** HELPER FUNCTIONS ******************//

void create_required_threads() 
{
	int s_id;
//	char msg[100] =""; 
//	int msg_length = 100;
	struct sockaddr_in serv_addr;
	s_id = socket (PF_INET,SOCK_STREAM,0);
	if(s_id == -1)
	{	
		perror("Client. Could not Assign Socket");
		// return 0;
	}		

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port = htons (PORT_NO);
	serv_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");

	// requesting to connect using the accept function
	int connect_id=connect(s_id,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
	if(connect_id == -1)
	{
		error("Client. Could not Connect ");
		// return 0;
	}		

    
    
	// Yahan pay ziada saray threads bnanay hain
	// us k baad har individual thread kaam karay ga.

	// struct Details *myDetails = malloc (sizeof(struct Details));
	// myDetails->send_id = s_id;
	// int rv1 = pthread_create(&threads[threadCount++], NULL, handle_request, (void*) myDetails);

}


void parse_command_line_arguments(int argc, char* argv[], int *totalThreads, int *requestsPerThread ) 
{

    char c;    
    //Parsing the command line arguments
   if( argc == 3 ) {
     	*totalThreads = atoi(argv[1]);
     	*requestsPerThread = atoi(argv[2]);
   }
   else if( argc > 3 ) {
      printf("Too many arguments supplied.\n");
      exit(1);
   }
   else {
      printf("2 arguments expected.\n");
      exit(1);
   }
}

int myReceive(int socket, char *arr, int length, int flag)
{
	int recv_id = recv (socket, arr, length, flag); 
	if(recv_id == -1 )
	{
		error("Client. Some error occured receiving\n");
		perror("Client. Some error occured receiving\n");
		close(socket);
		exit(1); 
	}
	else if(strcmp(arr, QUIT_CONNECTION) == 0) { // same strings
		printf("\nServer terminated connection\n");
		close(socket);
		exit(1); 
	}
	else if(strcmp(arr, FILE_SUCCESSFULLY_RECEIVED) == 0) { // same strings
		printf("\n***** File successfully received. *****\n");
	}
	return recv_id;
}

int mySend(int socket, char *arr, int length, int flag)
{
	int send_id = send(socket, arr, length, flag);
	if(send_id == -1)
	{
		error("Client. Could not send");
		perror("Client. Could not send");
		exit(1);
		return -1;
	}
	return send_id;
}

