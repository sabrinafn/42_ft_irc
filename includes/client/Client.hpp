#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {

    private:
        int fd;

    public:
        /* CONSTRUCTOR */
        Client(void);

        /* COPY CONSTRUCTOR */
        Client(const Client &other);

        /* OPERATORS */
        Client &operator=(const Client &other);

        /* DESTRUCTOR */
        ~Client(void);

        /* SETTERS */
        void setFd(int other);

        /* GETTERS */
        int getFd(void);
};

#endif
