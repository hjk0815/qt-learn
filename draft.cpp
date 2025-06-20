#include <functional>
#include <memory>
#include <vector>

class Object; // 前向声明

class Signal {
public:
    template <typename Func, typename Receiver>
    void connect(Func&& func, Receiver* receiver) {
        // 使用 std::function 包装可调用对象
        auto slot = std::make_shared<std::function<void()>>(
            [func, receiver] { 
                if (receiver) {
                    // 如果是成员函数，需要绑定对象实例
                    if constexpr (std::is_member_function_pointer_v<std::decay_t<Func>>) {
                        std::invoke(func, receiver); // 调用成员函数
                    } else {
                        func(); // 直接调用函数/Lambda
                    }
                }
            }
        );

        slots_.emplace_back(slot, receiver);
    }

    // 发射信号
    void emit() {
        for (auto& [slot, receiver] : slots_) {
            if (!receiver || receiver->thread() == nullptr) {
                (*slot)(); // 直接调用
            } else {
                // 投递到目标线程的事件队列（跨线程）
                receiver->thread()->post([slot] { (*slot)(); });
            }
        }
    }

private:
    std::vector<std::pair<
        std::shared_ptr<std::function<void()>>, 
        Object*
    >> slots_;
};