#include "my_linesh_lib.h"

int seq = 0;





int main(int argc, char const *argv[])
{
    int socketfd;
    char* msg_buffer[MAX_BUFFER_LENGTH];
    char* server_recv_response  = "Recived :)" ;
    char* stop_comunication_msg = "quit"; 
    struct sockaddr_in client,  server;
    socklen_t client_size;

    socketfd = socket(AF_INET, SOCK_RAW, 0); // Create Raw Socket

    // Server address configuration
    server.sin_addr.s_addr  = INADDR_ANY;
    server.sin_family       = AF_INET;
    server.sin_port         = htons(SERVER_PORT);

    // Binding port
    if(bind(socketfd, (struct sockaddr*) &server, sizeof(struct sockaddr)) == -1){
        close(socketfd);
        fail_errno("Binding procedure failed");
    }

    // 3-Way-Handshake procedure
    client_size = sizeof(struct sockaddr_in);

    if(linesh_3whs_server(socketfd, MAX_BUFFER_LENGTH, (struct sockaddr*) &client,  client_size) == 1)
        printf("Client sucessfully connect!");

    // Comunication Client - Server
    while(1)
    {
        memset(msg_buffer, 0, sizeof(msg_buffer));

        if(linesh_receive(socketfd, &msg_buffer, sizeof(msg_buffer), (struct sockaddr*)&client, client_size) <= 0){
            close(socketfd);
            fail("Something goes wrong with recieving the message. Server side");
        }

        printf("Client message recived:%s\n", msg_buffer);

        // Send a recived response to the client
        if(linesh_send(socketfd, server_recv_response, strlen(server_recv_response), (struct sockaddr*)&client, client_size) == -1) {
            close(socketfd);
            fail_errno("Sending server's response failed");
        }


        // Check if client wants stop the comunication
        if(strcmp(stop_comunication_msg,  msg_buffer) == 0){
            close(socketfd);
            printf("Comunication closed");
            break;
        }


    }


    return 0;
}
