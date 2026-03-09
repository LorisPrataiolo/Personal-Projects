#include "encripted_protocol_and_sniffer.h"



int main(int argc, char  *argv[]) {

    struct in_addr server_addr;
    struct sockaddr_in server;
    char buffer [256];
    int bytes_written;


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
    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1)
        fail_errno("Server connection failed");

    printf("Client connected!\n");
    
    
    // Run client
    while (1)
    {
        printf("Write the message do you want to send\n>>>");
        bzero(buffer,256);
        scanf("%s",buffer);

        bytes_written = write(sockfd, buffer, strlen(buffer));
        if (bytes_written < 0)
            fail_errno("Sending massage failed:");
        
        
        bzero(buffer,256); // reset the buffer
        if(read(sockfd, buffer, 256) == -1)
            fail_errno("Error server's response reading:");

        printf("Server response:%s\n", buffer);

        // escape
        if (bcmp(buffer, "quit", 4) == 0)
            break;

    }

    printf("Comunnication stopped");
    

return 0;
}



/* ================================================================== */

void check_main_arg (int argc, char  *argv[]){
    if (argc != 3) 
        fail("Usage: ./client <IPv4 address> <number port>");
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

