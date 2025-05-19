#include <iostream>     // std::cout, std::endl
#include <cstring>      // std::memset
#include <cstdio>       // std::perror

#include <sys/types.h>  // socklen_t
#include <sys/socket.h> // socket(), bind(), listen(), accept()
#include <netinet/in.h> // sockaddr_in, htons(), INADDR_ANY
#include <unistd.h>     // read(), close()

int main() {

    // --------- Setup sockets ----------------

    // structs representing an IPv4 socket address
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    // data type to store size of socket structures
    socklen_t client_addr_len = sizeof(client_addr);

    // buffer to store data read from the client
    char buffer[1024];

    // default IRC port
    int port = 6667;

    // --------- Create the server socket ----------------

    // creating a socket and returns a fd
    // socket(address family for IPv4, TCP socket, default protocol)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { // error check
        std::perror("socket");
        return 1;
    }

    // --------- Setup server address struct ----------------

    // clears the struct to avoid garbage values
    std::memset(&server_addr, 0, sizeof(server_addr));

    // AF_INET = using IPv4
    server_addr.sin_family = AF_INET;
    // INADDR_ANY = listen on all networks
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // htons(port) = conversion from host byte order to network byte order
    server_addr.sin_port = htons(port);


    // Allow address reuse (prevents "Address already in use" error)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    // --------- Bind the socket ----------------
    
    // bind links the socket to the network (IP address and port)
    // bind(socket fd, socket struct, size of socket struct)
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::perror("bind");
        close(server_fd); // error check
        return 1;
    }

    // --------- Listen incoming connections ----------------
    
    // listen makes the socket a listening socket
    // listen(socket fd, max connections that can wait in line)
    if (listen(server_fd, 5) < 0) {
        std::perror("listen"); // error check
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // --------- Accept client's connection ----------------

    // accept(socket fd, socket struct new fd, socket struct size)
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) { // error check
        std::perror("accept");
        close(server_fd);
        return 1;
    }

    std::cout << "Client connected" << std::endl;

    // --------- Read data from the client ----------------

    // read(client socket, buffer to store data received, size of buffer)
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) { // error check
        std::perror("read");
        // Close sockets
        close(client_fd);
        close(server_fd);
        return 1;
    }
    buffer[bytes_read] = '\0'; // Null-terminate the buffer
   
    // print message received and stored in buffer
    std::cout << "Received message: " << buffer << std::endl;

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
