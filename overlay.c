#include "overlay.h"
#include "strings.h"
#include "animations.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Overlay overlay;
size_t max_mark_len;

void open_overlay(struct winsize win) {
    overlay = (Overlay) {
        .marks = NULL,
        .marks_len = 0,
    };
    max_mark_len = win.ws_col;
}

int add_mark(size_t x, size_t y, bool slice) {
    Mark *marks = (Mark*) realloc(overlay.marks, sizeof(Mark) * (overlay.marks_len + 1));
    if (marks == NULL)
        return -1;

    overlay.marks = marks;
    overlay.marks[overlay.marks_len] = (Mark) {
        .x = x,
        .y = y,
        .text = repeat_char('\0', max_mark_len),
        .cursor = 0,
        .anim = slice,
        .last_anim = time(NULL),
    };

    return overlay.marks_len++;
}

int mark_stream_char(int index, char c) {
    Mark mark = overlay.marks[index];
    if (mark.cursor >= max_mark_len)
        return -1;

    char* str = mark.text->str;
    str[mark.cursor] = c;

    return ++overlay.marks[index].cursor;
}

void move_cursor_to(size_t x, size_t y) {
    printf("\x1b[%lu;%luH", y, x);
}

void print_overlay() {
    putchar(27);    putchar('0' + 7);
    // Color
    printf("\x1b[0m\x1b[48;2;200;200;200m\x1b[38;2;50;50;50m");

    for(size_t i = 0; i < overlay.marks_len; i++) {
        Mark *mark = overlay.marks + i;
        move_cursor_to(mark->x, mark->y);
        printf("%s", mark->text->str);
        if (!mark->anim) continue;

        time_t t = time(NULL);
        if (difftime(t, mark->last_anim) <= 1)
            continue;
        mark->last_anim = t;
        cyclic_shift(mark->text);
    }

    putchar(27);    putchar('0' + 8);
}

void close_overlay() {
    if (overlay.marks == NULL)
        return;
    for (size_t i = 0; i < overlay.marks_len; i++) {
        Mark mark = overlay.marks[i];
        free_string(mark.text);
    }
    free(overlay.marks);
}
