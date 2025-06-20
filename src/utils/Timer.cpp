#include "Timer.h"

void Timer::start(uint64_t interval)
{
  thread()->post([this, interval] {
    // std::this_thread::sleep_for(std::chrono::seconds(interval));
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    timeout.emit();
  });
}



