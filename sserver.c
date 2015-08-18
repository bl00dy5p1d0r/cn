#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#define MYPORT "3490"
#define BACKLOG 10
#define P_ERROR(msg) \
    perror(msg); \
    exit(1);
#define P_STATUS(msg) \
    write(1, msg, sizeof(msg)-1);
static unsigned int client_cnt = 0;
static unsigned int client_fds[BACKLOG];
int main(void)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, MYPORT, &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0) {
        P_ERROR("Couldn't open socket")
    }
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        P_ERROR("Couldn't bind address")
    }
    listen(sockfd, BACKLOG);

    addr_size = sizeof their_addr;
    while(1) {
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        client_fds[client_cnt++] = new_fd;
        if ( new_fd < 0 ) {
             P_ERROR("Couldn't accept connection!")
        }
        int pid;
        if ( (pid = fork()) == -1 ) {
             close(new_fd);
             P_ERROR("Fork Failed!")
        }
        else if ( pid > 0 ) {
            close(new_fd);
            P_STATUS("#! Parrent Closed FD")
            continue;
        }
        else if ( pid == 0 ) {
             char buffer[1024] = {'\0'};
             while(1) {
                 int recv_bytes = 0,i;
                 if ((recv_bytes = recv(new_fd, buffer, 1023, 0)) == -1) {
                    P_ERROR("recv Failed !")
                 }
                 for(i=0; i<client_cnt; i++) {
                     int to_fd = client_fds[i];
                     if(to_fd != new_fd) {
                         int sent_len = send(to_fd, buffer, strlen(buffer), 0);
                         if(sent_len < 0)
                             P_ERROR("Sending Failed")
                    }
                 }
                 memset(&buffer, 0, 1024);
             }
        }
    }
}
