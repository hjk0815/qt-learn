#include <iostream>
#include <vector>
#include <memory>

#include <utils/event.h>
#include <utils/Timer.h>



class Worker : public Object {
public:
    Signal<> workFinished;

    void doWork() {
        std::cout << "Working in thread: " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        workFinished.emit();
    }
};

class Controller : public Object {
public:
    void onWorkFinished() {
        std::cout << "Result received in thread: " << std::this_thread::get_id() << std::endl;
    }
};


int main() {

    Thread timerThread;

    Timer timer;
    // 创建线程
    // Thread workerThread;
    Thread mainThread;

    // 创建对象
    // Worker worker;
    Controller controller;

    // 设置线程亲和性
    // worker.moveToThread(&workerThread);
    controller.moveToThread(&mainThread);
    timer.moveToThread(&timerThread);

    // 连接信号槽（自动跨线程）
    // worker.workFinished.connect([&] { controller.onWorkFinished(); }, &controller);

    timer.timeout.connect([&] { controller.onWorkFinished(); }, &controller);

    timerThread.post([&] { timer.start(1000); });
    // 投递任务到工作线程
    // workerThread.post([&] { worker.doWork(); });

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}




