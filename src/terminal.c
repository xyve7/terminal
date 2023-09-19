#include <terminal.h>

static const uint32_t terminal_color_table[] = {
    0x00000000,
    0x00cd3131,
    0x000dbc79,
    0x00e5e510,
    0x002472c8,
    0x00bc3fbc,
    0x0011a8cd,
    0x00e5e5e5,
    0x00666666,
    0x00f14c4c,
    0x0023d18b,
    0x00f5f543,
    0x003b8eea,
    0x00d670d6,
    0x0029b8db,
    0x00FFFFFF};
arialib_error terminal_new(terminal* term, framebuffer fb, const uint8_t* font, uint64_t font_height, uint64_t font_width) {
    term->fb = fb;

    term->rows = fb.height / font_height;
    term->cols = fb.width / font_width;

    term->font = font;
    term->font_height = font_height;
    term->font_width = font_width;

    term->row = 0;
    term->col = 0;

    term->fg = 0xFFFFFFFF;
    term->bg = 0x00000000;
    term->ansi = false;
    term->ansi_params[0] = 0;
    term->ansi_params[1] = 0;
    term->ansi_params_i = 0;

    memset(term->fb.address, term->bg, term->fb.size);

    return ARIALIB_SUCCESS;
}
arialib_error terminal_write_str(terminal* term, const char* str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        terminal_write_char(term, str[i]);
    }
    return ARIALIB_SUCCESS;
}
arialib_error terminal_scroll(terminal* term) {
    uint64_t offset = term->fb.pitch * 16;
    memmove(term->fb.address, term->fb.address + offset, term->fb.size - offset);
    memset(term->fb.address + (term->fb.size - offset), term->bg, offset);
    return ARIALIB_SUCCESS;
}
/**
 * @brief Sets cursor position, without scrolling or wrapping. (Used by ANSI escape codes)
 * 
 * @param term 
 * @param row 
 * @param col 
 * @return arialib_error 
 */
arialib_error terminal_set_cursor_pos(terminal* term, uint64_t row, uint64_t col) {
    if (col >= term->cols) {
        if ((int64_t)col < 0) {
            col = 0;
        } else {
            col = term->cols - 1;
        }
    }
    if (row >= term->rows) {
        if ((int64_t)row < 0) {
            row = 0;
        } else {
            row = term->rows - 1;
        }
    }
    term->row = row;
    term->col = col;
    return ARIALIB_SUCCESS;
}
arialib_error terminal_ansi_parse(terminal* term, char c) {
    switch (c) {
    case '[':
        term->ansi_seq = CSI;
        return ARIALIB_SUCCESS;
    }
    if (c >= '0' && c <= '9') {
        term->ansi_params[term->ansi_params_i] *= 10;
        term->ansi_params[term->ansi_params_i] += (c - '0');
        return ARIALIB_SUCCESS;
    } else if (c == ';') {
        term->ansi_params_i++;
        return ARIALIB_SUCCESS;
    }
    uint64_t row = term->row;
    uint64_t col = term->col;
    if (term->ansi_seq == CSI) {
        term->ansi_params_i++;
        switch (c) {
        case 'F':
            col = 0;
            // fall through
        case 'A':
            if (term->ansi_params_i == 0) {
                terminal_set_cursor_pos(term, row - 1, col);
            } else {
                terminal_set_cursor_pos(term, row - term->ansi_params[0], col);
            }
            goto cleanup;
        case 'E':
            col = 0;
            // fall through
        case 'B':
            if (term->ansi_params_i == 0) {
                terminal_set_cursor_pos(term, row + 1, col);
            } else {
                terminal_set_cursor_pos(term, row + term->ansi_params[0], col);
            }
            goto cleanup;
        case 'C':
            if (term->ansi_params_i == 0) {
                terminal_set_cursor_pos(term, row, col + 1);
            } else {
                terminal_set_cursor_pos(term, row, col + term->ansi_params[0]);
            }
            goto cleanup;
        case 'D':
            if (term->ansi_params_i == 0) {
                terminal_set_cursor_pos(term, row, col - 1);
            } else {
                terminal_set_cursor_pos(term, row, col - term->ansi_params[0]);
            }
            goto cleanup;
        case 'H':
            if (term->ansi_params_i == 0) {
                terminal_set_cursor_pos(term, 0, 0);
            } else {
                terminal_set_cursor_pos(term, term->ansi_params[0], term->ansi_params[1]);
            }
            goto cleanup;
        case 'm':
            for (uint64_t i = 0; i < term->ansi_params_i; i++) {
                if (term->ansi_params[i] >= 30 && term->ansi_params[i] <= 37) {
                    term->fg = terminal_color_table[term->ansi_params[i] - 30];
                } else if (term->ansi_params[i] >= 90 && term->ansi_params[i] <= 97) {
                    term->fg = terminal_color_table[(term->ansi_params[i] - 90) + 10];
                } else if (term->ansi_params[i] >= 40 && term->ansi_params[i] <= 47) {
                    term->fg = terminal_color_table[term->ansi_params[i] - 40];
                } else if (term->ansi_params[i] >= 100 && term->ansi_params[i] <= 107) {
                    term->fg = terminal_color_table[(term->ansi_params[i] - 100) + 10];
                } else {
                    term->fg = 0xFFFFFFFF;
                    term->bg = 0x00000000;
                }
            }
            goto cleanup;
        }
    }
cleanup:
    term->ansi_params[0] = 0;
    term->ansi_params[1] = 0;
    term->ansi_params_i = 0;
    term->ansi_seq = NONE;
    term->ansi = false;
    return ARIALIB_UNKNOWN;
}
/**
 * @brief Sets the position of the cursor, but has scrolling and text wrapping enabled.
 * 
 * @param term Terminal Object
 * @param row Row (Not the pixel, but the actual row on the terminal)
 * @param col Col (Again, not the pixel, but the actual column on the terminal)
 * @returns arialib_error 
 */
arialib_error terminal_set_cursor_pos_write(terminal* term, uint64_t row, uint64_t col) {
    if (col >= term->cols) {
        if ((int64_t)col < 0) {
            col = 0;
        } else {
            col = 0;
            row++;
        }
    }
    if (row >= term->rows) {
        if ((int64_t)row < 0) {
            row = 0;
        } else {
            row = term->rows - 1;
            terminal_scroll(term);
        }
    }
    term->row = row;
    term->col = col;
    return ARIALIB_SUCCESS;
}
/**
 * @brief Writes a character to the terminal screen
 * 
 * @param term 
 * @param c 
 * @return arialib_error 
 */
arialib_error terminal_write_char(terminal* term, uint8_t c) {
    if (term->ansi) {
        return terminal_ansi_parse(term, c);
    }
    switch (c) {
    case '\n':
        terminal_set_cursor_pos_write(term, term->row + 1, 0);
        break;
    case '\b':
        terminal_set_cursor_pos_write(term, term->row, term->col - 1);
        break;
    case '\t':
        terminal_set_cursor_pos_write(term, term->row, term->col + 4);
        break;
    case 27:
        term->ansi = true;
        term->ansi_seq = ESC;
        break;
    default:
        terminal_draw_char(term, c, term->fg, term->bg, term->row, term->col);
        terminal_set_cursor_pos_write(term, term->row, term->col + 1);
        break;
    }
    return ARIALIB_SUCCESS;
}

arialib_error terminal_draw_char(terminal* term, uint8_t c, uint32_t fg, uint32_t bg, uint64_t row, uint64_t col) {
    for (uint64_t height = 0; height < term->font_height; height++) {
        for (uint64_t width = 0; width < term->font_width; width++) {
            if ((term->font[(c * term->font_height) + height] & (0x80 >> width))) {
                framebuffer_set_pixel(&term->fb, (col * term->font_width) + width, (row * term->font_height) + height, fg);
            } else {
                framebuffer_set_pixel(&term->fb, (col * term->font_width) + width, (row * term->font_height) + height, bg);
            }
        }
    }
    return ARIALIB_SUCCESS;
}
arialib_error terminal_draw_cursor(terminal* term, uint32_t fg, uint32_t bg, uint64_t row, uint64_t col) {
    (void)bg;
    for (uint64_t height = 12; height < term->font_height; height++) {
        for (uint64_t width = 0; width < term->font_width; width++) {
            framebuffer_set_pixel(&term->fb, (col * term->font_width) + width, (row * term->font_height) + height, fg);
        }
    }
    return ARIALIB_SUCCESS;
}
