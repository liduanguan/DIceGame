// client.c
// Author: Liduan Guan
//	   Leiwen Ma

#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define PORT 19999
#define ADDRESS "137.207.82.51"

int main(int argc, char*argv[]) {
    
    int portNumber;

    // some message
    static const char *PLAY_MSG = "You can now play\n";
    static const char *WIN_MSG = "Game over: you won the game\n";
    static const char *LOST_MSG = "Game over: you lost the game\n";
  
    if(argc != 3){
	printf("Call model: %s <IP> <Port #>\n", argv[0]);
	exit(0);
    }

    // get socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // connect to server
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNumber);
    server_addr.sin_port = htons((uint16_t)portNumber);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) <0){
        printf("Connect failed: ");
        perror("");
        exit(-1);
    }

    int dice;
    char msg[100];
    int total = 0;
    // Use current time as seed for random generator
    srand(time(0));
    while (1) {
        memset(msg, 0, sizeof(msg));
        // read message from server
        read(socket_fd, msg, 100);
        // if is play message
        if (strcmp(msg, PLAY_MSG) == 0) {
            // get dice by random number
            dice = rand() % 10 + 1;
	    total += dice;
            // print it
            printf("got %d points\n", dice);
	    printf("got %d total points\n", total);
            write(socket_fd, &dice, sizeof(dice));
        } else {
            // if receive win message
            if (strcmp(msg, WIN_MSG) == 0) {
                printf("I won the game\n");
            } else if (strcmp(msg, LOST_MSG) == 0) { // if receive lost message
                printf("I lost the game\n");
            }
            break;
        }
        // slow down the execution
        sleep(3);
    }
    close(socket_fd);
    return 0;
}
