

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

#include "common.h"

#define MENU_OP_HELLO 0
#define MENU_OP_LIST 1
#define MENU_OP_DOMAIN_RQ 2
#define MENU_OP_ADD_DOMAIN_IP 3
#define MENU_OP_ADD_DOMAIN_IPS 4
#define MENU_OP_CHANGE 5
#define MENU_OP_DELETE_IP 6
#define MENU_OP_DELETE_DOMAIN 7
#define MENU_OP_FINISH 8

#define DNS_TABLE_MAX_SIZE 2000
#define MAX_HOST_SIZE 50
#define MAX_ADDR_SIZE 16


/**
 * Function that sets the field addr->sin_addr.s_addr from a host name 
 * address.
 * @param addr struct where to set the address.
 * @param host the host name to be converted
 * @return -1 if there has been a problem during the conversion process.
 */
int setaddrbyname(struct sockaddr_in *addr, char *host);


/**
 * Function that sends a list request receives the list and displays it.
 * @param s The communications socket. 
 */
void process_list_operation(int s);

/** 
 * Function that process the menu option set by the user by calling 
 * the function related to the menu option.
 * @param s The communications socket
 * @param option the menu option specified by the user.
 */
void process_menu_option(int s, int option);
/**
 * Shows the menu options. 
 */
void printa_menu();

/**
 * Function that gets the dns_file name and port options from the program 
 * execution.
 * @param argc the number of execution parameters
 * @param argv the execution parameters
 * @param reference parameter to set the host name.
 * @param reference parameter to set the port. If no port is specified 
 * the DEFAULT_PORT is returned.
 */
int getProgramOptions(int argc, char* argv[], char *host, int *_port);


