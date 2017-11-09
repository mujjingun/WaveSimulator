#ifndef ERROR_H
#define ERROR_H

#include <cstdio>
#include <exception>

[[noreturn]] inline void error(const char* msg) noexcept {
    std::puts(msg);
    std::terminate();
}

#endif /* ERROR_H */
