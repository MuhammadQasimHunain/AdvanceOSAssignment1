// ***** PRJECT PAK-TUBE ***** //
// ***** MAIN-SERVER.c FILE ***** //
/*
* It is implemented as UDP.
* It will handle search requests.
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

struct videoDetails{
	char name[1024];
	char size[1024];
	char ip[1024];
	char port[1024];
};

struct Details
{
	char msg[1024];
	int recv_id;
	int socket_id;
	int port_no;
};
// GLOBAL VARIABLES

struct videoDetails _videos[1024];
int _videoIndex=0;


void *processing_sub_server(void *);
void *processing_client(void *);
int addNewSubServer();
void handleSearchRequest(char *);
void receiveFileNames();
char* toLower(char*);


int main()
{
	// initialize videos wala kaam when the server first sets up
	pthread_t thread1, thread2;
	int rv, rv1, rv2;
	char myData[1024];
	int rbuff_len = 100;
	char rbuff[100] ;

	// struct Details *myDetails = malloc (sizeof(struct Details));
	// myDetails->socket_id = sock;
  	rv = pthread_create(&thread1, NULL, processing_sub_server, (void*) myData);
  	rv1 = pthread_create(&thread1, NULL, processing_client, (void*) myData);


  	int i=0;
  	while(i<10000){i++;}// introducing some wait

	// printf("%s\n",_videos[0].name );
	// printf("%s\n",_videos[0].size );
	// printf("%s\n",_videos[0].ip );
	// printf("%s\n",_videos[0].port );




  	while(1){}
    //create a UDP socket
/*	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
	{
        //is k andar ni aya matlab socket ban gya
        error("could not create a UDP socket");
    }

	struct sockaddr_in server_addr;
	int socklen = sizeof(server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888); //for subserver
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sock,(struct sockaddr *)&server_addr,sizeof(server_addr));


	// sending server some data
	// sendto(sock,msg,msg_len,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	while(1)
	{
		// keep on receiving data from the sub servers
		recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);

		printf("-----------------\n%s\n---------------\n",rbuff );
	}

	close(sock);





*/



	return 0;
}





void *processing_sub_server(void *param)
{
	FILE *output;
	char file_buffer[1024] = {0};
	char *bin_data = file_buffer;
	int filesize,tmp;

	int sock_des=0;
	int rbuff_len = 100;
	char rbuff[100] ;

    //create a UDP socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sock == -1)
	{
        //is k andar ni aya matlab socket ban gya
        error("could not create a UDP socket");
    }

	struct sockaddr_in server_addr;
	int socklen = sizeof(server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sock,(struct sockaddr *)&server_addr,sizeof(server_addr));
	// printf("Test 1\n");

	// sending server some data
	// sendto(sock,msg,msg_len,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	int bytes_received=0;
	int videos_count=0;
	while(1)
	{
	output = fopen ("main file names.txt","a");
	if (output == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
			memset(rbuff, 0 , 99);
		// receiving name
		 bytes_received = recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);
		 if(bytes_received > 1)
		 {
		// printf("-----------------\n%s\n---------------\n",rbuff );
//			 _videos[_videoIndex].name[0] = *rbuff;
		 	rbuff[strlen(rbuff)-1]='\0';
			strcpy(_videos[_videoIndex].name, rbuff);
			fputs(rbuff, output);
			fputs("\n", output);
			printf("%s\n",_videos[videos_count].name );

		 }
	memset(rbuff, 0 , 99);
		 // receiving size
		 bytes_received = recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);
		 if(bytes_received > 1)
		 {
		// printf("-----------------\n%s\n---------------\n",rbuff );
		 	rbuff[strlen(rbuff)-1]='\0';
			strcpy(_videos[_videoIndex].size, rbuff);
			fputs(rbuff, output);
			fputs("\n", output);
			printf("%s\n",_videos[videos_count].size );

		 }
		memset(rbuff, 0 , 99);
		 // receiving ip
		 bytes_received = recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);
		 if(bytes_received > 1)
		 {
		// printf("-----------------\n%s\n---------------\n",rbuff );
		 	rbuff[strlen(rbuff)-1]='\0';
			strcpy(_videos[_videoIndex].ip, rbuff);
			fputs(rbuff, output);
			fputs("\n", output);
			printf("%s\n",_videos[videos_count].ip );

		 }
		memset(rbuff, 0 , 99);
		 //receiving port
		 bytes_received = recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);
		 if(bytes_received > 1)
		 {
		// printf("-----------------\n%s\n---------------\n",rbuff );
		 	rbuff[strlen(rbuff)-1]='\0';
			strcpy(_videos[_videoIndex].port, rbuff);
			fputs(rbuff, output);
			fputs("\n", output);
			printf("%s\n",_videos[videos_count].port );
			// fprintf(output, "%s", rbuff );
		 }
		_videoIndex++;
		videos_count++;
		memset(rbuff, 0 , 99);
		// printf("-----------------\n%s\n---------------\n",rbuff );


	fclose (output);
	}


	sendto(sock,rbuff, 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));

	close(sock);

}

void *processing_client(void *param)
{

	int sock_des=0;
	int rbuff_len = 100;
	char rbuff[100] ;
	char reply[1024] ;


    //create a UDP socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sock == -1)
	{
        //is k andar ni aya matlab socket ban gya
        error("could not create a UDP socket");
        perror("could not create a UDP socket");
    }

	struct sockaddr_in server_addr;
	int socklen = sizeof(server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666); // for client
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sock,(struct sockaddr *)&server_addr,sizeof(server_addr));
	while(1)
	{
		memset(rbuff, 0, 99);
		// keep on receiving data from the sub servers
		recvfrom(sock,rbuff,100,0,(struct sockaddr*)&server_addr,&socklen);

		rbuff[strlen(rbuff)]='\0';
		int flag=0;
		int i=0;
		char *temp1=NULL, *temp2=NULL;
		for(i=0 ; i<_videoIndex ; i++){
			temp1=toLower(_videos[i].name);
			temp2=toLower(rbuff);
			printf("%s\n",temp1 );
			printf("%s\n",temp2 );
			if(strstr(temp1, temp2) != NULL){// if some part of the string matches
				if(flag == 0){
					// for the first time send a +ve acknowledgement
					// and then keep on sending video information as
					// soon as it matches the searching criteria
					sendto(sock, "YES", 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
					flag=2;
				}
				if(flag == 2){
					sendto(sock, _videos[i].name, 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
					sendto(sock, _videos[i].size, 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
					sendto(sock, _videos[i].ip, 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
					sendto(sock, _videos[i].port, 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));

					
				}
			}
		}

		printf("-----------------\n%s\n---------------\n",rbuff );
		if(flag == 0){
			// sending a negative acknoledgement when the video is not found
			sendto(sock,"NOT FOUND", 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
		}
		else if(flag == 2){
			// At the end send of sending details a message that would tell the client that these were the videos
			// found on the server
			sendto(sock, "END", 100, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));

		}

		

	}

	close(sock);

}
	


int addNewSubServer()
{
	//returns 0 if not successfully added
	//returns 1 if successfully added


}


void handleSearchRequest(char *str)
{


}



void receiveFileNames(int socket)
{

	int recv_id;
	int termination=0;
	char arr[256] ;

	while(1)
	{
		// receiving messages from the server
		recv_id = recv (socket, arr, 256, 0);

		if(recv_id == -1 )
		{

			perror("MAIN-SERVER. Could not Receive");
			error("MAIN-SERVER. Could not Receive");
		}
		if(arr[0] == '\0')
		{
			printf("\n*** All Videos Successfully Listed ***\n");

			break;
		}
		termination++;
	//	printf("%s \n", input);
		if(recv_id > 0)
			printf("%s", arr);
		else 
			break;
	}

}

char* toLower(char * arr)
{
	char *temp = malloc(sizeof(arr));
	int i=0;
	for(i ; i<strlen(arr) ; i++){
		if(arr[i]>='A' && arr[i]<='Z' )
			temp[i] = arr[i]+'a'-'A';
		else if(arr[i]>='a' && arr[i]<='z')
			temp[i] = arr[i];
		else
			temp[i] = arr[i];

	}
	return temp;
}
