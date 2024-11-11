#include "tunix_renderer.h"
#include "tunix_platform.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct tunix_renderer {
    int width;
    int height;
    tunix_cell *front_buffer;
    tunix_cell *back_buffer;
};

tunix_renderer* tunix_renderer_create(int width, int height) {
    tunix_renderer *renderer = (tunix_renderer*)malloc(sizeof(tunix_renderer));
    if (!renderer) {
        return NULL;
    }
    
    renderer->width = width;
    renderer->height = height;
    renderer->front_buffer = (tunix_cell*)calloc(width * height, sizeof(tunix_cell));
    renderer->back_buffer = (tunix_cell*)calloc(width * height, sizeof(tunix_cell));
    
    if (!renderer->front_buffer || !renderer->back_buffer) {
        tunix_renderer_destroy(renderer);
        return NULL;
    }
    
    /* Initialize cells with default values */
    for (int i = 0; i < width * height; i++) {
        strcpy(renderer->front_buffer[i].ch, " ");
        strcpy(renderer->back_buffer[i].ch, " ");
        renderer->front_buffer[i].fg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        renderer->front_buffer[i].bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        renderer->back_buffer[i].fg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        renderer->back_buffer[i].bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
    }
    
    return renderer;
}

void tunix_renderer_destroy(tunix_renderer *renderer) {
    if (!renderer) {
        return;
    }
    
    free(renderer->front_buffer);
    free(renderer->back_buffer);
    free(renderer);
}

void tunix_renderer_resize(tunix_renderer *renderer, int width, int height) {
    if (!renderer || (renderer->width == width && renderer->height == height)) {
        return;
    }
    
    tunix_cell *new_front = (tunix_cell*)calloc(width * height, sizeof(tunix_cell));
    tunix_cell *new_back = (tunix_cell*)calloc(width * height, sizeof(tunix_cell));
    
    if (!new_front || !new_back) {
        free(new_front);
        free(new_back);
        return;
    }
    
    for (int i = 0; i < width * height; i++) {
        strcpy(new_front[i].ch, " ");
        strcpy(new_back[i].ch, " ");
        new_front[i].fg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        new_front[i].bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        new_back[i].fg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        new_back[i].bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
    }
    
    free(renderer->front_buffer);
    free(renderer->back_buffer);
    
    renderer->front_buffer = new_front;
    renderer->back_buffer = new_back;
    renderer->width = width;
    renderer->height = height;
}

void tunix_renderer_get_size(tunix_renderer *renderer, int *width, int *height) {
    if (!renderer) {
        return;
    }
    
    if (width) *width = renderer->width;
    if (height) *height = renderer->height;
}

void tunix_renderer_clear(tunix_renderer *renderer, tunix_style_color bg) {
    if (!renderer) {
        return;
    }
    
    for (int i = 0; i < renderer->width * renderer->height; i++) {
        strcpy(renderer->back_buffer[i].ch, " ");
        renderer->back_buffer[i].fg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
        renderer->back_buffer[i].bg = bg;
        renderer->back_buffer[i].bold = false;
        renderer->back_buffer[i].underline = false;
    }
}

void tunix_renderer_set_cell(tunix_renderer *renderer, int x, int y, const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || x < 0 || x >= renderer->width || y < 0 || y >= renderer->height) {
        return;
    }
    
    int index = y * renderer->width + x;
    strncpy(renderer->back_buffer[index].ch, ch ? ch : " ", sizeof(renderer->back_buffer[index].ch) - 1);
    renderer->back_buffer[index].ch[sizeof(renderer->back_buffer[index].ch) - 1] = '\0';
    renderer->back_buffer[index].fg = fg;
    renderer->back_buffer[index].bg = bg;
}

void tunix_renderer_draw_text(tunix_renderer *renderer, int x, int y, const char *text, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !text || y < 0 || y >= renderer->height) {
        return;
    }
    
    int pos = x;
    for (const char *p = text; *p && pos < renderer->width; p++, pos++) {
        if (pos >= 0) {
            char ch[2] = {*p, '\0'};
            tunix_renderer_set_cell(renderer, pos, y, ch, fg, bg);
        }
    }
}

void tunix_renderer_draw_rect(tunix_renderer *renderer, tunix_rect rect, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer) {
        return;
    }
    
    for (int y = rect.y; y < rect.y + rect.height && y < renderer->height; y++) {
        if (y < 0) continue;
        for (int x = rect.x; x < rect.x + rect.width && x < renderer->width; x++) {
            if (x < 0) continue;
            tunix_renderer_set_cell(renderer, x, y, " ", fg, bg);
        }
    }
}

void tunix_renderer_draw_border(tunix_renderer *renderer, tunix_rect rect, tunix_border_style style, const tunix_border_chars *chars, tunix_style_color color) {
    if (!renderer || style == TUNIX_BORDER_NONE) {
        return;
    }
    
    tunix_border_chars default_chars;
    if (!chars) {
        tunix_theme theme = tunix_theme_default();
        bool use_unicode = tunix_platform_supports_unicode();
        
        switch (style) {
            case TUNIX_BORDER_SINGLE:
                if (use_unicode) {
                    strcpy(default_chars.top_left, "┌");
                    strcpy(default_chars.top_right, "┐");
                    strcpy(default_chars.bottom_left, "└");
                    strcpy(default_chars.bottom_right, "┘");
                    strcpy(default_chars.horizontal, "─");
                    strcpy(default_chars.vertical, "│");
                } else {
                    strcpy(default_chars.top_left, "+");
                    strcpy(default_chars.top_right, "+");
                    strcpy(default_chars.bottom_left, "+");
                    strcpy(default_chars.bottom_right, "+");
                    strcpy(default_chars.horizontal, "-");
                    strcpy(default_chars.vertical, "|");
                }
                break;
            case TUNIX_BORDER_DOUBLE:
                if (use_unicode) {
                    strcpy(default_chars.top_left, "╔");
                    strcpy(default_chars.top_right, "╗");
                    strcpy(default_chars.bottom_left, "╚");
                    strcpy(default_chars.bottom_right, "╝");
                    strcpy(default_chars.horizontal, "═");
                    strcpy(default_chars.vertical, "║");
                } else {
                    strcpy(default_chars.top_left, "+");
                    strcpy(default_chars.top_right, "+");
                    strcpy(default_chars.bottom_left, "+");
                    strcpy(default_chars.bottom_right, "+");
                    strcpy(default_chars.horizontal, "=");
                    strcpy(default_chars.vertical, "|");
                }
                break;
            case TUNIX_BORDER_ROUNDED:
                if (use_unicode) {
                    strcpy(default_chars.top_left, "╭");
                    strcpy(default_chars.top_right, "╮");
                    strcpy(default_chars.bottom_left, "╰");
                    strcpy(default_chars.bottom_right, "╯");
                    strcpy(default_chars.horizontal, "─");
                    strcpy(default_chars.vertical, "│");
                } else {
                    strcpy(default_chars.top_left, "/");
                    strcpy(default_chars.top_right, "\\");
                    strcpy(default_chars.bottom_left, "\\");
                    strcpy(default_chars.bottom_right, "/");
                    strcpy(default_chars.horizontal, "-");
                    strcpy(default_chars.vertical, "|");
                }
                break;
            case TUNIX_BORDER_THICK:
                if (use_unicode) {
                    strcpy(default_chars.top_left, "┏");
                    strcpy(default_chars.top_right, "┓");
                    strcpy(default_chars.bottom_left, "┗");
                    strcpy(default_chars.bottom_right, "┛");
                    strcpy(default_chars.horizontal, "━");
                    strcpy(default_chars.vertical, "┃");
                } else {
                    strcpy(default_chars.top_left, "#");
                    strcpy(default_chars.top_right, "#");
                    strcpy(default_chars.bottom_left, "#");
                    strcpy(default_chars.bottom_right, "#");
                    strcpy(default_chars.horizontal, "=");
                    strcpy(default_chars.vertical, "#");
                }
                break;
            default:
                default_chars = theme.border_chars;
                break;
        }
        chars = &default_chars;
    }
    
    tunix_style_color bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
    
    /* Corners */
    tunix_renderer_set_cell(renderer, rect.x, rect.y, chars->top_left, color, bg);
    tunix_renderer_set_cell(renderer, rect.x + rect.width - 1, rect.y, chars->top_right, color, bg);
    tunix_renderer_set_cell(renderer, rect.x, rect.y + rect.height - 1, chars->bottom_left, color, bg);
    tunix_renderer_set_cell(renderer, rect.x + rect.width - 1, rect.y + rect.height - 1, chars->bottom_right, color, bg);
    
    /* Horizontal lines */
    for (int x = rect.x + 1; x < rect.x + rect.width - 1; x++) {
        tunix_renderer_set_cell(renderer, x, rect.y, chars->horizontal, color, bg);
        tunix_renderer_set_cell(renderer, x, rect.y + rect.height - 1, chars->horizontal, color, bg);
    }
    
    /* Vertical lines */
    for (int y = rect.y + 1; y < rect.y + rect.height - 1; y++) {
        tunix_renderer_set_cell(renderer, rect.x, y, chars->vertical, color, bg);
        tunix_renderer_set_cell(renderer, rect.x + rect.width - 1, y, chars->vertical, color, bg);
    }
}

void tunix_renderer_draw_hline(tunix_renderer *renderer, int x, int y, int width, const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !ch) {
        return;
    }
    
    for (int i = 0; i < width; i++) {
        tunix_renderer_set_cell(renderer, x + i, y, ch, fg, bg);
    }
}

void tunix_renderer_draw_vline(tunix_renderer *renderer, int x, int y, int height, const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !ch) {
        return;
    }
    
    for (int i = 0; i < height; i++) {
        tunix_renderer_set_cell(renderer, x, y + i, ch, fg, bg);
    }
}

static void write_color_code(char *buf, size_t size, tunix_style_color color, bool is_bg) {
    const char *prefix = is_bg ? "\033[48;" : "\033[38;";
    
    if (color.type == TUNIX_COLOR_DEFAULT) {
        snprintf(buf, size, "%s", is_bg ? "\033[49m" : "\033[39m");
    } else if (color.type == TUNIX_COLOR_RGB) {
        snprintf(buf, size, "%s2;%d;%d;%dm", prefix, color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        int code = 0;
        switch (color.type) {
            case TUNIX_COLOR_BLACK: code = 0; break;
            case TUNIX_COLOR_RED: code = 1; break;
            case TUNIX_COLOR_GREEN: code = 2; break;
            case TUNIX_COLOR_YELLOW: code = 3; break;
            case TUNIX_COLOR_BLUE: code = 4; break;
            case TUNIX_COLOR_MAGENTA: code = 5; break;
            case TUNIX_COLOR_CYAN: code = 6; break;
            case TUNIX_COLOR_WHITE: code = 7; break;
            case TUNIX_COLOR_BRIGHT_BLACK: code = 8; break;
            case TUNIX_COLOR_BRIGHT_RED: code = 9; break;
            case TUNIX_COLOR_BRIGHT_GREEN: code = 10; break;
            case TUNIX_COLOR_BRIGHT_YELLOW: code = 11; break;
            case TUNIX_COLOR_BRIGHT_BLUE: code = 12; break;
            case TUNIX_COLOR_BRIGHT_MAGENTA: code = 13; break;
            case TUNIX_COLOR_BRIGHT_CYAN: code = 14; break;
            case TUNIX_COLOR_BRIGHT_WHITE: code = 15; break;
            default: break;
        }
        snprintf(buf, size, "%s5;%dm", prefix, code);
    }
}

void tunix_renderer_present(tunix_renderer *renderer) {
    if (!renderer) {
        return;
    }
    
    char output_buffer[1024 * 64];
    size_t output_pos = 0;
    
    tunix_style_color last_fg = {0};
    tunix_style_color last_bg = {0};
    bool first_cell = true;
    
    for (int y = 0; y < renderer->height; y++) {
        for (int x = 0; x < renderer->width; x++) {
            int index = y * renderer->width + x;
            tunix_cell *front = &renderer->front_buffer[index];
            tunix_cell *back = &renderer->back_buffer[index];
            
            /* Skip unchanged cells */
            if (!first_cell && 
                strcmp(front->ch, back->ch) == 0 &&
                memcmp(&front->fg, &back->fg, sizeof(tunix_style_color)) == 0 &&
                memcmp(&front->bg, &back->bg, sizeof(tunix_style_color)) == 0) {
                continue;
            }
            
            /* Move cursor if needed */
            char move_buf[32];
            int move_len = snprintf(move_buf, sizeof(move_buf), "\033[%d;%dH", y + 1, x + 1);
            if (output_pos + move_len < sizeof(output_buffer)) {
                memcpy(output_buffer + output_pos, move_buf, move_len);
                output_pos += move_len;
            }
            
            /* Set colors if changed */
            if (first_cell || memcmp(&last_fg, &back->fg, sizeof(tunix_style_color)) != 0) {
                char fg_buf[32];
                write_color_code(fg_buf, sizeof(fg_buf), back->fg, false);
                size_t fg_len = strlen(fg_buf);
                if (output_pos + fg_len < sizeof(output_buffer)) {
                    memcpy(output_buffer + output_pos, fg_buf, fg_len);
                    output_pos += fg_len;
                }
                last_fg = back->fg;
            }
            
            if (first_cell || memcmp(&last_bg, &back->bg, sizeof(tunix_style_color)) != 0) {
                char bg_buf[32];
                write_color_code(bg_buf, sizeof(bg_buf), back->bg, true);
                size_t bg_len = strlen(bg_buf);
                if (output_pos + bg_len < sizeof(output_buffer)) {
                    memcpy(output_buffer + output_pos, bg_buf, bg_len);
                    output_pos += bg_len;
                }
                last_bg = back->bg;
            }
            
            /* Write character */
            size_t ch_len = strlen(back->ch);
            if (output_pos + ch_len < sizeof(output_buffer)) {
                memcpy(output_buffer + output_pos, back->ch, ch_len);
                output_pos += ch_len;
            }
            
            /* Update front buffer */
            *front = *back;
            first_cell = false;
            
            /* Flush if buffer is getting full */
            if (output_pos > sizeof(output_buffer) - 256) {
                tunix_platform_write(output_buffer, output_pos);
                output_pos = 0;
            }
        }
    }
    
    /* Flush remaining output */
    if (output_pos > 0) {
        tunix_platform_write(output_buffer, output_pos);
    }
    
    tunix_platform_flush();
}

tunix_style_color tunix_color_from_type(tunix_color_type type) {
    tunix_style_color color;
    color.type = type;
    color.rgb.r = 0;
    color.rgb.g = 0;
    color.rgb.b = 0;
    return color;
}

tunix_style_color tunix_color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    tunix_style_color color;
    color.type = TUNIX_COLOR_RGB;
    color.rgb.r = r;
    color.rgb.g = g;
    color.rgb.b = b;
    return color;
}

tunix_style_color tunix_color_interpolate(tunix_style_color start, tunix_style_color end, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    
    tunix_style_color result;
    result.type = TUNIX_COLOR_RGB;
    
    uint8_t sr = start.type == TUNIX_COLOR_RGB ? start.rgb.r : 128;
    uint8_t sg = start.type == TUNIX_COLOR_RGB ? start.rgb.g : 128;
    uint8_t sb = start.type == TUNIX_COLOR_RGB ? start.rgb.b : 128;
    
    uint8_t er = end.type == TUNIX_COLOR_RGB ? end.rgb.r : 255;
    uint8_t eg = end.type == TUNIX_COLOR_RGB ? end.rgb.g : 255;
    uint8_t eb = end.type == TUNIX_COLOR_RGB ? end.rgb.b : 255;
    
    result.rgb.r = (uint8_t)(sr + (er - sr) * t);
    result.rgb.g = (uint8_t)(sg + (eg - sg) * t);
    result.rgb.b = (uint8_t)(sb + (eb - sb) * t);
    
    return result;
}

void tunix_renderer_draw_gradient_rect(tunix_renderer *renderer, tunix_rect rect, 
                                       tunix_style_color start, tunix_style_color end, bool vertical) {
    if (!renderer) return;
    
    if (vertical) {
        for (int y = 0; y < rect.height; y++) {
            float t = rect.height > 1 ? (float)y / (float)(rect.height - 1) : 0.0f;
            tunix_style_color color = tunix_color_interpolate(start, end, t);
            tunix_renderer_draw_hline(renderer, rect.x, rect.y + y, rect.width, " ", color, color);
        }
    } else {
        for (int x = 0; x < rect.width; x++) {
            float t = rect.width > 1 ? (float)x / (float)(rect.width - 1) : 0.0f;
            tunix_style_color color = tunix_color_interpolate(start, end, t);
            tunix_renderer_draw_vline(renderer, rect.x + x, rect.y, rect.height, " ", color, color);
        }
    }
}

void tunix_renderer_draw_gradient_text(tunix_renderer *renderer, int x, int y, 
                                       const char *text, tunix_style_color start, tunix_style_color end) {
    if (!renderer || !text) return;
    
    size_t len = strlen(text);
    if (len == 0) return;
    
    tunix_style_color bg = tunix_color_from_type(TUNIX_COLOR_DEFAULT);
    
    for (size_t i = 0; i < len; i++) {
        float t = len > 1 ? (float)i / (float)(len - 1) : 0.0f;
        tunix_style_color color = tunix_color_interpolate(start, end, t);
        char ch[2] = {text[i], '\0'};
        tunix_renderer_set_cell(renderer, x + (int)i, y, ch, color, bg);
    }
}

void tunix_renderer_draw_circle(tunix_renderer *renderer, int cx, int cy, int radius, 
                                const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !ch) return;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                tunix_renderer_set_cell(renderer, cx + x, cy + y, ch, fg, bg);
            }
        }
    }
}

void tunix_renderer_draw_line(tunix_renderer *renderer, int x1, int y1, int x2, int y2, 
                              const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !ch) return;
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    
    while (true) {
        tunix_renderer_set_cell(renderer, x, y, ch, fg, bg);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void tunix_renderer_fill_rect(tunix_renderer *renderer, tunix_rect rect, 
                              const char *ch, tunix_style_color fg, tunix_style_color bg) {
    if (!renderer || !ch) return;
    
    for (int y = 0; y < rect.height; y++) {
        for (int x = 0; x < rect.width; x++) {
            tunix_renderer_set_cell(renderer, rect.x + x, rect.y + y, ch, fg, bg);
        }
    }
}

void tunix_renderer_draw_shadow(tunix_renderer *renderer, tunix_rect rect, int offset) {
    if (!renderer) return;
    
    tunix_style_color shadow = tunix_color_from_rgb(0, 0, 0);
    
    /* Draw shadow on right side */
    for (int y = offset; y < rect.height + offset; y++) {
        for (int x = 0; x < offset; x++) {
            tunix_renderer_set_cell(renderer, rect.x + rect.width + x, rect.y + y, " ", shadow, shadow);
        }
    }
    
    /* Draw shadow on bottom */
    for (int y = 0; y < offset; y++) {
        for (int x = offset; x < rect.width + offset; x++) {
            tunix_renderer_set_cell(renderer, rect.x + x, rect.y + rect.height + y, " ", shadow, shadow);
        }
    }
}

void tunix_renderer_draw_glow(tunix_renderer *renderer, int x, int y, int radius, tunix_style_color color) {
    if (!renderer) return;
    
    tunix_style_color dark = tunix_color_from_rgb(0, 0, 0);
    
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int dist_sq = dx*dx + dy*dy;
            if (dist_sq <= radius*radius) {
                float t = 1.0f - ((float)dist_sq / (float)(radius*radius));
                tunix_style_color glow_color = tunix_color_interpolate(dark, color, t);
                tunix_renderer_set_cell(renderer, x + dx, y + dy, " ", glow_color, glow_color);
            }
        }
    }
}
