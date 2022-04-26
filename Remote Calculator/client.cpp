#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char const* argv[]) {
    int sock_fd, port_num;
    struct sockaddr_in server_addr;
    struct hostent* server;

    int ret;
    char buffer[255];

    if (argc < 3) {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    sscanf(argv[2], "%d", &port_num);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        error("socket()");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, No such host\n");
        exit(1);
    }

    bzero((char*)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, sizeof(server->h_length));
    server_addr.sin_port = htons(port_num);

    ret = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        error("connect()");
    }

    int num1, num2, ans, choice;
S:
    bzero(buffer, sizeof(buffer));
    ret = read(sock_fd, buffer, sizeof(buffer));
    if (ret < 0) {
        error("Error reading to socket");
    }
    printf("<Server> %s", buffer);

    scanf("%d", &num1);
    write(sock_fd, &num1, sizeof(int));
    if (ret < 0) {
        error("Error writing to socket");
    }

    bzero(buffer, sizeof(buffer));
    ret = read(sock_fd, buffer, sizeof(buffer));
    if (ret < 0) {
        error("Error reading to socket");
    }
    printf("<Server> %s", buffer);

    scanf("%d", &num2);
    write(sock_fd, &num2, sizeof(int));
    if (ret < 0) {
        error("Error writing to socket");
    }

    bzero(buffer, sizeof(buffer));
    ret = read(sock_fd, buffer, sizeof(buffer));
    if (ret < 0) {
        error("Error reading to socket");
    }
    printf("<Server> %s", buffer);

    scanf("%d", &choice);
    write(sock_fd, &choice, sizeof(int));
    if (ret < 0) {
        error("Error writing to socket");
    }

    if (choice == 5)
        goto Q;

    ret = read(sock_fd, &ans, sizeof(int));
    if (ret < 0) {
        error("Error reading to socket");
    }
    printf("<Server> The Answer is : %d\n"
        "---------------------------------\n", ans);

    if (choice != 5)
        goto S;

Q:
    printf("Exiting application\n");
    close(sock_fd);

    return 0;
}
