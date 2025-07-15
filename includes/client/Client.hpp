#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>

class Client {

    private:
        int fd;
        std::string data;
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
        void setLastActivity(time_t time);

        /* GETTERS */
        int getFd(void);
        std::string getData(void);
        time_t getLastActivity(void);

        /* APPEND MORE DATA TO CURRENT BUFFER DATA */
        void appendData(std::string other);

};

#endif
