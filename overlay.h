#ifndef OVERLAY_STRUCT
#define OVERLAY_STRUCT

#include "strings.h"
#include <sys/ioctl.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    string_t text;
    size_t cursor;
    size_t x;
    size_t y;
    bool anim;
    time_t last_anim;
} Mark;

typedef struct {
    Mark* marks;
    size_t marks_len;
} Overlay;

void open_overlay(struct winsize win);

int add_mark(size_t x, size_t y, bool slice);

int mark_stream_char(int index, char c);

void print_overlay();

void close_overlay();

#endif
