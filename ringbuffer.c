#include "ringbuffer.h"

int8_t ringbuffer_is_empty(const RingBuffer *rb) {
    return (rb->readPos) == (rb->writePos);
}

int8_t ringbuffer_is_full(const RingBuffer *rb) {
    return ((rb->writePos + 1) % rb->count) == (rb->readPos);
}

void ringbuffer_clear(RingBuffer *rb) {
    rb->readPos = 0;
    rb->writePos = 0;
}

int8_t ringbuffer_read(RingBuffer *rb, uint8_t *buf, uint32_t size) {
    if (size > rb->size) {
        return -1;
    }
    
    if (ringbuffer_is_empty(rb)) {
        return -1;
    }
    
    memcpy(buf, rb->data + rb->readPos * rb->size, size);
    rb->readPos = (rb->readPos + 1) % rb->count;    
    return size;
}

int8_t ringbuffer_write(RingBuffer *rb, const uint8_t *buf, uint32_t size) {
    if (size > rb->size) {
        return -1;
    }
    
    if (ringbuffer_is_full(rb)) {
        return -1;
    }
    
    memcpy(rb->data + rb->writePos * rb->size, buf, size);
    rb->writePos = (rb->writePos + 1) % rb->count;
    return size;
}
