


#include "event.h"


void EventQueue::post(std::function<void()> task)
{
  std::lock_guard<std::mutex> lock(mutex_);
  queue_.push(std::move(task));
  cv_.notify_one();
}

void EventQueue::processEvents()
{
  while (!stopped_)
  {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] {return !queue_.empty() || stopped_;});
      if (stopped_)
      {
        break;
      }
      task = std::move(queue_.front());
      queue_.pop();
    }
    task();
  }
}

void EventQueue::stop()
{
  stopped_ = true;
  cv_.notify_all();
}





Thread::Thread()
  : eventQueue_(std::make_shared<EventQueue>())
{
  thread_ = std::thread([this] { eventQueue_->processEvents(); });
}

Thread::~Thread()
{
  eventQueue_->stop();
  if (thread_.joinable())
  {
    thread_.join();
  }
}

void Thread::post(std::function<void()> task)
{
  eventQueue_->post(std::move(task));
}



Object::~Object()
{
  for (auto child : children_)
  {
    child->parent_ = nullptr;
    delete child;
  }
}

void Object::setParent(Object* parent)
{
  if (parent_)
  {
    auto& siblings = parent_->children_;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
  }
  
}

void Object::moveToThread(Thread* thread)
{
  thread_ = thread;
}

