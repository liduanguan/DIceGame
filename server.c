// server.c
// Author: Liduan Guan
// 	   Leiwen Ma


#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PORT 19999

// wait for child process
void sigchld_handler(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void servicePlayers(int toto_fd, int titi_fd){
    // some message
    static const char *PLAY_MSG = "You can now play\n";
    static const char *WIN_MSG ="Game over: you won the game\n";
    static const char *LOST_MSG ="Game over: you lost the game\n";
    const int PLAY_MSG_LEN = strlen(PLAY_MSG);
    const int WIN_MSG_LEN = strlen(WIN_MSG);
    const int LOST_MGS_LEN = strlen(LOST_MSG);

    // current score
    int score;
    // total score for toto and titi
    int TOTO_sum = 0, TITI_sum = 0;
    while (1){
        // send play msg
        write(toto_fd, PLAY_MSG, PLAY_MSG_LEN);
        // read score
        read(toto_fd, &score, sizeof(score));
        // add toto total score
        TOTO_sum += score;

        // send play msg
        write(titi_fd, PLAY_MSG, PLAY_MSG_LEN);
        // read score
        read(titi_fd, &score, sizeof(score));
        // add score
        TITI_sum += score;

        // gameover
        if (TOTO_sum >= 100 || TITI_sum >= 100) {
            // toto win
            if (TOTO_sum >= 100) {
                write(toto_fd, WIN_MSG,WIN_MSG_LEN);
                write(titi_fd, LOST_MSG, LOST_MGS_LEN);
		printf("Game Over. TOTO win\n");
            } else{ // titi win
                write(toto_fd, LOST_MSG, LOST_MGS_LEN);
                write(titi_fd, WIN_MSG, WIN_MSG_LEN);
		printf("Game Over. TITI win\n");
            }
            break;
        }
    }
    close(toto_fd);
    close(titi_fd);
}

int main(int argc, char *argv[]){
    
    int portNumber;	

    if(argc != 2){
	printf("Call model: %s <Port #>\n", argv[0]);
	exit(0);
    }	    
	
    // set signal handler
    signal(SIGCHLD, sigchld_handler);

    // get server socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    // create and init sock addr
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));

    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    my_addr.sin_port = htons((uint16_t)portNumber);
    my_addr.sin_family = AF_INET;

    // bind
    bind(listen_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    // set reuse port
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&opt, sizeof(opt));

    // listen for client
    listen(listen_fd, 10);

    while(1){
        // wait for tow client
        int client_fd_1 = accept(listen_fd, NULL, NULL);
	printf("One client connect successful. Waiting for another client...\n");
        int client_fd_2 = accept(listen_fd, NULL, NULL);
	printf("Two client connected. Game start\n");
        if (fork() == 0){ // child process
            servicePlayers(client_fd_1, client_fd_2);
            break;
        }
    }

    close(listen_fd);
    return 0;
}
