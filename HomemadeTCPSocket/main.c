#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <errno.h>
#include <netdb.h>

#include "ether.h"


#define BUFFER_SIZE 65535

int tapdev;
const u_int8_t MYMAC[6]   = {0x02, 0x59, 0x11, 0x04, 0x20, 0x00};
const char MYIP[16]       = "22.22.22.22";

int main() {
    // int tap_fd;
    char* device_name = "tap0";
    char buffer[BUFFER_SIZE];
    
    // Open the TAP device
    tapdev = open("/dev/net/tun", O_RDWR);
    if (tapdev == -1) {
        perror("Error opening TAP device");
        return 1;
    }
    
    // Set TAP device flags
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strcpy(ifr.ifr_name, device_name);
    if (ioctl(tapdev, TUNSETIFF, (void *)&ifr) == -1) {
        perror("Error setting TAP device flags");
        close(tapdev);
        return 1;
    }
    
    // Enter the packet capture loop
    while (1) {
        ssize_t bytes_read = read(tapdev, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Error reading packet");
            close(tapdev);
            return 1;
        }
        
        printf("Packet captured! Length: %zd\n", bytes_read);
        parse_eth(buffer, bytes_read);
        
        
    }
    
    // Close the TAP device
    close(tapdev);

    return 0;
}

