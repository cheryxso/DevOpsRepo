#include "FuncA.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

class HTTPServer {
public:
    HTTPServer() : server_fd(-1) {}

    void start();

private:
    int server_fd;

    void handleClient(int clientSocket);
    std::string generateResponse(double elapsedTime);
    double performCalculations();
    void setupServer();
};

void HTTPServer::setupServer() {
    struct sockaddr_in address{};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

double HTTPServer::performCalculations() {
    FuncA func;
    const int numElements = 2000000;
    const int sortCycles = 500;
    std::vector<double> values;

    // Prepare random values for calculation
    for (int i = 0; i < numElements; ++i) {
        double x = static_cast<double>(i) / numElements * 2 * M_PI;
        values.push_back(func.calculate(x, 3));
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < sortCycles; ++i) {
        std::sort(values.begin(), values.end());
        std::reverse(values.begin(), values.end());
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

std::string HTTPServer::generateResponse(double elapsedTime) {
    std::string body = "Elapsed time: " + std::to_string(elapsedTime) + " seconds";
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "\r\n";
    response += body;
    return response;
}

void HTTPServer::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    read(clientSocket, buffer, 1024);

    // Parse the request method and path
    std::string request(buffer);
    std::string method, path;
    size_t methodEnd = request.find(' ');
    if (methodEnd != std::string::npos) {
        method = request.substr(0, methodEnd);
        size_t pathEnd = request.find(' ', methodEnd + 1);
        if (pathEnd != std::string::npos) {
            path = request.substr(methodEnd + 1, pathEnd - methodEnd - 1);
        }
    }

    if (method == "GET" && path == "/compute") {
        double elapsedTime = performCalculations();
        std::string response = generateResponse(elapsedTime);
        send(clientSocket, response.c_str(), response.size(), 0);
    } else if (method == "GET") {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    } else {
        std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
}

void HTTPServer::start() {
    setupServer();
    std::cout << "Server started on port " << PORT << std::endl;

    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(server_fd, (struct sockaddr *)&clientAddress, &clientAddressLen);

        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }

        handleClient(clientSocket);
    }
}

int main() {
    HTTPServer server;
    server.start();
    return 0;
}

