#ifndef OPENGLGAME_MUL_H
#define OPENGLGAME_MUL_H

#include <iostream>
#include <mutex>
#include <thread>

// 产品模型
struct object {
    float r{}, g{}, b{};
    int alli{}, rei{};

    explicit object(int index) : alli(index) {
        r = rand() % 10 / 10.0;
        g = rand() % 10 / 10.0;
        b = rand() % 10 / 10.0;
    }

    object() = default;
};

struct ItemRepository {
    int index;
    int BUFFER_SIZE; // Item buffer size.
    object **buffer = new object *[BUFFER_SIZE](); // 产品缓冲区
    size_t out = 0; // 消费者读取产品位置.
    size_t in = 0; // 生产者写入产品位置.
    size_t counter = 0; // 当前容量
    std::mutex mtx; // 互斥量,保护产品缓冲区

    ItemRepository(int index, int bufferSize) : index(index), BUFFER_SIZE(bufferSize) {}

    virtual ~ItemRepository() {
        delete[]buffer;
    }
}; // 产品库全局变量, 生产者和消费者操作该变量.

void ProduceItem(ItemRepository *ir, object *item) {
    while (ir->counter == ir->BUFFER_SIZE); // 等待
    std::lock_guard<std::mutex> lock(ir->mtx);
    ir->buffer[ir->in] = item;
    item->rei = ir->in;
    ir->in = (ir->in + 1) % ir->BUFFER_SIZE;
    ir->counter++;
}

object *ConsumeItem(ItemRepository *ir) {
    while (ir->counter == 0); // 等待
    std::lock_guard<std::mutex> lock(ir->mtx);
    if (ir->counter == 0) return nullptr; // 多个consumeItem 解决方法
    auto *item = ir->buffer[ir->out];
    ir->buffer[ir->out] = nullptr;
    item->rei = ir->out;
    ir->out = (ir->out + 1) % ir->BUFFER_SIZE;
    ir->counter--;
    return item; // 返回产品.
}

void MoveItem(ItemRepository *in, ItemRepository *out) {
    auto *item = ConsumeItem(in);
    if (item == nullptr) return;
    ProduceItem(out, item);
}

void putTask(ItemRepository *gItemRepository2, ItemRepository *gItemRepository, const float *idle) {
    static int i;
    while (true) {
        int idlei = *idle;
        std::this_thread::sleep_for(std::chrono::milliseconds(idlei));
        ProduceItem(gItemRepository, new object(i++)); // 循环生产 kItemsToProduce 个产品.
    }
}

void getTask(ItemRepository *gItemRepository, ItemRepository *gItemRepository2, const float *idle) {
    while (true) {
        int idlei = *idle;
        std::this_thread::sleep_for(std::chrono::milliseconds(idlei));
        delete ConsumeItem(gItemRepository); // 消费一个产品.
    }
}

void moveTask(ItemRepository *inRepository, ItemRepository *outRepository, const float *idle) {
    while (true) { // just move it
        int idlei = *idle;
        std::this_thread::sleep_for(std::chrono::milliseconds(idlei));
        MoveItem(inRepository, outRepository); // 消费一个产品.
    }
}

#endif //OPENGLGAME_MUL_H
