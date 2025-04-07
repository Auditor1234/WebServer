set(SRC
    main.cpp
    Channel.cpp
    Epoll.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    Util.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/base)

add_executable(WebServer ${SRC})

target_link_libraries(WebServer libserver_base)

add_subdirectory(base)
add_subdirectory(ThreadTest)
add_subdirectory(EventLoopTest)
add_subdirectory(EventLoopThreadTest)
add_subdirectory(EventLoopThreadPoolTest)