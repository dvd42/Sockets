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


#include <unistd.h> //per tipus com el socklen_t
#include <netdb.h> //pel gethostbyname
#include <errno.h> //per gestionar errors
#include <sys/types.h> //per tipus com el uint
#include <netinet/in.h> //pel INADDR_ANY i struct socaddr_in i struct in_addr
#include <sys/socket.h> //per la creació de sockets
#include <arpa/inet.h> //per la funcio inet_ntoa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 100
#define SPACE_BYTE_SIZE 1
#define MAX_BUFF_SIZE 2000

#define DEFAULT_PORT 8608

// Macros per afegir i extreure el codi d'operacio d'un missatge
#define stshort(sval, addr) ( *((short *)(addr))=htons(sval) )
#define ldshort(addr) ( ntohs(*((short *)(addr)) ) ) 

//Macros per afegir i i extreure addreces.
//Les adreces no són un long són un in_addr_t:  typedef uint32_t in_addr_t;
#define staddr(aval, addr) ( *((in_addr_t *)(addr))=aval )
#define ldaddr(addr) ( *((in_addr_t *)(addr))  )


#define MSG_HELLO_RQ 1
#define MSG_HELLO 2
#define MSG_LIST_RQ 3
#define MSG_LIST 4
#define MSG_DOMAIN_RQ 5
#define MSG_IP_LIST 6
#define MSG_ADD_DOMAIN 7
#define MSG_CHANGE_DOMAIN 8
#define MSG_DEL_IP 9
#define MSG_DEL_DOMAIN 10
#define MSG_OP_OK 11
#define MSG_OP_ERR 12
#define MSG_FINISH 13

#define ERR_1 1
#define ERR_2 2

/* structure that wraps an IP with the format uint32_t and points to the next IP
or null if there is no more IPs*/
struct _IP
{
  struct in_addr IP;
  struct _IP* nextIP;
};

/* structure that wraps a domain name entry in the domain name table. For each
domain we can have a list of Ips,no more than 10. The entry point to the next
entry in the domain name table; and the number of IP's of the domain */
struct _DNSEntry
{
  char domainName[NAME_LENGTH];
  struct _IP *first_ip;
  int numberOfIPs; 
  struct _DNSEntry *nextDNSEntry;
};


/* The dns table is an array of dns entries. It just can handle 50 dns entries*/
struct _DNSTable
{
  struct _DNSEntry *first_DNSentry;

};


/*Prints the dns table memory structure content to the stdout*/
void printDNSTable(struct _DNSTable* dns_table);


/**
 * Prints the message representing DNS table in a user readable mode. 
 * Prints for each domain name its list of IP's. Each domain is printed in 
 * one line.
 * @param dns_table the array of bytes representing the DNS table without the
 * operation code
 * @param number_of_bytes the size of the message
 */
void printDNSTableFromAnArrayOfBytes(char *dns_table, int number_of_bytes);

/**
 * Function that sends an operation code as an array of bytes through 
 * a socket
 * @param sock the socket to use to the communication
 * @param the operation code to be sent as an array of bytes
 */
void sendOpCodeMSG(int sock, short op_code);
