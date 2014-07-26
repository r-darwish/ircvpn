#ifndef __AUTO_FD_H__
#define __AUTO_FD_H__

#include <stdio.h>
#include <unistd.h>
#include <boost/log/trivial.hpp>

class auto_fd
{
public:
    auto_fd(int fd) : fd(fd) { }

    inline int get_fd() const { return this->fd; }

    ~auto_fd() {
        if (0 <= fd) {
            BOOST_LOG_TRIVIAL(debug) << "Closing " << fd;
            close(fd);
        }
    }

private:
    int fd;
};

#endif
