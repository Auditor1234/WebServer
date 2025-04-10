# WebServer
## 连接到来时的处理流程
1. 触发主线程事件循环监听的socket文件描述符，使用`queueInLoop()`将`HttpData::newEvent()`添加到子线程的事件循环中，并唤醒该子线程。
2. 子线程监听的事件文件描述符被触发，读取一个字节，更新wakeupChannel，处理任务队列中的任务。任务队列中的任务设置了客户端socket文件描述符的监听事件并添加到epoll中。
3. 因为客户端socket文件描述符本来就有读事件，因此`epoll_wait()`返回该文件描述符的事件，子线程调用`HttpData::handleRead()`与`HttpData::handleConn()`处理事件。此时任务队列为空，不需要执行。

