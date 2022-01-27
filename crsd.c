#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "interface.h"

struct chatroom{
    int num_members;
    int slave;
    int port;
    int members[256];
    char name[256];
    pthread_t id;
};

int main(int argc, char const *argv[]){
    //get preliminary information such as port number or name
    int slave_socket;
    int master_socket;
    int rc;
    struct chatroom rooms[256];
    int num_rooms = 0;
    
    memset(rooms, 0, sizeof(struct chatroom) * 256);
    
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
    //getting a socket descriptor 
    slave_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(slave_socket < 0){
        perror("Create Socket Failed");
        return -1;
    }
    
    //associate port with socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((short)8652);
    
    rc = bind(slave_socket, (struct sockaddr*)&server, sizeof(server));
    if(rc < 0){
        perror("Binding Failed");
        return -1;
    }
    
    rc = listen(slave_socket, 10);
    if(rc < 0){
        perror("Listen Failed");
        return -1;
    }
}
