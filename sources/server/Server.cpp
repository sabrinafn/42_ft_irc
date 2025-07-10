#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(0), socket_fd(0), password("") {}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other;}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->socket_fd = other.socket_fd;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {}

/* SETTERS */
void Server::setPortNumber(int other) {
    this->port = other;
}

void Server::setServerPassword(std::string other) {
    this->password = other;
}

/* GETTERS */
int Server::getPortNumber(void) {
    return this->port;
}

std::string Server::getServerPassword(void) {
    return this->password;
}

/* INIT SERVER */
void Server::initServer(void) {

   createSocket();
   connectToClient();
}

/* CREATE SOCKET */
void Server::createSocket(void) {

   // create socket
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   //int listening_socket = socket(AF_INET, SOCK_STREAM, 0); // replace by line above
   if (this->socket_fd == -1) {
       std::cerr << "Can't create a socket." << std::endl;
       return ;
   }


   /***********/


   /* Setar O_NONBLOCK com fcntl() */
   int flags = fcntl(this->socket_fd, F_GETFL, 0);
   if (flags == -1) {
       std::cerr << "Erro ao obter flags do socket\n";
       close(this->socket_fd);
       return ;
   }
   flags |= O_NONBLOCK;
   if (fcntl(this->socket_fd, F_SETFL, flags) == -1) {
       std::cerr << "Erro ao setar O_NONBLOCK\n";
       close(this->socket_fd);
       return ;
   }


   /***********/


   // bind socket to a IP/port
   sockaddr_in hint;
   hint.sin_family = AF_INET;
   hint.sin_port = htons(this->port);
   // hint.sin_port = htons(54000); // replace by line above
   inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);


   if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 ) {
       std::cerr << "Can't bind to IP/port";
       return;
   }


   // mark socket to start listening
   if (listen(this->socket_fd, SOMAXCONN) == -1) {
       std::cerr << "Can't listen" << std::endl;
       return;
   }
}


/* CONNECT TO CLIENT */
void Server::connectToClient(void) {


   // accept(socket fd, socket struct new fd, socket struct size)
   struct sockaddr_in client_addr;


   // data type to store size of socket structures
   socklen_t client_addr_len = sizeof(client_addr);


   int client_one = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
   this->clients_fd.push_back(client_one);
   if (this->clients_fd[0] < 0) {
       std::cerr << "Can't connect to client" << std::endl;
       close(this->socket_fd);
       close(this->clients_fd[0]);
       return ;
   }
   char buffer[1024];
   ssize_t bytes_read = read(this->clients_fd[0], buffer, sizeof(buffer) - 1);
   if (bytes_read < 0) {
       std::perror("read");
       // Close sockets
       close(this->clients_fd[0]);
       close(this->clients_fd[0]);
       return ;
   }
   buffer[bytes_read] = '\0';
 
   // print message received and stored in buffer
   std::cout << "Received message: " << buffer << std::endl;
   this->data_received = buffer;


   close(this->clients_fd[0]);
   close(this->socket_fd);
}

