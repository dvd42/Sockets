/*
*  C Implementation: common.h
*
* Description: 
*
*
* Author: MCarmen de Toro <mc@mc>, (C) 2015
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "common.h"


/**
 * Prints the message representing DNS table in a user readable mode. 
 * Prints for each domain name its list of IP's. Each domain is printed in 
 * one line.
 * @param dns_table the array of bytes representing the DNS table without the
 * operation code
 * @param table_size the size of the DNS table
 */
void printDNSTableFromAnArrayOfBytes(char *dns_table, int table_size)
{
  int offset = 0;
  int i = 0;
  char* dns_table_ptr = dns_table;
  int numberOfIps;
  struct in_addr address;
  
  while (offset < table_size)
  {
    printf("%s ", dns_table_ptr);
    offset += strlen(dns_table_ptr);
    offset += 1; //\0
    dns_table_ptr = dns_table + offset;
    numberOfIps = ldshort(dns_table_ptr);
    offset += sizeof(short);
    dns_table_ptr = dns_table + offset;
    //printing ips
    for(i=0; i< numberOfIps; i++)
    {
       address.s_addr = ldaddr(dns_table_ptr);
       printf("%s ", inet_ntoa(address));
       offset += sizeof(in_addr_t);
       dns_table_ptr = dns_table + offset;
    }
    printf("\n");
    fflush(stdout);
  }
  
}

/**
 * Prints the dns table structure to the stdout.
 * Use for debug purpose
 */
void printDNSTable(struct _DNSTable* dns_table)
{
  struct _DNSEntry *dns_entry;
  struct _IP *ip_structure;
 
  printf("\n");
  if ((dns_entry = dns_table->first_DNSentry) != NULL)
  {
    do
    {
      printf("%s ", dns_entry->domainName);
      if((ip_structure = dns_entry->first_ip)!=NULL)
      {
        do
        {          
          printf("%s ", inet_ntoa(ip_structure->IP));
          ip_structure = ip_structure->nextIP;
        }while(ip_structure!=NULL);        
      }      
      dns_entry = dns_entry->nextDNSEntry;       
      printf("\n");
    }while(dns_entry != NULL);
  }
}

/**
 * Function that sends an operation code as an array of bytes through 
 * a socket
 * @param sock the socket to use to the communication
 * @param the operation code to be sent as an array of bytes
 */
void sendOpCodeMSG(int sock, short op_code)
{

    int n=0;
    char buffer[sizeof(short)];
    stshort(op_code, buffer);

    n = send(sock, buffer, sizeof(buffer),0);
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }        

}


