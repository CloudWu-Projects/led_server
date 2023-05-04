#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

class TcpServer {
public:
    TcpServer(int port) : port(port) {}

    void start() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        // Initialize Winsock
        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != 0) {
            std::cerr << "Error: Could not initialize Winsock." << std::endl;
            return;
        }
#endif

        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket == -1) {
            std::cerr << "Error: Could not create socket." << std::endl;
            return;
        }

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(port);

        if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
            std::cerr << "Error: Could not bind to port " << port << "." << std::endl;
            return;
        }

        listen(server_socket, 5);

        std::cout << "Listening on port " << port << "..." << std::endl;

        std::thread accept_thread(&TcpServer::accept_clients, this);
        std::thread process_thread(&TcpServer::process_clients, this);

        accept_thread.join();
        process_thread.join();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        // Cleanup Winsock
        WSACleanup();
#endif
    }

private:
    int server_socket;
    int port;
    std::mutex queue_mutex;
    std::queue<int> client_sockets;

    void handle_client(int client_socket) {
        char buffer[1024] = {0};
        read(client_socket, buffer, 1024);
        std::cout << "Received data: " << buffer << std::endl;

        const char *response = "Thanks for connecting.";
        write(client_socket, response, strlen(response));

        close(client_socket);
    }

    void accept_clients() {
        while (true) {
            int client_socket;
            struct sockaddr_in client_address;
            socklen_t client_address_size = sizeof(client_address);

            client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_size);

            if (client_socket < 0) {
                std::cerr << "Error: Could not accept client connection." << std::endl;
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                client_sockets.push(client_socket);
            }
        }
    }

    void process_clients() {
        while (true) {
            int client_socket;

            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (client_sockets.empty()) {
                    continue;
                }
                client_socket = client_sockets.front();
                client_sockets.pop();
            }

            std::thread client_thread(&TcpServer::handle_client, this, client_socket);
            client_thread.detach();
        }
    }
};

