#define DEFAULT_SOURCE // this force to use BSD structure so that we can use ip structure instaed iphr 
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>   
#include <netinet/ip.h>
#include "my_fail_lib.h"

# define MAX_BUFFER_LENGTH 15000 // bytes

struct Linesh_TCP {
    uint16_t source;
    uint16_t dest;
    uint8_t  connect_req;
    uint8_t  connect_acc;
    uint8_t  final_acknow;
    uint8_t  data_flg;
    uint16_t seq;
    uint16_t rec_seq;
};

int CLIENT_PORT = 7777;
int SERVER_PORT = 8080;
char SERVER_ADDRESS[10]= "127.0.0.1";

/* Load the entire segment into buffer_to_store_sgmnt, and return the number of bytes recived.
Fail if the number of bytes is 0 or -1 (for more information look recvfrom method).*/
int get_tcp_packet(int socketfd, 
    char* buffer_to_store_sgmnt,  
    int buflen,  
    struct  sockaddr* source_addr, 
    socklen_t *  source_len)
{

    int total_bytes_recived = recvfrom(socketfd, buffer_to_store_sgmnt, buflen, 0, source_addr, source_len );
    
    if (total_bytes_recived == -1){
        close(socketfd);
        fail_errno("Recieving process failed:");
    }   
    else if (total_bytes_recived == 0)
    {
        close(socketfd);
        fail("Connection may be close. Recieve process failed");
    }

    return total_bytes_recived;
}



/* Extract from the custom TCP segment the actual payload. Load it into `buf_payload` and return the `payload_size`.
Fail if payload size is less or equal to zero or if its greater than length of the buffer. */
int get_payload_of_tcp_packet(char *buffer_segment, int buflen, char* buf_payload, int total_bytes_recived)
{
    int ip_offset;
    struct ip* ip_header;

    ip_header = (struct ip*) buffer_segment;
    ip_offset = ip_header->ip_hl *4;
    int payload_size = total_bytes_recived - ip_offset - sizeof(struct Linesh_TCP);

    if (payload_size <= 0)
        fail("Error during receiving message. Length is less or equal to zero");
    else if (payload_size > buflen)
    {
        fail("The message is too long. The server has refued it");
    }
    
    char* payload = buffer_segment + ip_offset + sizeof(struct  Linesh_TCP);
    memcpy(buf_payload, payload, payload_size);

    return payload_size;
}



/* Receive and extract from the TCP segment the Payload.
- Return:  `payload_size`*/
int linesh_receive(int socketfd, 
    char*buffer_to_store_msg, 
    int buflen, 
    struct sockaddr* cli_addr, 
    socklen_t * clilen )
{
    char buffer_temp[buflen];
    int total_bytes_recived = get_tcp_packet(socketfd, buffer_temp, buflen, cli_addr, clilen );

    int payload_size = get_payload_of_tcp_packet(buffer_temp, buflen, buffer_to_store_msg, total_bytes_recived);
    
    return payload_size;
}



/* Method that prepare the segments' linesh header. */
void prepare_linesh_header(struct Linesh_TCP* linesh_reply,
    uint16_t source_port, 
    uint16_t dest_port,
    uint8_t  connect_req,
    uint8_t  connect_acc,
    uint8_t  final_acknow,
    uint8_t  data_flg,
    uint16_t seq,
    uint16_t rec_seq)
{
    linesh_reply->source       = source_port;
    linesh_reply->dest         = dest_port;
    linesh_reply->connect_req  = connect_req;
    linesh_reply->connect_acc  = connect_acc;
    linesh_reply->final_acknow = final_acknow;
    linesh_reply->data_flg     = data_flg;
    linesh_reply->seq          = seq;
    linesh_reply->rec_seq      = rec_seq;
}

int check_seq(const uint16_t rec_seq, const uint16_t expected_seq)
{
    if(rec_seq != expected_seq){
        return -1;
    }
return 1;
}

void show_sequences(struct Linesh_TCP packet)
{
    printf("\nPACKET SEQUENCES  ----------------- +\n");
    printf("recived: %d \n", packet.rec_seq);
    printf("send: %d \n", packet.seq);
    printf("-----------------------------------\n\n");
}

/* Prints the complete state of a Linesh_TCP header for debugging */
static inline void debug_packet(const char* step_name, struct Linesh_TCP *packet, uint16_t expected_seq, uint16_t expected_rec_seq) 
{
    printf("\n[DEBUG] --- %s ---\n", step_name);
    
    // 1. Print the flags so you know what type of packet arrived
    printf("Flags -> SYN: %d | SYN-ACK: %d | FIN-ACK: %d | DATA: %d\n", 
           packet->connect_req, packet->connect_acc, packet->final_acknow, packet->data_flg);
    
    // 2. Print the Sequence Number vs what we expected
    printf("SEQ   -> Received: %-5u | Expected: %-5u | %s\n", 
           packet->seq, expected_seq, 
           (packet->seq == expected_seq) ? "✅ MATCH" : "❌ FAIL");
           
    // 3. Print the Acknowledgment Number vs what we expected
    printf("ACK   -> Received: %-5u | Expected: %-5u | %s\n", 
           packet->rec_seq, expected_rec_seq, 
           (packet->rec_seq == expected_rec_seq) ? "✅ MATCH" : "❌ FAIL");
           
    printf("---------------------------------------\n\n");
}

/* This method process the 3-way-handshake for linesh tcp protocol. Returns 1 in case of success otherwise faild automatically.
An other feature of this method is that stores the last client and server sequences in to input variables */
int linesh_3whs_server(int socketfd, 
    int buff_segment_length, 
    struct sockaddr* cli_addr, 
    socklen_t* clilen, 
    uint16_t* server_seq, 
    uint16_t* cli_seq)
{
    struct ip* ip_header;
    struct Linesh_TCP * incoming_request;
    struct Linesh_TCP outgoing_reply;
    char recieve_buffer[buff_segment_length];

    int segment_size = get_tcp_packet(socketfd,  recieve_buffer, buff_segment_length, cli_addr, clilen);

    // Get the linesh header
    ip_header = (struct ip*) recieve_buffer;
    int ip_offset = ip_header->ip_hl *4;
    incoming_request = (struct Linesh_TCP*)(recieve_buffer + ip_offset);

    // Check the incoming sequences
    debug_packet("SERVER RECIVED CONNECTION REQUEST", incoming_request, incoming_request->seq, incoming_request->rec_seq);
    show_sequences(*incoming_request);
    if (check_seq(incoming_request->rec_seq, 0) == -1)
    {
        fail("Incompatible sequence recived");
    }
    

    // get sequence from cli
    *cli_seq =  incoming_request->seq + 1;
    *server_seq = (uint16_t)rand();

    // Accept the connection with client -> send ACK to the client -----------------
    if(incoming_request->connect_req == 1){
        prepare_linesh_header(&outgoing_reply, SERVER_PORT, CLIENT_PORT, 0, 1, 0, 0, *server_seq, *cli_seq);
        debug_packet("SEND ACCEPT PACKAGE", &outgoing_reply, *server_seq, incoming_request->seq + 1);

        if (sendto(socketfd, &outgoing_reply, sizeof(struct Linesh_TCP),  0, cli_addr, *clilen) == -1){
            close(socketfd);
            fail_errno("Reply submission faled:");
        }
    }

    
    // Recieve the final ACK -------------------------------------------------------
    memset(recieve_buffer,0, sizeof(struct Linesh_TCP)); // clear the recieve buffer

    segment_size = get_tcp_packet(socketfd,  recieve_buffer, buff_segment_length, cli_addr, clilen);

    ip_header = (struct ip*) recieve_buffer;
    ip_offset = ip_header->ip_hl *4;
    incoming_request = (struct Linesh_TCP*)(recieve_buffer + ip_offset);
    (*server_seq)++;

    // check if the sequences corresponds
    debug_packet("SERVER: Verifying Final ACK", incoming_request, *cli_seq, *server_seq);
    if (check_seq(incoming_request->seq, *cli_seq) == -1 ||  check_seq(incoming_request->rec_seq, *server_seq) == -1){
        fail("Sequences do not corresponds");
    }
    
    // check the final ack
    if(incoming_request->final_acknow != 1){
        fprintf(stderr, "Final ACK: %d\n", incoming_request->final_acknow);
        fail("Error with Final ACK");
    }

return 1;
}





/* This method send a package/segment that includes a payload of chars.*/
int linesh_send(int socketfd, 
    const char *msg_payload, 
    int payload_length, 
    struct sockaddr* dest_addr, 
    socklen_t dest_len, 
    uint16_t current_seq, 
    uint16_t current_rec_seq)
{
    int total_packet_size = sizeof(struct Linesh_TCP) + payload_length;
    struct Linesh_TCP outgoing_header;

    char packet_buffer[total_packet_size];
    memset(packet_buffer, 0 ,total_packet_size);

    prepare_linesh_header(&outgoing_header, SERVER_PORT, CLIENT_PORT, 0, 0, 0, 1, current_seq, current_rec_seq);
    memcpy(packet_buffer, &outgoing_header, sizeof(struct Linesh_TCP)); // put the header at the buffer beginning
    memcpy(packet_buffer + sizeof(struct Linesh_TCP), msg_payload, payload_length); // put the payload immediatly after the header

    if (sendto(socketfd, packet_buffer, total_packet_size,  0, dest_addr, dest_len) == -1){
        close(socketfd);
        fail_errno("Reply submission faled:");
    }

return 1;
}


int linesh_3whs_client(int socketfd, 
int buff_len,
struct sockaddr*  server_addr,
socklen_t* server_len,
uint16_t* server_seq,
uint16_t *client_seq
)
{
    struct ip* ip_header;
    struct Linesh_TCP outgoing_packet;
    struct Linesh_TCP *incoming_packet;
    char buff_temp[buff_len];


    // Send connection recquest to the server
    *client_seq = (uint16_t) rand();
    *server_seq = 0; 

    prepare_linesh_header(&outgoing_packet, CLIENT_PORT, SERVER_PORT, 1, 0, 0, 0, *client_seq, *server_seq);
    
    printf("Send connection request\n");
    if(sendto(socketfd, &outgoing_packet, sizeof(struct Linesh_TCP),  0,  server_addr, *server_len) == -1)
    {
        
        fail_errno("Submition request to server has failed");
    }
    
    //  RECIEVE & ANALYZE REQUEST ------------------------
    printf("Server response recived \n");
    int packet_size = get_tcp_packet(socketfd, buff_temp, buff_len, server_addr, server_len);
    
    // Get the header
    ip_header = (struct ip*) buff_temp;
    int ip_offset =  ip_header->ip_hl*4;
    incoming_packet = (struct Linesh_TCP*)(buff_temp + ip_offset);

    // Check if server has accepted the connection
    debug_packet("SERVER RESPONSE:", incoming_packet, incoming_packet->seq, *client_seq + 1);
    if(check_seq(incoming_packet->rec_seq, *client_seq+1) != 1)
    {
        fail("Inconsistent sequence: Packet refused. \n Connectio closed.");
    }

    if (incoming_packet->connect_acc != 1)
    {
        fail("Connection request refused");
    }
    

    // SEND FINAL SYN-ACK -----------------------------------
    printf("Send final ACK\n");
    (*client_seq)++; // update sequence
    *server_seq = incoming_packet->seq + 1;
    
    prepare_linesh_header(&outgoing_packet, CLIENT_PORT, SERVER_PORT, 0, 0, 1, 0, *client_seq,  *server_seq );

    show_sequences(outgoing_packet);
    if (sendto(socketfd, &outgoing_packet,  sizeof(struct Linesh_TCP), 0, server_addr, *server_len ) == -1)
    {
        fail_errno("Submit the final ACK to the server has failed.");
    }

return 1;  
}
