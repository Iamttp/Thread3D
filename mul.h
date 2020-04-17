#ifndef OPENGLGAME_MUL_H
#define OPENGLGAME_MUL_H

#include <iostream>
#include <mutex>
#include <thread>

struct ItemRepository {
    static const int BUFFER_SIZE = 5; // Item buffer size.
    int buffer[BUFFER_SIZE]{}; // 产品缓冲区
    size_t out = 0; // 消费者读取产品位置.
    size_t in = 0; // 生产者写入产品位置.
    size_t counter = 0; // 当前容量
    std::mutex mtx; // 互斥量,保护产品缓冲区
} gItemRepository; // 产品库全局变量, 生产者和消费者操作该变量.

void ProduceItem(ItemRepository *ir, int item) {
    while (ir->counter == ItemRepository::BUFFER_SIZE); // 等待
    std::unique_lock<std::mutex> lock(ir->mtx);
    ir->buffer[ir->in] = item;
    ir->in = (ir->in + 1) % ItemRepository::BUFFER_SIZE;
    std::cout << "P " << item << " item\n";
    ir->counter++;
    lock.unlock(); // 解锁.
}

int ConsumeItem(ItemRepository *ir) {
    while (ir->counter == 0); // 等待
    std::unique_lock<std::mutex> lock(ir->mtx);
    int item = ir->buffer[ir->out];
    ir->out = (ir->out + 1) % ItemRepository::BUFFER_SIZE;
    std::cout << "C " << item << " item\n";
    ir->counter--;
    lock.unlock(); // 解锁.
    return item; // 返回产品.
}

static const int kItemsToProduce = 1000;   // How many items we plan to produce.
void ProducerTask() {
    for (int i = 1; i <= kItemsToProduce; ++i) {
        ProduceItem(&gItemRepository, i); // 循环生产 kItemsToProduce 个产品.
    }
}

void ConsumerTask() {
    static int cnt = 0;
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ConsumeItem(&gItemRepository); // 消费一个产品.
        if (++cnt == kItemsToProduce) break; // 如果产品消费个数为 kItemsToProduce, 则退出.
    }
}

#endif //OPENGLGAME_MUL_H
