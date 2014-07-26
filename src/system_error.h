#ifndef __SYSTEM_ERROR_H__
#define __SYSTEM_ERROR_H__

#include <string.h>
#include <exception>
#include <boost/format.hpp>

using namespace std;
using boost::format;

class system_error : public exception
{
public:
    system_error(int err, const char * message) :
        error_code(err),
        message(message),
        format_msg("%1%: %2% (%3%)")
    {
        format_msg % message % strerror(errno) % errno;
    }

    const char * what() const throw(){
        return format_msg.str().c_str();
    }

    inline int get_error_code() const { return error_code; }

    ~system_error() throw() { }

private:
    int error_code;
    const char * message;
    format format_msg;
};

static int inline system_call(int result, const char * error_message)
{
    if (-1 == result) {
        throw system_error(errno, error_message);
    }

    return result;
}

#endif
