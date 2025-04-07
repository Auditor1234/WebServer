#pragma once
#include <stdint.h>

namespace CurrentThread {
extern __thread int t_cached_tid;
extern __thread char t_tid_string[32];
extern __thread int t_tid_string_length;
extern __thread const char* t_thread_name;

void cacheTid();
inline int tid() {
    if(__builtin_expect(t_cached_tid == 0, 0)) {
        cacheTid();
    }
    return t_cached_tid;
}

inline char* tidString() {
    return t_tid_string;
}

inline int tidStringLength() {
    return t_tid_string_length;
}

inline const char* tidThreadName() {
    return t_thread_name;
}
}