#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>




void fail_errno(const char * const msg ) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void fail(const char * const msg) {
    fprintf(stderr, "%s\n",msg);
    exit(EXIT_FAILURE);
}



void check_main_arg (int , char  *[]);

int parse_ipv4addr(char*ip, struct in_addr *addr){
    int res = inet_pton(AF_INET, ip, addr);

    if (res == 0) 
        fail("Invalid IPv4 address");
    
    if (res ==  - 1)
        fail_errno("Invalid IPv4 address");

  return res;
}

int parse_port(const char *);

