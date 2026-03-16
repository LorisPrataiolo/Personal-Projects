#include "my_linesh_lib.h"


int main()
{
    int socketfd;
    char msg_buffer[MAX_BUFFER_LENGTH];
    struct sockaddr_in  server_addr;
    socklen_t server_len;
    uint16_t client_seq, server_seq;


    socketfd = socket(AF_INET, SOCK_RAW, 150);
    if (socketfd < 0) 
    {
        fail_errno("Socket creation failed (Did you forget sudo?)");
    }

    // Server addres configuration
    memset(&server_addr, 0, sizeof(server_addr));
    inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);

    server_len = sizeof(struct sockaddr_in);

    printf("Start Linesh Protocol: \n Client submit request connection to the server.");

    if(linesh_3whs_client(socketfd, MAX_BUFFER_LENGTH, (struct sockaddr*) &server_addr, &server_len, &server_seq, &client_seq) == 1)
    {
        printf("Server succesfully connect!\n");
    }

    printf("Comunication started!\n");
// Communication loop: Client <-> Server
    while (1) 
    {
        memset(msg_buffer, 0, MAX_BUFFER_LENGTH);

        // Get input from the user
        printf("Enter message for server (type 'quit' to exit): ");
        if (fgets(msg_buffer, MAX_BUFFER_LENGTH, stdin) == NULL) {
            break; // Exit if there's an input error
        }

        msg_buffer[strcspn(msg_buffer, "\n")] = 0;  // strip user input

        int payload_length = strlen(msg_buffer);

        // 2. Wrap and send the message using your custom protocol
        int payload_sent = linesh_send(socketfd, msg_buffer, payload_length, (struct sockaddr*)&server_addr, server_len, client_seq, server_seq);
        
        if (payload_sent == -1) {
            close(socketfd);
            fail_errno("Sending message to server failed");
        }

        // Advance the client's sequence number by the amount of data we just sent!
        client_seq += payload_length;

        // Check if the user has sent the quit command
        if (strcmp("quit", msg_buffer) == 0) {
            printf("Communication closed by client.\n");
            break;
        }

        // Wait for the server's acknowledgment
        memset(msg_buffer, 0, MAX_BUFFER_LENGTH); // Clear buffer for incoming data
        
        int payload_received = linesh_receive(socketfd, msg_buffer, MAX_BUFFER_LENGTH, (struct sockaddr*)&server_addr, &server_len);
        
        if (payload_received <= 0) {
            close(socketfd);
            fail("Something went wrong receiving the server's reply.");
        }

        // Advance the server's sequence number by the amount of data it sent us!
        server_seq += payload_received;

        // Ensure it's null-terminated so we can print it safely
        msg_buffer[payload_received] = '\0';
        printf("Server replied: %s\n\n", msg_buffer);
    }



    return 0;
}