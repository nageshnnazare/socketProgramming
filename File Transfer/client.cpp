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
    char buffer[2048];

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

    bzero(buffer, sizeof(buffer));

    FILE* f;
    int words = 0;
    char c;

    f = fopen("input.txt", "r");
    while ((c = getc(f)) != EOF) {
        fscanf(f, "%s", buffer);
        if (isspace(c) || c == '\t') {
            words++;
        }
    }
    write(sock_fd, &words, sizeof(int));
    rewind(f);

    char ch;
    while (ch != EOF) {
        fscanf(f, "%s", buffer);
        write(sock_fd, buffer, sizeof(buffer));
        ch = fgetc(f);
    }

    fprintf(stdout, "File Transferred Successfully!\n");

    close(sock_fd);

    return 0;
}
