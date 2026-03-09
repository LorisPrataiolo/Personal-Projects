#include "encripted_protocol_and_sniffer.h"


int main()
{
    int sockfd, comunication_sockfd, portno = 8080, backlog_connections = 5, bytes_written;
    struct sockaddr_in server_addr,  client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[256];
    int option = 1;


    // Socket Initializiation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
        fail_errno("Creation socket failed");
    printf("Socket initialized\n");
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    // Server address structure initialization
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
    server_addr.sin_port   = htons(portno);
        
    // Bind the server port
    if (bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1 )
        fail_errno("Binding socket failed");
    printf("Port binded correctly\n");

    // Listen for a Client interaction
    if(listen(sockfd, backlog_connections) == -1)
        fail_errno("Listen process has failed");
    printf("Server is listening\n");

    // Accept comunication
    comunication_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_len);
    if( comunication_sockfd == -1)
        fail_errno("Connection refused");
    
    printf("Connection accepted!\n");
    
    while (1)
    {
        bzero(buffer,256); // reset the buffer
        if(read(comunication_sockfd, buffer, 256) == -1)
            fail_errno("Error server's request reading:");

        printf("Client request:%s\n", buffer);
               
        
        bzero(buffer,256);
        strcpy(buffer, "Ok");

        bytes_written = write(comunication_sockfd, buffer, strlen(buffer));
        if (bytes_written < 0)
            fail_errno("Sending massage failed:");
        
    

        // escape
        if (bcmp(buffer, "quit", 4))
            break;

    }

    printf("Comunication ended");


    // Run Client-server comunication
    return 0;
}
