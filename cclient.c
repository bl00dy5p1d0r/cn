#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#define SERV_PORT "3490"
#define SERV_ADDR "127.0.0.1"

#define P_ERROR(msg) \
    perror(msg); \
    exit(1);
#define P_STATUS(msg) \
    write(1, msg, sizeof(msg)-1);

int main(void)
{
    struct addrinfo serv, *res;
    int sockfd;

    memset(&serv, 0, sizeof(serv));
    serv.ai_family = AF_UNSPEC;
    serv.ai_socktype = SOCK_STREAM;
    getaddrinfo(SERV_ADDR, SERV_PORT, &serv, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0) {
        P_ERROR("Couldn't open socket")
    }

    connect(sockfd, res->ai_addr, res->ai_addrlen);
    while(1) {
        char send_buffer[1024] = {'\0'};
        char recv_buffer[1024] = {'\0'};
        printf("\n> ");
        fgets(send_buffer, 1024, stdin);
        int sent_len = send(sockfd, send_buffer, strlen(send_buffer), 0);
        if(sent_len < 0) {
            P_ERROR("Sending Failed")
        }
        memset(&send_buffer, 0, 1024);
        int recv_bytes = 0;
        if ((recv_bytes = recv(sockfd, recv_buffer, 1023, 0)) == -1) {
            P_ERROR("recv Failed !")
        }
        printf("#> %s", recv_buffer);
        memset(&recv_buffer, 0, 1024);
    }
    close(sockfd);
}
