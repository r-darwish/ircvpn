#ifndef __AUTO_FD_H__
#define __AUTO_FD_H__

#include <unistd.h>

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
            close(m_fd);
        }
    }

private:
    int m_fd;
};

#endif
