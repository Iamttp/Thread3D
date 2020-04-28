#ifndef OPENGLGAME_MUL_H
#define OPENGLGAME_MUL_H

#include "semaphore.h"

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
    object **buffer; // 产品缓冲区
    size_t out = 0; // 消费者读取产品位置.
    size_t in = 0; // 生产者写入产品位置.
    size_t counter = 0; // 当前容量
    semaphore *mtxL;
    semaphore *emptyL;
    semaphore *fullL;

    ItemRepository(int index, int bufferSize) : index(index), BUFFER_SIZE(bufferSize) {
        buffer = new object *[BUFFER_SIZE]();
        emptyL = new semaphore(bufferSize);
        fullL = new semaphore(0);
        mtxL = new semaphore();
    }

    virtual ~ItemRepository() {
        delete[]buffer;
        delete mtxL;
        delete emptyL;
        delete fullL;
    }
};

void ProduceItem(ItemRepository *ir, object *item) {
    ir->emptyL->wait();
    ir->mtxL->wait();
    ir->buffer[ir->in] = item;
    item->rei = ir->in;
    ir->in = (ir->in + 1) % ir->BUFFER_SIZE;
    ir->counter++;
    ir->mtxL->signal();
    ir->fullL->signal();
}

object *ConsumeItem(ItemRepository *ir) {
    ir->fullL->wait();
    ir->mtxL->wait();
    auto *item = ir->buffer[ir->out];
    ir->buffer[ir->out] = nullptr;
    item->rei = ir->out;
    ir->out = (ir->out + 1) % ir->BUFFER_SIZE;
    ir->counter--;
    ir->mtxL->signal();
    ir->emptyL->signal();
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
