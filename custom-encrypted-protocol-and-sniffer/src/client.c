#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>



void fail_errno(const char * const  );
void fail (const char * const  );
void check_main_arg (int , char  *[]);
int parse_ipv4addr(char *, struct in_addr *);
int parse_port(const char *);


/* ================================================================== */

int main(int argc, char  *argv[]) {

    struct in_addr server_addr;
    struct sockaddr_in server;


    // Sanity check of program arguments
    check_main_arg(argc, argv);

    if (parse_ipv4addr(argv[1], &server_addr))
        printf ("Server IP address: valid\n");

    int port =  parse_port(argv[2]);
    printf("Port: valid\n");


    // Create Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
        fail_errno("Creation socket failed");

    
    // Converting server port to network format
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr   = server_addr;
    server.sin_port   = htons(port);
    
    // Start Client-Server connection
    int status_connection = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    if(status_connection == -1)
        fail_errno("Server connection failed");
    else
        printf("Client connected!\n (status:%d) \n", status_connection);
    
    
    // Run client

return 0;
}



/* ================================================================== */
void fail_errno(const char * const msg ) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void fail(const char * const msg) {
    fprintf(stderr, "%s\n",msg);
    exit(EXIT_FAILURE);
}

void check_main_arg (int argc, char  *argv[]){
    if (argc != 3) 
        fail("Usage: ./client <IPv4 address> <number port>");
}

int parse_ipv4addr(char*ip, struct in_addr *addr){
    int res = inet_pton(AF_INET, ip, addr);

    if (res == 0) 
        fail("Invalid IPv4 address");
    
    if (res ==  - 1)
        fail_errno("Invalid IPv4 address");

  return res;
}

int parse_port(const char *port_str)
{
    char *endptr;
    errno = 0;

    long port = strtol(port_str, &endptr, 10);

    if (errno != 0)
        fail_errno("strtol");

    if (*endptr != '\0')
        fail("Port must be numeric");

    if (port < 1 || port > 65535)
        fail("Port must be between 1 and 65535");

    return (int)port;
}

