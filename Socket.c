#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 


int main(int argc, char* argv[]){


	int listenfd = 0, connfd = 0;

	struct sockaddr_in serv_addr;


	char buffer[1025];


	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, '0', sizeof(buffer));
	memset(buffer, '0', sizeof(buffer));


	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port =htons(5000);


	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);



	while(1){

		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

		snprintf(buffer, sizeof(buffer), "%s\n", "Hello World");

		write(connfd, buffer, strlen(buffer));	

		sleep(1);	

	}





	return 0;
}