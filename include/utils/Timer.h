


// ----- 定时器 

#pragma once
#include "event.h"

class Timer : public Object
{
public:
  Signal<> timeout;
  void start(uint64_t interval);
};



typedef struct uart_opaque_type * uart_handle_t;

