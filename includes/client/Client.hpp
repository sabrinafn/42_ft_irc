
/*
 * Client.hpp
 *
 * Defines the Client class, representing a single IRC client connection,
 * including its socket file descriptor, input buffer, and connection state.
 */
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>

class Client {

    private:
        int fd;
        std::string buffer;
        time_t last_activity;

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
        void setLastActivity(time_t t);

        /* GETTERS */
        int getFd(void) const;
        std::string getData(void) const;
        time_t getLastActivity() const;

        /* APPEND MORE DATA TO CURRENT BUFFER DATA */
        void appendData(std::string other);

};

#endif
