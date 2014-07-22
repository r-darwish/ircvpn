#include <stdio.h>
#include <exception>
#include "tun_device.h"

using namespace std;

int main() {
    try {
        TunDevice tun("10.0.0.1", "10.0.0.2");
        getchar();
    } catch (exception & e) {
        printf("Critical Exception: %s\n", e.what());
    }
}
