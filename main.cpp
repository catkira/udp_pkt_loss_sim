#include <iostream>
#include <random>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "argparse.hpp"

constexpr int BUFFER_SIZE = 2048;

int main(int argc, char *argv[]) {
    argparse::ArgumentParser parser("RTP_Stream_Forwarder", "Reads an RTP stream from a socket and forwards it to another socket with a different UDP port");

    parser.add_argument("-s", "--source_port")
        .help("Source port number")
        .required()
        .action([](const std::string& value) { return std::stoi(value); });

    parser.add_argument("-d", "--destination_port")
        .help("Destination port number")
        .required()
        .action([](const std::string& value) { return std::stoi(value); });

    parser.add_argument("-p", "--drop_probability")
        .help("Drop probability")
        .required()
        .action([](const std::string& value) { return std::stof(value); });

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cout << parser;
        return 1;
    }

    int sourcePort = parser.get<int>("--source_port");
    int destinationPort = parser.get<int>("--destination_port");
    float dropProbability = parser.get<float>("--drop_probability");

    if (sourcePort == 0 || destinationPort == 0 || dropProbability < 0.0 || dropProbability > 1.0) {
        std::cerr << "Invalid arguments" << std::endl;
        std::cout << parser;
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

    // Forward RTP stream with drop probability
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);
    while (true) {
        ssize_t bytesRead = recv(sourceSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead == -1) {
            perror("Error reading from source socket");
            break;
        } else if (bytesRead == 0) {
            std::cout << "Source socket closed" << std::endl;
            break;
        }

        // Drop packet with probability 'dropProbability'
        if (dis(gen) < dropProbability) {
            std::cout << "Dropped packet" << std::endl;
            continue;
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
