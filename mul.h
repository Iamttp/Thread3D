#ifndef OPENGLGAME_MUL_H
#define OPENGLGAME_MUL_H

#include <iostream>
#include <mutex>
#include <thread>

struct ItemRepository {
    int BUFFER_SIZE = 9; // Item buffer size.
    int *buffer = new int[BUFFER_SIZE](); // 产品缓冲区 TODO delete
    size_t out = 0; // 消费者读取产品位置.
    size_t in = 0; // 生产者写入产品位置.
    size_t counter = 0; // 当前容量
    std::mutex mtx; // 互斥量,保护产品缓冲区

    ItemRepository(int bufferSize) : BUFFER_SIZE(bufferSize) {}
}; // 产品库全局变量, 生产者和消费者操作该变量.

void ProduceItem(ItemRepository *ir, int item) {
    while (ir->counter == ir->BUFFER_SIZE); // 等待
    std::unique_lock<std::mutex> lock(ir->mtx);
    ir->buffer[ir->in] = item;
    ir->in = (ir->in + 1) % ir->BUFFER_SIZE;
//    std::cout << "P " << item << " item\n";
    ir->counter++;
    lock.unlock(); // 解锁.
}

int ConsumeItem(ItemRepository *ir) {
    while (ir->counter == 0); // 等待
    std::unique_lock<std::mutex> lock(ir->mtx);
    int item = ir->buffer[ir->out];
    ir->out = (ir->out + 1) % ir->BUFFER_SIZE;
//    std::cout << "C " << item << " item\n";
    ir->counter--;
    lock.unlock(); // 解锁.
    return item; // 返回产品.
}

void MoveItem(ItemRepository *in, ItemRepository *out) {
    while (in->counter == 0 || out->counter == out->BUFFER_SIZE); // 等待
    std::unique_lock<std::mutex> lockIn(in->mtx);
    std::unique_lock<std::mutex> lockOut(out->mtx);
    int item = in->buffer[in->out];
    in->out = (in->out + 1) % in->BUFFER_SIZE;
    in->counter--;

    out->buffer[out->in] = item;
    out->in = (out->in + 1) % out->BUFFER_SIZE;
    out->counter++;
    lockIn.unlock(); // 解锁.
    lockOut.unlock(); // 解锁.
}

static const int kItemsToProduce = 1000;   // How many items we plan to produce.
void putTask(ItemRepository *gItemRepository, int idle) {
    for (int i = 1; i <= kItemsToProduce; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(idle));
        ProduceItem(gItemRepository, i); // 循环生产 kItemsToProduce 个产品.
    }
}

void getTask(ItemRepository *gItemRepository, int idle) {
    static int cnt = 0;
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(idle));
        ConsumeItem(gItemRepository); // 消费一个产品.
        if (++cnt == kItemsToProduce) break; // 如果产品消费个数为 kItemsToProduce, 则退出.
    }
}

void moveTask(ItemRepository *inRepository, ItemRepository *outRepository, int idle) {
    while (1) { // just move it
        std::this_thread::sleep_for(std::chrono::milliseconds(idle));
        MoveItem(inRepository, outRepository); // 消费一个产品.
    }
}

#endif //OPENGLGAME_MUL_H
