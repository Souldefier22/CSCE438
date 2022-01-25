#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inferface.h"

struct chatroom{
    int members = 0;
    int slave;
    int port;
    char name[];
    pthread_t id;
}

int main(int argc, char const *argv[]){
    int server;
    int slave_socket;
    int master_socket;
    
    
}
