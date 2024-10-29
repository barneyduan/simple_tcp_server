#include <iostream>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "utils.h"

std::mutex g_lock;

void handle_client(int client_socket) {
    std::lock_guard<std::mutex> lock(g_lock);

    char* buffer = new char[64];
    std::strcpy(buffer, generate_response().c_str());
    send(client_socket, buffer, strlen(buffer), 0);

    delete[] buffer;
    close(client_socket);
}

void start_server(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_socket);
        return;
    }

    listen(server_socket, 5);

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket >= 0) {
            std::thread(handle_client, client_socket).detach();
        }
    }
}

int main() {
    start_server(8080);
    return 0;
}
