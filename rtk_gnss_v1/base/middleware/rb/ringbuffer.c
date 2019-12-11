#include "ringbuffer.h"

status_t ringbuffer_init(ringbuffer_handle *prb, uint8_t *buffer, uint32_t capacity)
{
  prb->elements = buffer;
  prb->capacity = capacity;
  prb->ptr_in = 0;
  prb->ptr_out = 0;
  prb->flipped = false;
  return status_ok;
}

status_t ringbuffer_reset(ringbuffer_handle *prb)
{
  prb->ptr_in = 0;
  prb->ptr_out = 0;
  prb->flipped = false;
  return status_ok;
}

uint32_t ringbuffer_usage(ringbuffer_handle *prb)
{
  uint32_t used;
  if(prb->flipped == false)
    used = prb->ptr_in - prb->ptr_out;
  else
    used = prb->capacity - prb->ptr_out + prb->ptr_in;
  return ((used * 100) / prb->capacity);
}

uint32_t ringbuffer_available(ringbuffer_handle *prb)
{
  if(prb->flipped == false) {
    return prb->ptr_in - prb->ptr_out;
  }
  return prb->capacity - prb->ptr_out + prb->ptr_in;
}

status_t ringbuffer_push_byte(ringbuffer_handle *prb, uint8_t *byte)
{
  if(prb->flipped == false) {
    if(prb->ptr_in == prb->capacity) {
      prb->ptr_in = 0;
      prb->flipped = true;

      if(prb->ptr_in < prb->ptr_out) {
        prb->elements[prb->ptr_in ++] = *byte;
        return status_ok;
      } else {
        return status_error;
      }
    } else {
      prb->elements[prb->ptr_in ++] = *byte;
      return status_ok;
    }
  } else {
    if(prb->ptr_in < prb->ptr_out) {
      prb->elements[prb->ptr_in ++] = *byte;
      return status_ok;
    } else {
      return status_error;
    }
  }
}

status_t ringbuffer_poll_byte(ringbuffer_handle *prb, uint8_t *byte)
{
  if(prb->flipped == false) {
    if(prb->ptr_out < prb->ptr_in) {
      *byte = prb->elements[prb->ptr_out ++];
      return status_ok;
    } else {
      return status_error;
    }
  } else {
    if(prb->ptr_out == prb->capacity) {
      prb->ptr_out = 0;
      prb->flipped = false;
      if(prb->ptr_out < prb->ptr_in) {
        *byte = prb->elements[prb->ptr_out ++];
        return status_ok;
      } else {
        return status_error;
      }
    } else {
      *byte = prb->elements[prb->ptr_out ++];
      return status_ok;
    }
  }
}

uint32_t ringbuffer_push(ringbuffer_handle *prb, uint8_t *cache, uint32_t len)
{
  uint32_t newElementsReadPos = 0, endPos;
  if(prb->flipped == false) {
    // readPos lower than writePos - free sections are:
    // 1) from writePos tp capacity
    // 2) from 0 to readPos
    if(len <= prb->capacity - prb->ptr_in) {
      // new elements fit into top of elements array - copy directly
      for(; newElementsReadPos < len; newElementsReadPos ++) {
        prb->elements[prb->ptr_in ++] = cache[newElementsReadPos];
      }
      return newElementsReadPos;
    } else {
      // new elements must be divided between top and bottom of elements array
      
      // writing to top
      for(; prb->ptr_in < prb->capacity; prb->ptr_in ++) {
        prb->elements[prb->ptr_in] = cache[newElementsReadPos ++];
      }
      prb->ptr_in = 0;
      prb->flipped = true;
      endPos = (len - newElementsReadPos);
      endPos = (endPos < prb->ptr_out) ? endPos : prb->ptr_out;
      for(; prb->ptr_in < endPos; prb->ptr_in ++) {
        prb->elements[prb->ptr_in] = cache[newElementsReadPos ++];
      }
      return newElementsReadPos;
    }
  } else {
    // readPos higher than writePos - free sections are:
    // 1) from writePos to readPos
    endPos = prb->ptr_in + len;
    endPos = (endPos < prb->ptr_out) ? endPos : prb->ptr_out;
    for(; prb->ptr_in < endPos; prb->ptr_in ++) {
      prb->elements[prb->ptr_in] = cache[newElementsReadPos ++];
    }
    return newElementsReadPos;
  }
}

uint32_t ringbuffer_poll(ringbuffer_handle *prb, uint8_t *cache, uint32_t len)
{
  uint32_t intoWritePos = 0, endPos;
  if(prb->flipped == false) {
    // writePos higher than readPos - available section is writePos - readPos
    endPos = prb->ptr_out + len;
    endPos = (endPos < prb->ptr_in) ? endPos : prb->ptr_in;
    for(; prb->ptr_out < endPos; prb->ptr_out ++) {
      cache[intoWritePos ++] = prb->elements[prb->ptr_out];
    }
    return intoWritePos;
  } else {
    // readPos higher than writePos - available sections are:
    // top + bottom of elements array

    if(len <= prb->capacity - prb->ptr_out) {
      // length is lower than the elements available at the top
      // of the elements array - copy directly
      for(; intoWritePos < len; intoWritePos ++) {
        cache[intoWritePos] = prb->elements[prb->ptr_out ++];
      }
      return intoWritePos;
    } else {
      // length is higher than elements available at the top of the elements array
      // split copy into a copy from both top and bottom of elements array.

      // copy from top
      for(; prb->ptr_out < prb->capacity; prb->ptr_out ++) {
        cache[intoWritePos ++] = prb->elements[prb->ptr_out];
      }

      // copy from bottom
      prb->ptr_out = 0;
      prb->flipped = false;
      endPos = len - intoWritePos;
      endPos = (endPos < prb->ptr_in) ? endPos : prb->ptr_in;
      for(; prb->ptr_out < endPos; prb->ptr_out ++) {
        cache[intoWritePos ++] = prb->elements[prb->ptr_out];
      }
      return intoWritePos;
    }
  }
}

status_t ringbuffer_check_byte(ringbuffer_handle *prb, uint8_t *byte)
{
  if(prb->flipped == false) {
    if(prb->ptr_out < prb->ptr_in) {
      *byte = prb->elements[prb->ptr_out];
      return status_ok;
    } else {
      return status_error;
    }
  } else {
    if(prb->ptr_out == prb->capacity) {
      if(0 < prb->ptr_in) {
        *byte = prb->elements[0];
        return status_ok;
      } else {
        return status_error;
      }
    } else {
      *byte = prb->elements[prb->ptr_out];
      return status_ok;
    }
  }
}

status_t ringbuffer_check_byte_offset(ringbuffer_handle *prb, uint8_t *byte, uint32_t offset)
{
  uint32_t rd_pos = prb->ptr_out + offset;
  if(prb->flipped == false) {
    if(rd_pos < prb->ptr_in) {
      *byte = prb->elements[rd_pos];
      return status_ok;
    } else {
      return status_error;
    }
  } else {
    if(rd_pos >= prb->capacity) {
      rd_pos -= prb->capacity;
      if(rd_pos < prb->ptr_in) {
        *byte = prb->elements[rd_pos];
        return status_ok;
      } else {
        return status_error;
      }
    } else {
      *byte = prb->elements[rd_pos];
      return status_ok;
    }
  }
}
