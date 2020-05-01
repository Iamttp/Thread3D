## 先上题目：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200429081445345.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwNTE1Njky,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200429081813556.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwNTE1Njky,size_16,color_FFFFFF,t_70)
## 再上结果

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200429082609189.gif)
## 分析

对于这道题第一想法大都会是生产-消费者模型。这里确实非常类似，区别主要就是Move操作，但是Move操作可以看成 Put 和 Get 操作结合。

这里首先讲解下生产-消费者模型：

生产者－消费者问题是典型的PV操作问题，假设系统中有一个比较大的缓冲池，生产者的任务是只要缓冲池未满就可以将生产出的产品放入其中，而消费者的任务是只要缓冲池未空就可以从缓冲池中拿走产品。缓冲池被占用时，任何进程都不能访问。

### 简单想法

那么我们的第一想法应该就是：

```c
// 共享数据
#define BUFFER_SIZE 10
typedef struct{...} item;
item buffer[BUFFER_SIZE];
int in = out = counter = 0;
```

```c
// 生产者进程
while(true){
	while(counter == BUFFER_SIZE) ;
	buffer[in] = item;
	in = (in+1)%BUFFER_SIZE;
	counter++;
}
```

```c
// 消费者进程
while(true){
	while(counter == 0) ;
	item = buffer[out];
	out = (out+1)%BUFFER_SIZE;
	counter--;
}
```

上面的代码引用自哈工大操作系统的多线程讲解。第一次看到就觉得太精简漂亮了。
然而如果你没有汇编基础或者有基础但是没注意的话，就发现不了其中的猫腻！

### 问题及解决

问题就在于我们的程序一般都会编译成汇编语言，然后一般对应着编译成机器码。

比如： `counter++;` 就不是这么方便（暂时不考虑编译器优化）

它会转化为对应的汇编代码伪代码：

```c
reg = counter;
reg++;
counter = reg;
```

即需要先将内存的值移动到寄存器，再修改寄存器，最后把寄存器的值保存到内存。

而每一个进程都有其对应的PCB，它会保存进程自己的寄存器备份。

所以一个可能的执行序列为：

```c
P.reg = counter;	// 假设 counter 初始值为5
P.reg++;			// 6
C.reg = counter;	// 5
C.reg--;			// 4
counter = P.reg;	// 6
counter = C.reg;	// 4
```

所以counter的值最终变成了4！！！而不是5。

那么我们应该怎么办！一个简单方法是我执行生产者进程时，直接关闭中断就行了（即让操作系统不要跳过来跳过去的），执行完后你再跳呗。关中断可是需要权限的（内核态才行，要不然你忘了开回去不就完了）。即需要中断进入内核态！（理解需要一点操作系统知识）

停！这么麻烦，自然需要用到库，那么我们就需要 C++11 。C++新标准，跨平台，使用方便！！

### C++11 thread使用

一个简单的想法就是用C++11的锁。

```c
// 共享数据
#define BUFFER_SIZE 10
typedef struct{
	...
} item;
item buffer[BUFFER_SIZE];
std::mutex mtx; // 互斥量,保护产品缓冲区
int in = out = counter = 0;
```

```c
// 生产者进程
while(true){
	while(counter == BUFFER_SIZE) ;
	std::lock_guard<std::mutex> lock(mtx); // 锁上
    buffer[in] = item;
	in = (in+1)%BUFFER_SIZE;
	counter++;
	// 自动解锁
}
```

```c
// 消费者进程
while(true){
	while(counter == 0) ;
	std::lock_guard<std::mutex> lock(mtx);
    item = buffer[out];
	out = (out+1)%BUFFER_SIZE;
	counter--;
}
```

### 进一步改进

上面解法有没有问题？？笔者其实之前就觉得可以了，但是看了看操作系统书籍后发现问题了。自己的代码是"忙等待"的！！

（还有一个隐患！因为我们在判断 counter 时没有加锁！这可能在多个生产进程或者多个消费进程时，出现问题！）

忙等状态：
当一个进程正处在某临界区内，任何试图进入其临界区的进程都必须进入代码连续循环，陷入忙等状态。连续测试一个变量直到某个值出现为止，称为忙等。
（没有进入临界区的正在等待的某进程不断的在测试循环代码段中的变量的值，占着处理机而不释放，这是一种忙等状态～）-> 这个时候应该释放处理机让给其他进程

让权等待：
当进程不能进入自己的临界区时，应立即释放处理机，以免进程陷入“忙等”状态～（受惠的是其他进程）

（参考自：<https://blog.csdn.net/liuchuo/article/details/51986201>）

（我知道为啥我的风扇呼呼的了）

### 信号量与PV原语

那该怎么解决呢？事情似乎越来越复杂了！这里真的不得不佩服计算机的大神们！

<https://baike.baidu.com/item/%E8%89%BE%E5%85%B9%E6%A0%BC%C2%B7%E8%BF%AA%E7%A7%91%E6%96%AF%E5%BD%BB/5029407?fr=aladdin>

大家学数据结构都听过 Dijkstra 算法吧！这位大神还提出过信号量和PV原语！

这个大杀器可以解决很多经典的多线程/进程问题。现在我们先忘掉前面的那些东西，抽象出两个原语（不会被系统中断打断）。

	PV操作：一种实现进程互斥与同步的有效方法，包含P操作与V操作。
		
	P操作：使 S=S-1 ，若 S>=0 ，则该进程继续执行，否则排入等待队列。
	
	V操作：使 S=S+1 ，若 S>0 ,唤醒等待队列中的一个进程。
	
	临界资源：同一时刻只允许一个进程访问的资源，与上面所说的 S 有关联。


对于生产消费者问题直接可以这样解决：

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200429091228621.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwNTE1Njky,size_16,color_FFFFFF,t_70)

## 核心代码

首先用实现c++11信号量，因为C++11只提供了互斥量（锁）和条件变量。
所以我们通过这两个配合实现 semaphore 信号量。

```cpp
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
```

然后就是生产消费者的代码

```cpp
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

	// 省略构造析构函数
}
```

```cpp
void ProduceItem(ItemRepository *ir, object *item) {
    ir->emptyL->wait();
    ir->mtxL->wait();
    ir->buffer[ir->in] = item;
    ir->in = (ir->in + 1) % ir->BUFFER_SIZE;
    ir->counter++;
    ir->mtxL->signal();
    ir->fullL->signal();
}
```

```cpp
object *ConsumeItem(ItemRepository *ir) {
    ir->fullL->wait();
    ir->mtxL->wait();
    auto *item = ir->buffer[ir->out];
    ir->buffer[ir->out] = nullptr;
    ir->out = (ir->out + 1) % ir->BUFFER_SIZE;
    ir->counter--;
    ir->mtxL->signal();
    ir->emptyL->signal();
    return item; // 返回产品.
}
```

```cpp
void MoveItem(ItemRepository *in, ItemRepository *out) {
    ProduceItem(out, ConsumeItem(in));
}
```

