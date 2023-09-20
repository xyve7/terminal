#include <framebuffer.h>

terminal_error framebuffer_new(framebuffer* fb, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch) {
    fb->size = height * pitch;
    fb->address = malloc(fb->size);
    if (fb->address == NULL) {
        return ALLOC;
    }
    fb->height = height;
    fb->width = width;
    fb->pitch = pitch;
    fb->bpp = bpp;
    return SUCCESS;
}
terminal_error framebuffer_from(framebuffer* fb, void* address, uint64_t width, uint64_t height, uint16_t bpp, uint64_t pitch) {
    fb->size = height * pitch;
    fb->address = address;
    if (fb->address == NULL) {
        return NULL_REF;
    }
    fb->height = height;
    fb->width = width;
    fb->pitch = pitch;
    fb->bpp = bpp;
    return SUCCESS;
}
terminal_error framebuffer_dup(framebuffer* fb, framebuffer* other) {
    fb->size = other->size;
    if (other->address == NULL) {
        return NULL_REF;
    }
    fb->address = realloc(fb->address, other->size);
    if (fb->address == NULL) {
        return ALLOC;
    }
    memcpy(fb->address, other->address, other->size);
    fb->height = other->height;
    fb->width = other->width;
    fb->pitch = other->pitch;
    fb->bpp = other->bpp;
    return SUCCESS;
}
terminal_error framebuffer_copy(framebuffer* dest, framebuffer* source) {
    if (dest->size != source->size ||
        dest->width != source->width ||
        dest->height != source->height ||
        dest->pitch != source->pitch ||
        dest->bpp != source->bpp) {
        return NEQ;
    }
    if (dest->address == source->address) {
        return SAME_REF;
    }
    if (dest->address == NULL || source->address == NULL) {
        return NULL_REF;
    }
    memmove(dest->address, source->address, source->size);
    return SUCCESS;
}
terminal_error framebuffer_set_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t color) {
    *(volatile uint32_t*)((uint64_t)fb->address + (x * 4) + (y * (fb->pitch / (fb->bpp / 8)) * 4)) = color;
    return SUCCESS;
}
terminal_error framebuffer_get_pixel(framebuffer* fb, uint64_t x, uint64_t y, uint32_t* out_color) {
    *out_color = *(uint32_t*)((uint64_t)fb->address + (x * 4) + (y * (fb->pitch / (fb->bpp / 8)) * 4));
    return SUCCESS;
}
