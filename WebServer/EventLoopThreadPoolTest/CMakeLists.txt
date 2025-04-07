set(SRC
    main.cpp
    ../Channel.cpp
    ../Epoll.cpp
    ../EventLoop.cpp
    ../EventLoopThread.cpp
    ../EventLoopThreadPool.cpp
    ../Util.cpp
)

add_executable(EventLoopThreadPoolTest ${SRC})

target_link_libraries(EventLoopThreadPoolTest libserver_base)

