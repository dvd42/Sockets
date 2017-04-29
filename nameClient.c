

/*
*  C Implementation: nameClient
*
* Description: 
*
*
* Author: MCarmen de Toro <mc@mc>, (C) 2015
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "nameClient.h"

/**
 * Function that sets the field addr->sin_addr.s_addr from a host name 
 * address.
 * @param addr struct where to set the address.
 * @param host the host name to be converted
 * @return -1 if there has been a problem during the conversion process.
 */
int setaddrbyname(struct sockaddr_in *addr, char *host)
{
  struct addrinfo hints, *res;
	int status;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM; 
 
  if ((status = getaddrinfo(host, NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return -1;
  }
  
  addr->sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
  
  freeaddrinfo(res);
    
  return 0;  
}

/**
 * Function that gets the dns_file name and port options from the program 
 * execution.
 * @param argc the number of execution parameters
 * @param argv the execution parameters
 * @param reference parameter to set the host name.
 * @param reference parameter to set the port. If no port is specified 
 * the DEFAULT_PORT is returned.
 */
int getProgramOptions(int argc, char* argv[], char *host, int *_port)
{
  int param;
  *_port = DEFAULT_PORT;

  // We process the application execution parameters.
	while((param = getopt(argc, argv, "h:p:")) != -1){
		switch((char) param){		
			case 'h':
				strcpy(host, optarg);				
				break;
			case 'p':
				// Donat que hem inicialitzat amb valor DEFAULT_PORT (veure common.h) 
				// la variable port, aquest codi nomes canvia el valor de port en cas
				// que haguem especificat un port diferent amb la opcio -p
				*_port = atoi(optarg);
				break;				
			default:
				printf("Parametre %c desconegut\n\n", (char) param);
				return -1;
		}
	}
	
	return 0;
}

/**
 * Shows the menu options. 
 */
void printa_menu()
{
		// Mostrem un menu perque l'usuari pugui triar quina opcio fer

		printf("\nAplicatiu per la gestió d'un DNS Server\n");
		printf("  0. Hola mon!\n");
		printf("  1. Llistat dominis\n");
		printf("  2. Consulta domini\n");
		printf("  3. Alta Ip\n");
		printf("  4. Alta Ips\n");
		printf("  5. Modificacio Ip\n");
		printf("  6. Baixa Ip\n");
		printf("  7. Baixa Domini\n");
		printf("  8. Sortir\n\n");
		printf("Escolliu una opcio: ");
}

/**
 * Function that sends a list request receives the list and displays it.
 * @param s The communications socket. 
 */
void process_list_operation(int s)
{
  char buffer[DNS_TABLE_MAX_SIZE];
  int msg_size = 0;
 
  sendOpCodeMSG(s, MSG_LIST_RQ);

  msg_size = recv(s,buffer,sizeof(buffer),0);
  if (msg_size < 0) {
		perror("ERROR reading from socket");
		exit(4);
	}

  printDNSTableFromAnArrayOfBytes(buffer+sizeof(short), msg_size-sizeof(short));
}


void process_hello_operation(int s){


	int n = 0;
	char buffer[MAX_BUFF_SIZE];

  sendOpCodeMSG(s,MSG_HELLO_RQ);

 	n = recv(s,buffer,sizeof(buffer),0);
	if (n < 0) {
  	perror("ERROR reading from socket");
  	exit(4);
	}

	printf("%s\n", buffer + sizeof(short));

}

void process_checkDomain_operation(int s){ 

	int n = 0;
	char buffer[MAX_BUFF_SIZE];
	char domain[NAME_LENGTH];
	int offset = 0;
	int op_code = 0;
	int op_code2 = 0;

	memset(buffer, 0, sizeof(buffer));

	//TODO replace scanf with fgets

	printf("Enter the domain you want to check\n\n");
	scanf("%s", domain);

	stshort(MSG_DOMAIN_RQ, buffer);
	offset = sizeof(short);

	//Insert the domain we want to check into the buffer
	memcpy(buffer + offset, domain, strlen(domain));
	offset += strlen(domain) + SPACE_BYTE_SIZE;

	n = send(s,buffer,offset,0); 
	if (n < 0) {
  	perror("ERROR writing to socket");
  	exit(5);
	}

	n = recv(s,buffer,sizeof(buffer),0);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(4);
  } 

  //If the domain exists print every Ip address associated to it
  if(n != 4){
	  offset = sizeof(short);
	  while(offset < n){
	  	printf("%s\n",inet_ntoa(*((struct in_addr*)(&ldaddr(buffer + offset)))));
	  	offset += sizeof(struct in_addr);
  	}
  }
  else{
  	//Extract the error codes from buffer and print it to stdout
  	op_code = ldshort(buffer);
  	op_code2 = ldshort(buffer + sizeof(short));
  	printf("Code %d.%d\nThe domain has not been found\n", op_code, op_code2);	 
  }	
}


void process_addDomainIP_operation(int s){

	char buffer[MAX_BUFF_SIZE];
	char domain[NAME_LENGTH];
	int offset = 0;
	struct in_addr address;
	int n = 0;
	char ip[MAX_ADDR_SIZE];
	char moreIPs = 'n';
	int op_code;

	//Insert message code into buffer
	memset(buffer, 0, sizeof(buffer));
	stshort(MSG_ADD_DOMAIN,buffer);
	offset = sizeof(short);

	//TODO replace scanfs with fgets

	printf("Enter the domain you want to add\n");
	scanf("%s", domain);
	
	//Insert the domain into buffer
	memcpy(buffer + offset, domain, strlen(domain));
	offset += strlen(domain) + SPACE_BYTE_SIZE;

	do{

		printf("Enter the Ip you want to add\n");
		scanf("%s", ip);

		//Insert ip in network format into buffer
		inet_aton(ip, &address);
		staddr(address, buffer + offset);
		offset += sizeof(address);

		printf("Do you want to add more IPs to this domain (y/n)\n");
		fflush(stdin);
		scanf(" %c", &moreIPs);


		while(moreIPs != 'y' && moreIPs != 'n'){ 
			printf("Please enter 'y' or 'n'\n");
			fflush(stdin);
			scanf(" %c", &moreIPs);

		}

	
	}while(moreIPs != 'n');


	n = send(s,buffer,offset,0); 
	if (n < 0) {
  	perror("ERROR writing to socket");
  	exit(5);
	}

	n = recv(s,buffer,sizeof(buffer),0);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(4);
  } 
  op_code = ldshort(buffer);
  printf("Code %d\nThe operation was completed successfully\n", op_code);

}


void process_changeDomain_operation(int s){

	char buffer[MAX_BUFF_SIZE];
	char domain[NAME_LENGTH];
	int offset = sizeof(short);
	struct in_addr address;
	int n = 0;
	char ip[MAX_ADDR_SIZE];
	char ip2[MAX_ADDR_SIZE];
	int op_code;
	int op_code2;
	
	memset(buffer, 0, sizeof(buffer));
	stshort(MSG_CHANGE_DOMAIN, buffer);

	printf("Enter the domain you want to change\n");
	scanf("%s", domain);
	
	//Insert the domain into buffer
	memcpy(buffer + offset, domain, strlen(domain));
	offset += strlen(domain) + SPACE_BYTE_SIZE;

	printf("Enter the IP you want to replace\n");
	fflush(stdin);
	scanf("%s", ip);


	printf("Enter the new IP\n");
	fflush(stdin);
	scanf("%s", ip2);

	//Insert both IPs into buffer in dotted quad format
	inet_aton(ip, &address);
	staddr(address, buffer + offset);
	offset += sizeof(address);
	
	inet_aton(ip2, &address);
	staddr(address, buffer + offset);
	offset += sizeof(address);

	n = send(s,buffer,offset,0); 
	if (n < 0) {
  	perror("ERROR writing to socket");
  	exit(5);
	}


	n = recv(s,buffer,sizeof(buffer),0);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(4);
  } 

 	if(n == 2){
	  op_code = ldshort(buffer);
	  printf("Code %d\n The operation was completed successfully", op_code);
	}
	else{
		op_code = ldshort(buffer);
  	op_code2 = ldshort(buffer + sizeof(short));
  	
  	if(op_code2 == 2)
  		printf("Code %d.%d\nThe domain has not been found\n", op_code, op_code2);	 
  	else
  		printf("Code %d.%d\nThe IP has not been found\n", op_code, op_code2);	 


	}
}


void process_deleteIP_operation(int s){

	char buffer[MAX_BUFF_SIZE];
	char domain[NAME_LENGTH];
	int offset = sizeof(short);
	struct in_addr address;
	int n = 0;
	char ip[MAX_ADDR_SIZE];
	int op_code;
	int op_code2;


	memset(buffer, 0, sizeof(buffer));
	stshort(MSG_DEL_IP, buffer);

	printf("Enter the domain you want to change\n");
	scanf("%s", domain);
	
	//Insert the domain into buffer
	memcpy(buffer + offset, domain, strlen(domain));
	offset += strlen(domain) + SPACE_BYTE_SIZE;

	printf("Enter the IP you want to delete\n");
	fflush(stdin);
	scanf("%s", ip);

	//Insert IP into buffer in dotted quad format
	inet_aton(ip, &address);
	staddr(address, buffer + offset);
	offset += sizeof(address);

	n = send(s,buffer,offset,0); 
	if (n < 0) {
  	perror("ERROR writing to socket");
  	exit(5);
	}

	n = recv(s,buffer,sizeof(buffer),0);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(4);
  } 

  if(n == 2){
	  op_code = ldshort(buffer);
	  printf("Code %d\n The operation was completed successfully", op_code);
	}
	else{
		op_code = ldshort(buffer);
  	op_code2 = ldshort(buffer + sizeof(short));
  	
  	if(op_code2 == 2)
  		printf("Code %d.%d\nThe domain has not been found\n", op_code, op_code2);	 
  	else
  		printf("Code %d.%d\nThe IP has not been found\n", op_code, op_code2);	 
	}

}

void process_deleteDomain_operation(int s){


	char buffer[MAX_BUFF_SIZE];
	char domain[NAME_LENGTH];
	int offset = sizeof(short);
	int n = 0;
	int op_code;
	int op_code2;

	memset(buffer, 0, sizeof(buffer));
	stshort(MSG_DEL_DOMAIN, buffer);

	printf("Enter the domain you want to delete\n");
	scanf("%s", domain);
	
	//Insert the domain into buffer
	memcpy(buffer + offset, domain, strlen(domain));
	offset += strlen(domain) + SPACE_BYTE_SIZE;

	n = send(s,buffer,offset,0); 
	if (n < 0) {
  	perror("ERROR writing to socket");
  	exit(5);
	}

	n = recv(s,buffer,sizeof(buffer),0);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(4);
  } 

  if(n == 2){
	  op_code = ldshort(buffer);
	  printf("Code %d\n The operation was completed successfully", op_code);
	}
	else{
		op_code = ldshort(buffer);
  	op_code2 = ldshort(buffer + sizeof(short));
		printf("Code %d.%d\nThe domain has not been found\n", op_code, op_code2);	 

	}

}

/** 
 * Function that process the menu option set by the user by calling 
 * the function related to the menu option.
 * @param s The communications socket
 * @param option the menu option specified by the user.
 */
void process_menu_option(int s, int option)
{		  
  switch(option){
    // Opció HELLO
    case MENU_OP_HELLO:
  		process_hello_operation(s);		
  		break;

    case MENU_OP_LIST:
      process_list_operation(s);
      break;

    case MENU_OP_DOMAIN_RQ:
    	process_checkDomain_operation(s);
    	break;
    case MENU_OP_ADD_DOMAIN_IP:
    	process_addDomainIP_operation(s);
    	break;

    case MENU_OP_ADD_DOMAIN_IPS:
    	process_addDomainIP_operation(s);
    	break;

    case MENU_OP_CHANGE:
    	process_changeDomain_operation(s);
    	break;

    case MENU_OP_DELETE_IP:
    	process_deleteIP_operation(s);
    	break;

    case MENU_OP_DELETE_DOMAIN:
    	process_deleteDomain_operation(s);	
    	break;

    case MENU_OP_FINISH:
    	sendOpCodeMSG(s,MSG_FINISH);
      break;

  
    default:
      	printf("Invalid menu option\n");
  		}
}

int main(int argc, char *argv[])
{
	int port; // variable per al port inicialitzada al valor DEFAULT_PORT (veure common.h)
	char host[MAX_HOST_SIZE]; // variable per copiar el nom del host des de l'optarg
	int option = 0; // variable de control del menu d'opcions
	int ctrl_options;

	struct sockaddr_in serv_addr;
	int sockfd;
  
  ctrl_options = getProgramOptions(argc, argv, host, &port);
              
	// Comprovem que s'hagi introduit un host. En cas contrari, terminem l'execucio de
	// l'aplicatiu	
	if(ctrl_options<0){
		perror("No s'ha especificat el nom del servidor\n\n");
		return -1;
	}

	//Create client socket
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
 	if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

	//Initialize socket structure
	memset(&serv_addr,'\0',sizeof(serv_addr));  
	serv_addr.sin_family = AF_INET;  
 	serv_addr.sin_port = htons(port);
 	if(setaddrbyname(&serv_addr, host) == -1) {
  	perror("ERROR setting address");
  	exit(2);
  }

 	socklen_t servlen = sizeof(serv_addr);

 	//Connect with the server
	if (connect(sockfd, (struct sockaddr*)&serv_addr, servlen) < 0) {
  	perror("ERROR connecting");
  	exit(3);	
	}

  do{
      printa_menu();
		  // getting the user input making sure its an integer.
     	while(scanf("%d",&option) != 1){
        printf("Please enter an integer: ");
        while(getchar() != '\n');
    	}

		  printf("\n\n");
		  process_menu_option(sockfd, option);

	  }while(option != MENU_OP_FINISH); //end while(opcio)
    // TODO

  return 0;
}

