#ifndef OPENGLGAME_SEMAPHORE_H
#define OPENGLGAME_SEMAPHORE_H

#include <thread>
#include <mutex>
#include <condition_variable>

class semaphore {
    int count;
    std::mutex mtk;
    std::condition_variable cv;

public:
    explicit semaphore(int value = 1) : count(value) {}

    void wait() {
        std::unique_lock<std::mutex> lck(mtk);
        if (--count < 0)//资源不足挂起线程
            cv.wait(lck);
    }

    void signal() {
        std::unique_lock<std::mutex> lck(mtk);
        if (++count <= 0)//有线程挂起，唤醒一个
            cv.notify_one();
    }
};

#endif //OPENGLGAME_SEMAPHORE_H
