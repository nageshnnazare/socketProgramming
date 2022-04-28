#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#define MAX_LEN 200
#define NUM_COLORS 6

struct terminal {
	int id;
	std::string name;
	int socket;
	std::thread th;
};

std::vector<terminal> clients;
std::string def_col = "\033[0m";
std::string colors[] = { "\033[31m", "\033[32m",
"\033[33m", "\033[34m", "\033[35m","\033[36m" };
int seed = 0;
std::mutex cout_mtx, clients_mtx;

std::string color(int code);
void set_name(int id, char name[]);
void shared_print(std::string str, bool endLine);
int broadcast_message(std::string message, int sender_id);
int broadcast_message(int num, int sender_id);
void end_connection(int id);
void handle_client(int client_socket, int id);

void error(const char* msg) {
	perror(msg);
	exit(1);
}

int main() {
	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		error("socket error ");
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	if ((bind(server_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_in))) == -1) {
		error("bind error ");
	}

	if ((listen(server_socket, 8)) == -1) {
		error("listen error ");
	}

	struct sockaddr_in client;
	int client_socket;
	unsigned int len = sizeof(sockaddr_in);

	std::cout << colors[NUM_COLORS - 1]
		<< "\n\t  ====== Welcome to the Group Chat ======   " << std::endl << def_col;

	while (1) {
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &len)) == -1) {
			error("accept error: ");
		}
		seed++;
		std::thread t(handle_client, client_socket, seed);
		std::lock_guard<std::mutex> guard(clients_mtx);
		clients.push_back({ seed, std::string("Anonymous"),client_socket,(move(t)) });
	}

	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].th.joinable())
			clients[i].th.join();
	}

	close(server_socket);
	return 0;
}

std::string color(int code) {
	return colors[code % NUM_COLORS];
}

void set_name(int id, char name[]) {
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].id == id) {
			clients[i].name = std::string(name);
		}
	}
}


void shared_print(std::string str, bool endLine = true) {
	std::lock_guard<std::mutex> guard(cout_mtx);
	std::cout << str;
	if (endLine)
		std::cout << std::endl;
}


int broadcast_message(std::string message, int sender_id) {
	char temp[MAX_LEN];
	strcpy(temp, message.c_str());
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].id != sender_id) {
			send(clients[i].socket, temp, sizeof(temp), 0);
		}
	}
}


int broadcast_message(int num, int sender_id) {
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].id != sender_id) {
			send(clients[i].socket, &num, sizeof(num), 0);
		}
	}
}

void end_connection(int id) {
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].id == id) {
			std::lock_guard<std::mutex> guard(clients_mtx);
			clients[i].th.detach();
			clients.erase(clients.begin() + i);
			close(clients[i].socket);
			break;
		}
	}
}

void handle_client(int client_socket, int id) {
	char name[MAX_LEN], str[MAX_LEN];
	recv(client_socket, name, sizeof(name), 0);
	set_name(id, name);

	std::string welcome_message = std::string(name) + std::string(" joined the chat");
	broadcast_message("#NULL", id);
	broadcast_message(id, id);
	broadcast_message(welcome_message, id);
	shared_print(color(id) + welcome_message + def_col);

	while (1) {
		int bytes_received = recv(client_socket, str, sizeof(str), 0);
		if (bytes_received <= 0)
			return;
		if (strcmp(str, "#exit") == 0) {

			std::string message = std::string(name) + std::string(" left the chat");
			broadcast_message("#NULL", id);
			broadcast_message(id, id);
			broadcast_message(message, id);
			shared_print(color(id) + message + def_col);
			end_connection(id);
			return;
		}
		broadcast_message(std::string(name), id);
		broadcast_message(id, id);
		broadcast_message(std::string(str), id);
		shared_print(color(id) + name + " : " + def_col + str);
	}
}
