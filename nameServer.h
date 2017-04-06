/*
*  C Implementation: nameServer
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

#define MAX_QUEUED_CON 10 // Max number of connections queued
#define MAX_FILE_NAME_SIZE 50

/* Reads a line ended with \n from the file pointer.  */
/* Return: a line ended not with an EOL but with a 0 or NULL if the end of the
file is reached */
char *readLine(FILE *file, char *line, int sizeOfLine);


/**
 * Creates a DNSEntry variable from the content of a file line and links it 
 * to the DNSTable.
 * @param line the line from the file to be parsed
 * @param delim the character between tokens.
 */
struct _DNSEntry* buildADNSEntryFromALine(char *line, char *delim);

/* Reads a file with the dns information and loads into a _DNSTable structure.
Each line of the file is a DNS entry. 
RETURNS: the DNS table */
struct _DNSTable* loadDNSTableFromFile(char *fileName);

/*Dumps the dnstable into a byte array*/
/*@Return a pointer to the byte array representing the DNS table */
/*@param dnsTable the table to be serialized into an array of byes */
/*@param _tableSize reference parameter that will be filled with the table size*/
char *dnsTableToByteArray(struct _DNSTable* dnsTable, int *_tableSize);

/**
 * Converts the DNSEntry passed as a parameter into a byte array pointed by 
 * next_DNSEntry_ptr. The representation will be 
 * domain_name\0number_of_ips[4byte_ip]*]. 
 * @param dnsEntry the DNSEntry to be converted to a Byte Array.
 * @param next_DNSEntry_ptr a pointer to Byte Array where to start copying 
 * the DNSEntry. The pointer moves to the end of the ByteArray representation.
 */
void dnsEntryToByteArray(struct _DNSEntry* dnsEntry, char **p_entry_as_string);


/**
 * Calculates the number of bytes of the DNS table as a byte array format. 
 * It does not  include the message identifier. 
 * @param dnsTable a pointer to the DNSTable in memory.
 */
int getDNSTableSize(struct _DNSTable* dnsTable);


/**
 * Function that gets the dns_file name and port options from the program 
 * execution.
 * @param argc the number of execution parameters
 * @param argv the execution parameters
 * @param reference parameter to set the dns_file name.
 * @param reference parameter to set the port. If no port is specified 
 * the DEFAULT_PORT is returned.
 */
int getProgramOptions(int argc, char* argv[], char *dns_file, int *_port);


