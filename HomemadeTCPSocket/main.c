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
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>


#define BUFFER_SIZE 65535

int main() {
    int tap_fd;
    char* device_name = "tap0";
    char buffer[BUFFER_SIZE];
    
    // Open the TAP device
    tap_fd = open("/dev/net/tun", O_RDWR);
    if (tap_fd == -1) {
        perror("Error opening TAP device");
        return 1;
    }
    
    // Set TAP device flags
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strcpy(ifr.ifr_name, device_name);
    if (ioctl(tap_fd, TUNSETIFF, (void *)&ifr) == -1) {
        perror("Error setting TAP device flags");
        close(tap_fd);
        return 1;
    }
    
    // Enter the packet capture loop
    while (1) {
        ssize_t bytes_read = read(tap_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Error reading packet");
            close(tap_fd);
            return 1;
        }
        
        printf("Packet captured! Length: %zd\n", bytes_read);
        
        
    }
    
    // Close the TAP device
    close(tap_fd);
    
    return 0;
}

