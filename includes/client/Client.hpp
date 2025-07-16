#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {

    private:
        int fd;
        std::string data;

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
        void setData(std::string other);

        /* GETTERS */
        int getFd(void) const;
        std::string getData(void) const;

        /* APPEND MORE DATA TO CURRENT BUFFER DATA */
        void appendData(std::string other);

};

#endif
