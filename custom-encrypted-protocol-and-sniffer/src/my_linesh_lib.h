#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

/* Load the entire segment into buffer_to_store_sgmnt, and return the number of bytes recived.
Fail if the number of bytes is 0 or -1 (for more information look recvfrom method).*/
int get_tcp_segment(int socketfd, char* buffer_to_store_sgmnt,  int buflen,  struct  sockaddr* cli_addr, socklen_t *  clilen){

    int total_bytes_recived = recvfrom(socketfd, buffer_to_store_sgmnt, buflen, 0, cli_addr, clilen );
    
    if (total_bytes_recived == -1){
        fail_errno("Recieving process failed");
    }   
    else if (total_bytes_recived == 0)
    {
        fail("Connection may be close. Recieve process failed");
    }

    return total_bytes_recived;
}

/* Extract from the custom TCP segment the actual payload. Load it into `buf_payload` and return the `payload_size`.
Fail if payload size is less or equal to zero or if its greater than length of the buffer. */
int get_payload_of_tcp_segment(char *buffer_segment, int buflen, char* buf_payload, int total_bytes_recived){
    int ip_header_sz;
    struct ip* ip_header;

    ip_header = (struct ip*) buffer_segment;
    ip_header_sz = ip_header->ip_hl *4;
    int payload_size = total_bytes_recived - ip_header_sz - sizeof(struct Linesh_TCP);

    if (payload_size <= 0)
        fail("Error during receiving message. Length is less or equal to zero");
    else if (payload_size > buflen)
    {
        fail("The message is too long. The server has refued it");
    }
    
    char* payload = buffer_segment + ip_header_sz + sizeof(struct  Linesh_TCP);
    memcpy(buf_payload, payload, payload_size);

    return payload_size;
}

/* Receive and extract from the TCP segment the Payload.
- Return:  `payload_size`*/
int linesh_receive(int socketfd, char*buffer_to_store_msg, int buflen, struct sockaddr* cli_addr, socklen_t * clilen )
{
    char buffer_temp[buflen];
    int total_bytes_recived = get_tcp_segment(socketfd, buffer_temp, buflen, cli_addr, clilen );

    int payload_size = get_payload_of_tcp_segment(buffer_temp, buflen, buffer_to_store_msg, total_bytes_recived);
    
    return payload_size;
}

