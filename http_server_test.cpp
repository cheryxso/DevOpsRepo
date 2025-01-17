#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

void testServerResponse() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    const char *httpRequest = "GET /compute HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send(sock, httpRequest, strlen(httpRequest), 0);

    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = read(sock, buffer, BUFFER_SIZE);

    if (bytesRead > 0) {
        std::string response(buffer);
        std::cout << "Server response:\n" << response << std::endl;

        // Validate response structure
        assert(response.find("HTTP/1.1 200 OK") != std::string::npos);
        assert(response.find("Elapsed time:") != std::string::npos);

        // Extract elapsed time and check range
        size_t start = response.find("Elapsed time: ") + 14;
        size_t end = response.find(" seconds", start);
        double elapsedTime = std::stod(response.substr(start, end - start));
        std::cout << "Elapsed time: " << elapsedTime << " seconds" << std::endl;

        assert(elapsedTime >= 5.0 && elapsedTime <= 20.0);
    } else {
        std::cerr << "No response received from server" << std::endl;
        assert(false);
    }

    close(sock);
}

int main() {
    std::cout << "Starting HTTP server test..." << std::endl;

    // Allow server startup delay if running together
    std::this_thread::sleep_for(std::chrono::seconds(2));

    testServerResponse();

    std::cout << "HTTP server test passed." << std::endl;
    return 0;
}

