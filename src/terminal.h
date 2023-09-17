#ifndef TERMINAL_H
#define TERMINAL_H

#include <framebuffer.h>
#include <stdio.h>

typedef enum {
    NONE,
    ESC,
    CSI
} ansi_mode;

typedef struct {
    framebuffer fb;
    uint64_t rows;
    uint64_t cols;
    const uint8_t* font;
    uint64_t font_height;
    uint64_t font_width;
    uint64_t row;
    uint64_t col;

    /* ANSI Control Codes. */
    bool ansi;
    ansi_mode ansi_seq;

    uint32_t fg;
    uint32_t bg;
    
    uint8_t ansi_params[2];
    uint8_t ansi_params_i;
} terminal;

arialib_error terminal_new(terminal* term, framebuffer fb, const uint8_t* font, uint64_t font_height, uint64_t font_width);

arialib_error terminal_write_str(terminal* term, const char* str, uint64_t len);
arialib_error terminal_write_char(terminal* term, uint8_t c);

arialib_error terminal_draw_char(terminal* term, uint8_t c, uint32_t fg, uint32_t bg, uint64_t row, uint64_t col);

arialib_error terminal_set_cursor_pos(terminal* term, uint64_t row, uint64_t col);

#endif