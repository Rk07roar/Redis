#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char **argv) {
    std::cout << "Logs from your program will appear here!\n";

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Create a socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket\n";
        WSACleanup();
        return 1;
    }

    // Set socket options
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Set up the sockaddr_in structure
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    // Bind the socket
    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 6379\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, SOMAXCONN) != 0) {
        std::cerr << "listen failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for a client to connect...\n";

    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    // Accept incoming connections
    while (true) {
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection\n";
            continue; // Skip to the next iteration
        }

        std::string response = "+PONG\r\n";
        send(client_fd, response.c_str(), response.size(), 0);
        std::cout << "Client connected and responded with PONG\n";

        closesocket(client_fd); // Close the client socket after sending the response
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
