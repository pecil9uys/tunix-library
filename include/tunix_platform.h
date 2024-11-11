#ifndef TUNIX_PLATFORM_H
#define TUNIX_PLATFORM_H

#include "tunix_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Platform initialization */
bool tunix_platform_init(void);
void tunix_platform_shutdown(void);

/* Terminal control */
bool tunix_platform_enable_raw_mode(void);
bool tunix_platform_disable_raw_mode(void);
bool tunix_platform_get_terminal_size(int *width, int *height);

/* Input handling */
bool tunix_platform_poll_event(tunix_event *event, int timeout_ms);

/* Output control */
void tunix_platform_clear_screen(void);
void tunix_platform_move_cursor(int x, int y);
void tunix_platform_hide_cursor(void);
void tunix_platform_show_cursor(void);
void tunix_platform_write(const char *str, size_t len);
void tunix_platform_flush(void);

/* Alternative screen buffer */
void tunix_platform_enter_alt_screen(void);
void tunix_platform_exit_alt_screen(void);

/* Advanced terminal control */
void tunix_platform_set_title(const char *title);
void tunix_platform_bell(void);
void tunix_platform_clear_line(void);
void tunix_platform_clear_to_end_of_line(void);
void tunix_platform_clear_to_start_of_line(void);
void tunix_platform_save_cursor(void);
void tunix_platform_restore_cursor(void);

/* Cursor styles */
typedef enum {
    TUNIX_CURSOR_BLOCK_BLINKING = 0,
    TUNIX_CURSOR_BLOCK_STEADY = 1,
    TUNIX_CURSOR_UNDERLINE_BLINKING = 2,
    TUNIX_CURSOR_UNDERLINE_STEADY = 3,
    TUNIX_CURSOR_BAR_BLINKING = 4,
    TUNIX_CURSOR_BAR_STEADY = 5
} tunix_cursor_style;

void tunix_platform_set_cursor_style(tunix_cursor_style style);

/* Terminal capabilities detection */
typedef enum {
    TUNIX_TERM_UNKNOWN = 0,
    TUNIX_TERM_XTERM,
    TUNIX_TERM_VT100,
    TUNIX_TERM_WINDOWS_CONSOLE,
    TUNIX_TERM_WINDOWS_TERMINAL,
    TUNIX_TERM_POWERSHELL,
    TUNIX_TERM_CMD,
    TUNIX_TERM_GNOME,
    TUNIX_TERM_KONSOLE,
    TUNIX_TERM_ITERM2,
    TUNIX_TERM_TMUX,
    TUNIX_TERM_SCREEN
} tunix_terminal_type;

tunix_terminal_type tunix_platform_detect_terminal(void);
const char* tunix_platform_get_terminal_name(tunix_terminal_type type);
bool tunix_platform_supports_unicode(void);
bool tunix_platform_supports_truecolor(void);
bool tunix_platform_supports_mouse(void);

/* External terminal support */
bool tunix_platform_can_spawn_terminal(void);
bool tunix_platform_spawn_terminal(const char *command);
const char* tunix_platform_get_terminal_executable(void);

#ifdef __cplusplus
}
#endif

#endif /* TUNIX_PLATFORM_H */
