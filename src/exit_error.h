#pragma once

_Noreturn void exit_error(const STRING format, ...) {
    va_list args;
    va_start(args, format);
    VFPRINT(stderr, format, args);
    va_end(args);
    exit(1);
}
