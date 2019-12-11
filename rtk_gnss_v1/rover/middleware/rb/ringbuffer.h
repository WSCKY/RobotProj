#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "SysConfig.h"

typedef struct {
  uint8_t *elements;
  uint32_t capacity;
  uint32_t ptr_in;
  uint32_t ptr_out;
  bool_t flipped;
} ringbuffer_handle;

status_t ringbuffer_init(ringbuffer_handle *prb, uint8_t *buffer, uint32_t capacity);

status_t ringbuffer_reset(ringbuffer_handle *prb);
uint32_t ringbuffer_usage(ringbuffer_handle *prb);
uint32_t ringbuffer_available(ringbuffer_handle *prb);
status_t ringbuffer_push_byte(ringbuffer_handle *prb, uint8_t *byte);
status_t ringbuffer_poll_byte(ringbuffer_handle *prb, uint8_t *byte);
status_t ringbuffer_check_byte(ringbuffer_handle *prb, uint8_t *byte);
uint32_t ringbuffer_push(ringbuffer_handle *prb, uint8_t *cache, uint32_t len);
uint32_t ringbuffer_poll(ringbuffer_handle *prb, uint8_t *cache, uint32_t len);
status_t ringbuffer_check_byte_offset(ringbuffer_handle *prb, uint8_t *byte, uint32_t offset);

#endif /* __RINGBUFFER_H */
