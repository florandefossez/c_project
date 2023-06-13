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
// #include <memory.h>
#include <errno.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 1500


int main() {
    int tap_fd, tcp_fd, client_fd;

    if ((tap_fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Cannot open TUN/TAP driver");
        return -1;
    }
    if (tap_fd < 0) {
        fprintf(stderr, "Failed to create TAP interface\n");
        return 1;
    }

    // Create TCP socket
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        perror("Failed to create TCP socket");
        close(tap_fd);
        return 1;
    }

    // Bind TCP socket to TAP interface
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind TCP socket");
        close(tap_fd);
        close(tcp_fd);
        return 1;
    }

    // Set TCP socket to listen mode
    if (listen(tcp_fd, 1) < 0) {
        perror("Failed to listen on TCP socket");
        close(tap_fd);
        close(tcp_fd);
        return 1;
    }

    // printf("TAP interface '%s' created.\n", tap_name);
    printf("Listening for incoming TCP connections on port 12345...\n");

    // Accept incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_fd = accept(tcp_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Failed to accept incoming connection");
        close(tap_fd);
        close(tcp_fd);
        return 1;
    }

    printf("TCP connection accepted.\n");

    // Data transfer loop
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    int n=5;
    while (n>0) {
        // Read
        bytes_read = read(client_fd, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            perror("Failed to read from TAP interface");
            break;
        }

        // Write
        bytes_written = write(client_fd, buffer, bytes_read);
        if (bytes_written < 0) {
            perror("Failed to write to TCP socket");
            break;
        }
        n--;
    }

    // Cleanup
    close(client_fd);
    close(tap_fd);
    close(tcp_fd);

    return 0;
}
