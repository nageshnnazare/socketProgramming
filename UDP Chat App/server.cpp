#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

const int maxClients = 5;

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char const* argv[]) {
    int sock_fd, port_num;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    int ret;
    char buffer[255];

    if (argc < 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    sscanf(argv[1], "%d", &port_num);

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        error("socket()");
    }

    bzero((char*)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_num);

    ret = bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        error("bind()");
    }

    client_len = sizeof(client_addr);

    while (true) {
        bzero(buffer, sizeof(buffer));

        ret = recvfrom(sock_fd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&client_addr, &client_len);
        if (ret < 0) {
            error("recvfrom()");
        }
        fprintf(stdout, "Client : %s\n", buffer);

        fprintf(stdout, "Server : ");
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        ret = sendto(sock_fd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&client_addr, client_len);
        if (ret < 0) {
            error("sendto()");
        }

        if (0 == strncmp("Bye", buffer, strlen("Bye"))) {
            break;
        }
    }

    close(sock_fd);

    return 0;
}
