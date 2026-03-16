#include "my_linesh_lib.h"


int main()
{
    int socketfd;
    char msg_buffer[MAX_BUFFER_LENGTH];
    char* server_recv_response  = "Recived :)" ;
    char* stop_comunication_msg = "quit"; 
    struct sockaddr_in client,  server;
    socklen_t client_size;
    uint16_t server_seq, client_seq;


    socketfd = socket(AF_INET, SOCK_RAW, 150); // Create Raw Socket
    if (socketfd < 0) 
    {
        fail_errno("Socket creation failed (Did you forget sudo?)");
    }
    // Server address configuration
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr  = INADDR_ANY;
    server.sin_family       = AF_INET;
    server.sin_port         = htons(SERVER_PORT);

    // Binding port
    if(bind(socketfd, (struct sockaddr*) &server, sizeof(struct sockaddr)) == -1){
        close(socketfd);
        fail_errno("Binding procedure failed");
    }
    printf("Server Port: sucessfully binded\n");

    // 3-Way-Handshake procedure
    client_size = sizeof(struct sockaddr_in);

    printf("Start Listening for a client connection...\n");
    if(linesh_3whs_server(socketfd, MAX_BUFFER_LENGTH, (struct sockaddr*) &client,  &client_size, &server_seq, &client_seq) == 1)
        printf("Client sucessfully connect!\n");

    // Communication Client - Server
    printf("Comunication started!\n");
    while(1)
    {
        memset(msg_buffer, 0, sizeof(msg_buffer));

        int payload_received = linesh_receive(socketfd, msg_buffer, sizeof(msg_buffer), (struct sockaddr*)&client, &client_size);
        
        if(payload_received <= 0){
            close(socketfd);
            fail("Something went wrong with receiving the message. Server side");
        }

        // Advance client's sequence number by the amount of data we received
        client_seq += payload_received;

        printf("Client message received: %s\n", msg_buffer);

        // Check if client wants to stop the communication
        if(strcmp(stop_comunication_msg, msg_buffer) == 0){
            close(socketfd);
            printf("Communication closed\n");
            break;
        }

        // Send a received response to the client
        int payload_sent = linesh_send(socketfd, server_recv_response, strlen(server_recv_response), (struct sockaddr*)&client, client_size, server_seq, client_seq);
        
        if(payload_sent == -1) {
            close(socketfd);
            fail_errno("Sending server's response failed");
        }

        // Advance server's sequence number by the amount of data we sent
        server_seq += strlen(server_recv_response);
    }


    return 0;
}
