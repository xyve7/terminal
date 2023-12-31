#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCESS,
    ALLOC,
    NEQ,
    NULL_REF,
    SAME_REF,
    UNKNOWN
} terminal_error;

typedef struct {
    void* address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t size;
    uint16_t bpp;
} framebuffer;

terminal_error framebuffer_new(framebuffer* fb, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch);
terminal_error framebuffer_from(framebuffer* fb, void* address, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch);
terminal_error framebuffer_dup(framebuffer* fb, framebuffer* other);

terminal_error framebuffer_copy(framebuffer* dest, framebuffer* source);

terminal_error framebuffer_set_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t color);
terminal_error framebuffer_get_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t* out_color);
#endif
