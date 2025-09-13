#ifndef POLLSET_HPP
#define POLLSET_HPP

#include <vector>
#include <poll.h>
#include <iostream>
#include <unistd.h>

class Pollset {
   private:
    std::vector<pollfd> fd;

   public:

    Pollset(void);
    Pollset(const Pollset &other);
    Pollset &operator=(const Pollset &other);
    ~Pollset(void);
    void add(int fd);
    void remove(int fd);
    int poll(void);

    size_t                     getSize(void) const;
    struct pollfd             &getPollFd(int index);
    const std::vector<pollfd> &getPollfds() const;


    void clear(void);
};

#endif
