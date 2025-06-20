
#pragma once
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

// ---------- 事件队列 线程安全 ---------

class EventQueue {
public:
  void post(std::function<void()> task);
  void processEvents();
  void stop();

private:
  std::queue<std::function<void()>> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> stopped_{false};  // 用于实现原子操作
};



// --------------- thread with event loop -----------

class Thread
{
private:
  std::shared_ptr<EventQueue> eventQueue_;
  std::thread thread_;
public:
  Thread();
  ~Thread();

  void post(std::function<void()> task);
};


// ------------- 对象基类 ------------------

class Object
{
public:
  virtual ~Object();

  void setParent(Object* parent);
  void moveToThread(Thread* thread);
  Thread* thread() const {return thread_; }
protected:
  Thread* thread_ = nullptr;
  Object* parent_ = nullptr;
  std::vector<Object*> children_;

};

// --------------- 信号和槽函数系统 --------------

class SlotBase
{
private:
  
public:
  virtual void execute() = 0;
  virtual ~SlotBase() = default;
};

template <typename Func>
class Slot : public SlotBase
{
public:
  Slot(Func func) : func_(std::move(func)) {};
  void execute() override { func_(); };
private:
  Func func_;
};


template <typename... Args>
class Signal  
{
public:
  template <typename Func>
  void connect(Func&& func, Object* receiver = nullptr) {
    slots_.emplace_back(new Slot<Func>(std::forward<Func>(func)), receiver);
  }

  void emit(Args... args) {
    for (auto& [slot, receiver] : slots_) {
      if (!receiver || receiver->thread() == nullptr) {
        slot->execute();
      } else {
        // 投递到目标线程的事件队列
        receiver->thread()->post([slot = slot.get()] { slot->execute(); });
      }
    }
  }

private:
  std::vector<std::pair<std::unique_ptr<SlotBase>, Object*>> slots_;
};




