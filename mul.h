#ifndef OPENGLGAME_MUL_H
#define OPENGLGAME_MUL_H

#include <iostream>
#include <mutex>
#include <thread>
#include "myList.h"

struct ItemRepository {
    int index;
    int BUFFER_SIZE; // Item buffer size.
    int *buffer = new int[BUFFER_SIZE](); // 产品缓冲区
    size_t out = 0; // 消费者读取产品位置.
    size_t in = 0; // 生产者写入产品位置.
    size_t counter = 0; // 当前容量
    std::mutex mtx; // 互斥量,保护产品缓冲区

    ItemRepository(int index, int bufferSize) : index(index), BUFFER_SIZE(bufferSize) {}

    virtual ~ItemRepository() {
        delete[]buffer;
    }
}; // 产品库全局变量, 生产者和消费者操作该变量.

struct node {
    ItemRepository *ir;
    ItemRepository *ir2{};
    int action; // 1 p 2 m 3 c

    node(ItemRepository *ir, ItemRepository *ir2, int action) : ir(ir), ir2(ir2), action(action) {}

    node(ItemRepository *ir, int action) : ir(ir), action(action) {}
};

threadsafe_queue<node> mesQ; // 消息队列


void ProduceItem(ItemRepository *ir, int item) {
    while (ir->counter == ir->BUFFER_SIZE); // 等待
    std::lock_guard<std::mutex> lock(ir->mtx);
    ir->buffer[ir->in] = item;
    ir->in = (ir->in + 1) % ir->BUFFER_SIZE;
    ir->counter++;

    node n(ir, 1);
    int x = 1;
    while (x--) mesQ.push(n);
}

int ConsumeItem(ItemRepository *ir) {
    while (ir->counter == 0); // 等待
    std::lock_guard<std::mutex> lock(ir->mtx);
    if (ir->counter == 0) return -1; // 多个consumeItem 解决方法
    int item = ir->buffer[ir->out];
    ir->out = (ir->out + 1) % ir->BUFFER_SIZE;
    ir->counter--;

    node n(ir, 3);
    int x = 1;
    while (x--) mesQ.push(n);
    return item; // 返回产品.
}

void MoveItem(ItemRepository *in, ItemRepository *out) {
//    while (in->counter <= 1);
    int item = ConsumeItem(in);
    if (item == -1) return;
    ProduceItem(out, item);

    node n(in, out, 2);
    int x = 1;
    while (x--) mesQ.push(n);
}

void putTask(ItemRepository *gItemRepository, const float *idle) {
    static int i;
    while (true) {
        int idlei = *idle;
        std::this_thread::sleep_for(std::chrono::milliseconds(idlei));
        ProduceItem(gItemRepository, i++); // 循环生产 kItemsToProduce 个产品.
    }
}

void getTask(ItemRepository *gItemRepository, const float *idle) {
    while (true) {
        int idlei = *idle;
        std::this_thread::sleep_for(std::chrono::milliseconds(idlei));
        ConsumeItem(gItemRepository); // 消费一个产品.
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
