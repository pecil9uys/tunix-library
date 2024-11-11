#ifndef TUNIX_TYPES_H
#define TUNIX_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Core types */
typedef struct tunix_rect {
    int x, y;
    int width, height;
} tunix_rect;

typedef struct tunix_point {
    int x, y;
} tunix_point;

typedef struct tunix_color {
    uint8_t r, g, b;
} tunix_color;

typedef enum tunix_color_type {
    TUNIX_COLOR_DEFAULT = 0,
    TUNIX_COLOR_BLACK,
    TUNIX_COLOR_RED,
    TUNIX_COLOR_GREEN,
    TUNIX_COLOR_YELLOW,
    TUNIX_COLOR_BLUE,
    TUNIX_COLOR_MAGENTA,
    TUNIX_COLOR_CYAN,
    TUNIX_COLOR_WHITE,
    TUNIX_COLOR_BRIGHT_BLACK,
    TUNIX_COLOR_BRIGHT_RED,
    TUNIX_COLOR_BRIGHT_GREEN,
    TUNIX_COLOR_BRIGHT_YELLOW,
    TUNIX_COLOR_BRIGHT_BLUE,
    TUNIX_COLOR_BRIGHT_MAGENTA,
    TUNIX_COLOR_BRIGHT_CYAN,
    TUNIX_COLOR_BRIGHT_WHITE,
    TUNIX_COLOR_RGB
} tunix_color_type;

typedef struct tunix_style_color {
    tunix_color_type type;
    tunix_color rgb;
} tunix_style_color;

typedef enum tunix_border_style {
    TUNIX_BORDER_NONE = 0,
    TUNIX_BORDER_SINGLE,
    TUNIX_BORDER_DOUBLE,
    TUNIX_BORDER_ROUNDED,
    TUNIX_BORDER_THICK,
    TUNIX_BORDER_CUSTOM
} tunix_border_style;

typedef struct tunix_border_chars {
    char top_left[8];
    char top_right[8];
    char bottom_left[8];
    char bottom_right[8];
    char horizontal[8];
    char vertical[8];
} tunix_border_chars;

typedef struct tunix_theme {
    tunix_style_color fg;
    tunix_style_color bg;
    tunix_style_color border_color;
    tunix_border_style border;
    tunix_border_chars border_chars;
    int padding_top, padding_right, padding_bottom, padding_left;
    int margin_top, margin_right, margin_bottom, margin_left;
} tunix_theme;

typedef enum tunix_align {
    TUNIX_ALIGN_LEFT = 0,
    TUNIX_ALIGN_CENTER,
    TUNIX_ALIGN_RIGHT,
    TUNIX_ALIGN_TOP,
    TUNIX_ALIGN_MIDDLE,
    TUNIX_ALIGN_BOTTOM
} tunix_align;

/* Spinner styles */
typedef enum tunix_spinner_style {
    TUNIX_SPINNER_DOTS = 0,      /* ⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏ */
    TUNIX_SPINNER_LINE,          /* -\|/ */
    TUNIX_SPINNER_CIRCLE,        /* ◐◓◑◒ */
    TUNIX_SPINNER_ARROW,         /* ←↖↑↗→↘↓↙ */
    TUNIX_SPINNER_BOUNCE,        /* ⠁⠂⠄⠂ */
    TUNIX_SPINNER_CLOCK,         /* 🕐🕑🕒🕓🕔🕕🕖🕗🕘🕙🕚🕛 */
    TUNIX_SPINNER_DOTS_SIMPLE    /* . .. ... */
} tunix_spinner_style;

typedef enum tunix_key {
    TUNIX_KEY_NONE = 0,
    TUNIX_KEY_ENTER = 13,
    TUNIX_KEY_ESC = 27,
    TUNIX_KEY_SPACE = 32,
    TUNIX_KEY_TAB = 9,
    TUNIX_KEY_BACKSPACE = 127,
    TUNIX_KEY_ARROW_UP = 256,
    TUNIX_KEY_ARROW_DOWN,
    TUNIX_KEY_ARROW_LEFT,
    TUNIX_KEY_ARROW_RIGHT,
    TUNIX_KEY_HOME,
    TUNIX_KEY_END,
    TUNIX_KEY_PAGE_UP,
    TUNIX_KEY_PAGE_DOWN,
    TUNIX_KEY_DELETE,
    TUNIX_KEY_INSERT,
    TUNIX_KEY_F1, TUNIX_KEY_F2, TUNIX_KEY_F3, TUNIX_KEY_F4,
    TUNIX_KEY_F5, TUNIX_KEY_F6, TUNIX_KEY_F7, TUNIX_KEY_F8,
    TUNIX_KEY_F9, TUNIX_KEY_F10, TUNIX_KEY_F11, TUNIX_KEY_F12,
    TUNIX_KEY_CTRL_C = 3,
    TUNIX_KEY_CTRL_D = 4,
    TUNIX_KEY_CTRL_Z = 26
} tunix_key;

typedef enum tunix_event_type {
    TUNIX_EVENT_KEY,
    TUNIX_EVENT_RESIZE,
    TUNIX_EVENT_FOCUS,
    TUNIX_EVENT_TEXT,
    TUNIX_EVENT_MOUSE,
    TUNIX_EVENT_QUIT
} tunix_event_type;

typedef struct tunix_event {
    tunix_event_type type;
    union {
        struct {
            tunix_key key;
            char ch;
            bool ctrl, alt, shift;
        } key;
        struct {
            int width, height;
        } resize;
        struct {
            bool focused;
        } focus;
        struct {
            char text[32];
        } text;
        struct {
            int x, y;
            int button;
            bool pressed;
        } mouse;
    } data;
} tunix_event;

/* Default theme helper */
tunix_theme tunix_theme_default(void);

#ifdef __cplusplus
}
#endif

#endif /* TUNIX_TYPES_H */
