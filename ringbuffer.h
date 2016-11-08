#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_
#include <stdint.h>
#include <string.h>

typedef struct {
    uint32_t  count; // number of items in ringbuffer
    uint32_t  size;  // size of data item
    uint32_t  readPos;
    uint32_t  writePos;
    uint8_t  *data;
} RingBuffer;

#define DECLEAR_RINGBUFFER(count, size, name) \
    uint8_t raw_data_##name[count * size]; \
    RingBuffer name = { count, size, 0, 0, (uint8_t *)(&raw_data_##name) };
    
int8_t ringbuffer_is_empty(const RingBuffer *rb);
int8_t ringbuffer_is_full(const RingBuffer *rb);
    
void ringbuffer_clear(RingBuffer *rb);
    
int8_t ringbuffer_read(RingBuffer *rb, uint8_t *buf, uint32_t size);
int8_t ringbuffer_write(RingBuffer *rb, const uint8_t *buf, uint32_t size);

#endif
