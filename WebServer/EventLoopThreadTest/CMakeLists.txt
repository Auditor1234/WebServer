set(SRC
    main.cpp
    ../Channel.cpp
    ../Epoll.cpp
    ../EventLoop.cpp
    ../EventLoopThread.cpp
    ../Util.cpp
)

add_executable(EventLoopThreadTest ${SRC})

target_link_libraries(EventLoopThreadTest libserver_base)

