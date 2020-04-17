#include "mul.h"

int main() {
    std::thread producer(ProducerTask); // 创建生产者线程.
    std::thread consumer(ConsumerTask); // 创建消费之线程.
    producer.join();
    consumer.join();
}
