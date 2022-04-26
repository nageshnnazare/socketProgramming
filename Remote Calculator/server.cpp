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
    int sock_fd, new_sock_fd, port_num;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    int ret;
    char buffer[255];

    if (argc < 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    sscanf(argv[1], "%d", &port_num);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    listen(sock_fd, maxClients);

    client_len = sizeof(client_addr);

    new_sock_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_len);
    if (new_sock_fd < 0) {
        error("accept()");
    }

    int num1, num2, ans, choice;
S:
    ret = write(new_sock_fd, "Enter Number 1 : ", strlen("Enter Number 1 : "));
    if (ret < 0) {
        error("Error writing to socket");
    }
    read(new_sock_fd, &num1, sizeof(int));
    if (ret < 0) {
        error("Error reading to socket");
    }

    ret = write(new_sock_fd, "Enter Number 2 : ", strlen("Enter Number 2 : "));
    if (ret < 0) {
        error("Error writing to socket");
    }
    read(new_sock_fd, &num2, sizeof(int));
    if (ret < 0) {
        error("Error reading to socket");
    }

    ret = write(new_sock_fd, "Enter choice : \n1. Add \n2. Sub \n3. Mul \n4. Div \n5. Exit\n",
        strlen("Enter choice : \n1. Add \n2. Sub \n3. Mul \n4. Div \n5. Exit\n"));
    if (ret < 0) {
        error("Error writing to socket");
    }
    read(new_sock_fd, &choice, sizeof(int));
    if (ret < 0) {
        error("Error reading to socket");
    }

    switch (choice) {
    case 1:
        ans = num1 + num2;
        break;
    case 2:
        ans = num1 - num2;
        break;
    case 3:
        ans = num1 * num2;
        break;
    case 4:
        if (num2 == 0) {
            ans = 0;
            break;
        }
        ans = num1 / num2;
        break;
    case 5:
        goto Q;
        break;
    default:
        goto S;
        break;
    }

    write(new_sock_fd, &ans, sizeof(int));
    if (choice != 5)
        goto S;

Q:
    close(new_sock_fd);
    close(sock_fd);

    return 0;
}
