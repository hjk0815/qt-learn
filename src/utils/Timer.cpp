#include "Timer.h"

#include <iostream>

void Timer::start(uint64_t interval)
{
  thread()->post([this, interval] {
    std::cout << "Timer in thread: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    timeout.emit();
  });
}



