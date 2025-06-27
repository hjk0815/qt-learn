
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <iostream>


typedef struct ring_type * ring_handle_t;


struct ring_type
{
  volatile uint16_t rdpos;
  volatile uint16_t wrpos;
  uint16_t LEN;
  uint16_t RSVD;
  volatile uint8_t *buf;
};

class RingBuffer
{
public:
  RingBuffer(void *buf_memory, size_t buf_size_bytes);
  ~RingBuffer() = default;

protected:
  void ring_init(void * buf_memory, size_t buf_size_bytes);

private:
  // 禁止移动构造和移动赋值
  RingBuffer(RingBuffer&&) = delete;
  RingBuffer& operator=(RingBuffer&&) = delete;
  // 禁止拷贝和赋值
  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
private:
  ring_handle_t ring_handle_;

};



