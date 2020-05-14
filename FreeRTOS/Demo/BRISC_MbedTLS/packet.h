#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>

typedef struct Packet
{
    uint32_t size;
    uint8_t *body;
}
Packet_t;

#endif /* __PACKET_H__ */
