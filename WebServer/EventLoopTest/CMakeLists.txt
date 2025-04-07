set(SRC
    main.cpp
    ../Channel.cpp
    ../Epoll.cpp
    ../EventLoop.cpp
    ../Util.cpp
)

add_executable(EventLoopTest ${SRC})

target_link_libraries(EventLoopTest libserver_base)

