
#include "modules/ring.h"
#include "ring.h"

RingBuffer::RingBuffer(void *buf_memory, size_t buf_size_bytes)
{
  ring_init(buf_memory, buf_size_bytes);
}

void RingBuffer::ring_init(void *buf_memory, size_t buf_size_bytes)
{
  ring_handle_ = (ring_handle_t)malloc(sizeof(struct ring_type));
  if (ring_handle_)
  {
    ring_handle_->buf = (uint8_t *)buf_memory;
    ring_handle_->LEN = buf_size_bytes;
    ring_handle_->rdpos = 0;
    ring_handle_->wrpos = 0;
  }
}
