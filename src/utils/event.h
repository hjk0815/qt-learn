
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
public:
  virtual void execute() = 0;
  virtual ~SlotBase() = default;

  template<typename... Args>
  void invoke(Args&&... args) {
    executeImpl(std::forward<Args>(args)...);
  }
private:
  virtual void executeImpl() {}
};

template <typename Func, typename... Args>
class Slot : public SlotBase
{
public:
  Slot(Func func) : func_(std::move(func)) {};
  void execute() override { func_(); }

  void executeImpl(Args&&... args) override {
    func_(std::forward<Args>(args)...);
  }
private:
  Func func_;
};


template <typename... Args>
class Signal  
{
public:
  // 支持lambda函数和普通函数
  template <typename Func>
  void connect(Func&& func, Object* receiver = nullptr) {
    using SlotType = Slot<Func, Args...>;
    slots_.emplace_back(std::make_unique<SlotType>(std::forward<Func>(func)), receiver);
  }

  // 支持成员函数指针
  template <typename T>
  void connect(void (T::*method)(Args...), T* receiver) {
    auto func = [receiver, method](Args... args) {
      (receiver->*method)(std::forward<Args>(args)...);
    };
    using FuncType = decltype(func);
    using SlotType = Slot<FuncType, Args...>;
    slots_.emplace_back(std::make_unique<SlotType>(std::move(func)), receiver);
  }

  void emit(Args... args) {
    for (auto& [slot, receiver] : slots_) {
      if (!receiver || receiver->thread() == nullptr) {
        // slot->execute();
        slot->invoke(std::forward<Args>(args)...);
      } else {
        // 捕获参数并转发到线程
        auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
        auto task = [slot = slot.get(), args_tuple = std::move(args_tuple)]() mutable {
          std::apply([slot](auto&&... unpackedArgs) {
              slot->invoke(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
          }, args_tuple);
        };
        receiver->thread()->post(std::move(task));
      }
    }
  }

private:
  std::vector<std::pair<std::unique_ptr<SlotBase>, Object*>> slots_;
};




