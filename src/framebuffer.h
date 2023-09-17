#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <arialib.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    void* address; 
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t size;
    uint16_t bpp;
} framebuffer;

arialib_error framebuffer_new(framebuffer* fb, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch);
arialib_error framebuffer_from(framebuffer* fb, void* address, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch);
arialib_error framebuffer_dup(framebuffer* fb, framebuffer* other);

arialib_error framebuffer_copy(framebuffer* dest, framebuffer* source);

arialib_error framebuffer_set_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t color);
arialib_error framebuffer_get_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t* out_color);
#endif
