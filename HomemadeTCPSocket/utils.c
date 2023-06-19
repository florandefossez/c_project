#include "utils.h"

// Taken from https://tools.ietf.org/html/rfc1071
uint16_t checksum(void *addr, int count) {
    register uint32_t sum = 0;
    uint16_t* ptr = addr;

    while( count > 1 ) {
        sum += * ptr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += *(uint8_t *) ptr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}