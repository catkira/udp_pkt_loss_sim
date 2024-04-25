#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " -s <source_port> -d <destination_port>" << std::endl;
        return 1;
    }

    int sourcePort = 0, destinationPort = 0;

    // Parse command line arguments
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0) {
            sourcePort = std::atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-d") == 0) {
            destinationPort = std::atoi(argv[i + 1]);
        } else {
            std::cerr << "Invalid option: " << argv[i] << std::endl;
            return 1;
        }
    }

    if (sourcePort == 0 || destinationPort == 0) {
        std::cerr << "Invalid port numbers" << std::endl;
        return 1;
    }

    int sourceSocket, destinationSocket;
    struct sockaddr_in sourceAddr, destinationAddr;
    char buffer[BUFFER_SIZE];

    // Create source socket
    if ((sourceSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Source socket creation failed");
        return 1;
    }

    // Set source socket address
    memset((char *)&sourceAddr, 0, sizeof(sourceAddr));
    sourceAddr.sin_family = AF_INET;
    sourceAddr.sin_port = htons(sourcePort);
    sourceAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind source socket
    if (bind(sourceSocket, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr)) == -1) {
        perror("Source socket bind failed");
        close(sourceSocket);
        return 1;
    }

    // Create destination socket
    if ((destinationSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Destination socket creation failed");
        close(sourceSocket);
        return 1;
    }

    // Set destination socket address
    memset((char *)&destinationAddr, 0, sizeof(destinationAddr));
    destinationAddr.sin_family = AF_INET;
    destinationAddr.sin_port = htons(destinationPort);

    // Forward RTP stream
    while (true) {
        ssize_t bytesRead = recv(sourceSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead == -1) {
            perror("Error reading from source socket");
            break;
        } else if (bytesRead == 0) {
            std::cout << "Source socket closed" << std::endl;
            break;
        }
        ssize_t bytesSent = sendto(destinationSocket, buffer, bytesRead, 0, (struct sockaddr *)&destinationAddr, sizeof(destinationAddr));
        if (bytesSent == -1) {
            perror("Error forwarding to destination socket");
            break;
        }
    }

    // Close sockets
    close(sourceSocket);
    close(destinationSocket);

    return 0;
}
