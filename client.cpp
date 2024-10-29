#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void connect_and_request(const std::string& server_ip, int port, int requests_per_connection) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to server\n";
        close(sock);
        return;
    }

    for (int i = 0; i < requests_per_connection; ++i) {
        const char* message = "Client request\n";
        send(sock, message, strlen(message), 0);

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            std::cout << "Received from server: " << buffer << std::endl;
        } else {
            std::cerr << "Failed to receive data from server\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    close(sock);
}

int main() {
    std::string server_ip = "127.0.0.1";
    int port = 8080;
    int num_clients = 20;
    int requests_per_connection = 10;

    std::vector<std::thread> client_threads;
    for (int i = 0; i < num_clients; ++i) {
        client_threads.emplace_back(connect_and_request, server_ip, port, requests_per_connection);
    }

    for (auto& t : client_threads) {
        t.join();
    }

    return 0;
}
