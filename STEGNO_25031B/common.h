#ifndef COMMON_H
#define COMMON_H


/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

/* ANSI color codes */
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RESET   "\033[0m"

/* Color print macros */
#define RED(fmt, ...) \
    printf(COLOR_RED fmt COLOR_RESET, ##__VA_ARGS__)

#define YELLOW(fmt, ...) \
    printf(COLOR_YELLOW fmt COLOR_RESET, ##__VA_ARGS__)

#endif /* COMMON_H */