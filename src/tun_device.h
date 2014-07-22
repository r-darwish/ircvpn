#ifndef __TUN_DEVICE_H__
#define __TUN_DEVICE_H__

class TunDevice
{
public:
    TunDevice(const char * src_ip, const char * dest_ip);
    ~TunDevice();
    int get_fd() { return this->m_fd; }

private:
    int m_fd;
};

#endif
