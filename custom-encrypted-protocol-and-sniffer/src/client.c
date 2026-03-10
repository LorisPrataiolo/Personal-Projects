#include "encripted_protocol_and_sniffer.h"

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

/// PORTS FOR COMMUNICATION (7777 -> CLIENT) AND (8080 -> SERVER)
int CLIENT_PORT = 7777;
int SERVER_PORT = 8080;

int linesh_connect(int sfd,struct sockaddr* addr,ssize_t sz)
{
	struct Linesh_TCP *request;
    int sz1;

    // while the connection is not accepted
	while(1)
	{
        // prepare and send the request of connection to server through coustom tcp protocol
		request = (struct Linesh_TCP*)malloc(sizeof(struct Linesh_TCP));
		memset(request,0,sizeof(request));
		request->source = CLIENT_PORT;
		request->dest = SERVER_PORT;
		request->connect_req = 1;
		request->seq = 5;
		request->rec_seq = 0;
		request->final_acknow = 0;

		sendto(sfd,request,sizeof(request),0,addr,sz);
		char buffer[1000];

        // check the ack from server
		if(recvfrom(sfd,buffer,1000,0,addr,&sz1)>=0){
			printf("Conection was accepted by the server\n");
        }


		struct iphdr* ip;
		ip = (struct iphdr*)buffer;

		request = (struct Linesh_TCP*)(buffer+(ip->ihl*4));
        printf("Connection Accepted Flag : %d\n",request->connect_acc);

        // check if the connection is accepted
		if(request->connect_acc==1)
			break;
	}

    // send final ACK to server
	printf("Continue \n");
	request = (struct Linesh_TCP*)malloc(sizeof(struct Linesh_TCP));
	memset(request,0,sizeof(request));
	request->source = CLIENT_PORT;
	request->dest = SERVER_PORT;
	request->connect_req = 0;
	request->seq = 5;
	request->rec_seq = 0;
	request->final_acknow = 1;
	if(sendto(sfd,request,sizeof(request),0,addr,sz1)>=0)
    {
		printf("Sent the Final acknowledge Flag\n");
    }
	return 1;
}

int custom_msg(int sfd,char *buffer,int bufsiz,struct sockaddr* addr,ssize_t sz)
{
	char msg[1000];
	struct Linesh_TCP *buff;
	buff = (struct Linesh_TCP*)(msg);
	memset(buff,0,sizeof(buff));

    //Defining the fields
	buff->source = CLIENT_PORT;
	buff->dest = SERVER_PORT;
	buff->connect_req = 0;
	buff->connect_acc = 0;
	buff->seq = 0;
	buff->rec_seq = 0;
	buff->final_acknow =0;
	buff->data_flg = 1;
	memcpy(msg+sizeof(*buff),buffer,bufsiz);
	sendto(sfd,msg,1000,0,addr,sz);
}


int main()
{
    // initialization: client and server socket addresses
    int sfd = socket(AF_INET,SOCK_RAW,253);
	struct sockaddr_in client_addr;
	memset(&client_addr,0,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.2");


	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.7");


	bind(sfd,(struct sockaddr*)&client_addr,sizeof(client_addr)); // bind the client port
	linesh_connect(sfd,(struct sockaddr*)&server_addr,sizeof(server_addr)); 

    // send message to server
	char buffer[100] = "Linesh 21CSB0A35";
	custom_msg(sfd,buffer,strlen(buffer),(struct sockaddr*)&server_addr,sizeof(server_addr));
	return 0;

}