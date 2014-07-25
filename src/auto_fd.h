#ifndef __AUTO_FD_H__
#define __AUTO_FD_H__

#include <stdio.h>
#include <unistd.h>
#include <boost/log/trivial.hpp>

class AutoFD
{
public:
    AutoFD(int fd) {
        this->m_fd = fd;
    }

    int get_fd() {
        return this->m_fd;
    }

    ~AutoFD() {
        if (0 <= m_fd) {
            BOOST_LOG_TRIVIAL(debug) << "Closing " << m_fd;
            close(m_fd);
        }
    }

private:
    int m_fd;
};

#endif
