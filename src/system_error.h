#ifndef __SYSTEM_ERROR_H__
#define __SYSTEM_ERROR_H__

#include <string.h>
#include <exception>
#include <boost/format.hpp>
using namespace std;
using boost::format;

class SystemError : public exception
{
public:
    int m_errno;
    const char * m_message;

    SystemError(int err, const char * message) :
            m_format("%1%: %2% (%3%)") {
        m_format % message % strerror(errno) % errno;
        m_errno = err;
        this->m_message = message;
    }

    const char * what() const throw(){
        return m_format.str().c_str();
    }

    ~SystemError()  throw() {

    }

private:
    format m_format;
};

static int inline system_call(int result, const char * error_message)
{
    if (-1 == result) {
        throw SystemError(errno, error_message);
    }

    return result;
}

#endif
