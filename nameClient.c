

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
  int port;
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
				port = atoi(optarg);
				*_port = port;
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
  int msg_size;

  //TODO: uncomment sendOpCodeMSG(s, MSG_LIST_RQ); //remember to implement sendOpCode in common.c
  memset(buffer, '\0', sizeof(buffer));
  //TODO: rebre missatge LIST
  //TODO: Descomentar la següent línia
  //printDNSTableFromAnArrayOfBytes(buffer+sizeof(short), msg_size-sizeof(short));
}


void communicate(int op_code, int s, int n){

	char buffer[MAX_BUFF_SIZE];
	op_code = 01;

  sendOpCodeMSG(s,op_code);

 	n = recv(s,buffer,sizeof(buffer),0);
	if (n < 0) {
  	perror("ERROR reading from socket");
  	exit(1);
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
	short op_code = 0;
	int n = 0;


  switch(option){
    // Opció HELLO
    case MENU_OP_HELLO:
  		communicate(op_code,s,n);		
  		break;

    case MENU_OP_LIST:
      process_list_operation(s);
      break;
    case MENU_OP_FINISH:
      //TODO:
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
	memset(&serv_addr,'0',sizeof(serv_addr));  
	serv_addr.sin_family = AF_INET;
 	serv_addr.sin_addr.s_addr = inet_addr(host);
 	serv_addr.sin_port = htons(port);

 	//Connect with the server
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
  	perror("ERROR connecting");
  	exit(1);
	}

  do{
      printa_menu();
		  // getting the user input.
		  scanf("%d",&option); //FIXME if you introduce a letter it goes into infinite Loop
		  printf("\n\n"); 
		  process_menu_option(sockfd, option);

	  }while(option != MENU_OP_FINISH); //end while(opcio)
    // TODO

  return 0;
}

