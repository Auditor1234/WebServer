set(SRC
    main.cpp
)

add_executable(ThreadTest ${SRC})

target_link_libraries(ThreadTest libserver_base)
