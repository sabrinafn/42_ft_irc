#ifndef SERVER_HPP
#define SERVER_HPP

class Server {

    private:
        int port;
        int server_socket_fd;

    public:
        /* CONSTRUCTOR */
        Server(void);

        /* COPY CONSTRUCTOR */
        Server(const Server &other);

        /* OPERATORS */
        Server &operator=(const Server &other);

        /* DESTRUCTOR */
        ~Server(void);

        /* INIT SERVER */
        void initServer(void);

        /* CREATE SOCKET */
        void createSocket(void);
};

#endif