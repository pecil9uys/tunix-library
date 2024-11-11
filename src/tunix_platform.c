#include "tunix_platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <fcntl.h>
#endif

/* Platform state */
static struct {
    bool initialized;
    bool raw_mode;
#ifdef _WIN32
    HANDLE hStdin;
    HANDLE hStdout;
    DWORD original_in_mode;
    DWORD original_out_mode;
#else
    struct termios original_termios;
    int original_flags;
#endif
} platform_state = {0};

bool tunix_platform_init(void) {
    if (platform_state.initialized) {
        return true;
    }

#ifdef _WIN32
    platform_state.hStdin = GetStdHandle(STD_INPUT_HANDLE);
    platform_state.hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (platform_state.hStdin == INVALID_HANDLE_VALUE || 
        platform_state.hStdout == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    GetConsoleMode(platform_state.hStdin, &platform_state.original_in_mode);
    GetConsoleMode(platform_state.hStdout, &platform_state.original_out_mode);
#else
    platform_state.original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
#endif

    platform_state.initialized = true;
    return true;
}

void tunix_platform_shutdown(void) {
    if (!platform_state.initialized) {
        return;
    }

    tunix_platform_disable_raw_mode();
    tunix_platform_show_cursor();
    tunix_platform_exit_alt_screen();

#ifdef _WIN32
    SetConsoleMode(platform_state.hStdin, platform_state.original_in_mode);
    SetConsoleMode(platform_state.hStdout, platform_state.original_out_mode);
#else
    fcntl(STDIN_FILENO, F_SETFL, platform_state.original_flags);
#endif

    platform_state.initialized = false;
}

bool tunix_platform_enable_raw_mode(void) {
    if (platform_state.raw_mode) {
        return true;
    }

#ifdef _WIN32
    DWORD mode = 0;
    GetConsoleMode(platform_state.hStdin, &mode);
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    SetConsoleMode(platform_state.hStdin, mode);
    
    GetConsoleMode(platform_state.hStdout, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    SetConsoleMode(platform_state.hStdout, mode);
#else
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &platform_state.original_termios) == -1) {
        return false;
    }
    
    raw = platform_state.original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return false;
    }
#endif

    platform_state.raw_mode = true;
    return true;
}

bool tunix_platform_disable_raw_mode(void) {
    if (!platform_state.raw_mode) {
        return true;
    }

#ifdef _WIN32
    SetConsoleMode(platform_state.hStdin, platform_state.original_in_mode);
    SetConsoleMode(platform_state.hStdout, platform_state.original_out_mode);
#else
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &platform_state.original_termios) == -1) {
        return false;
    }
#endif

    platform_state.raw_mode = false;
    return true;
}

bool tunix_platform_get_terminal_size(int *width, int *height) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(platform_state.hStdout, &csbi)) {
        return false;
    }
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return false;
    }
    *width = ws.ws_col;
    *height = ws.ws_row;
#endif
    return true;
}

bool tunix_platform_poll_event(tunix_event *event, int timeout_ms) {
#ifdef _WIN32
    DWORD wait_result = WaitForSingleObject(platform_state.hStdin, timeout_ms);
    if (wait_result != WAIT_OBJECT_0) {
        return false;
    }
    
    INPUT_RECORD input_record;
    DWORD events_read;
    
    if (!ReadConsoleInput(platform_state.hStdin, &input_record, 1, &events_read) || events_read == 0) {
        return false;
    }
    
    if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
        event->type = TUNIX_EVENT_KEY;
        event->data.key.key = input_record.Event.KeyEvent.wVirtualKeyCode;
        event->data.key.ch = input_record.Event.KeyEvent.uChar.AsciiChar;
        event->data.key.ctrl = (input_record.Event.KeyEvent.dwControlKeyState & 
                               (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
        event->data.key.alt = (input_record.Event.KeyEvent.dwControlKeyState & 
                              (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
        event->data.key.shift = (input_record.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;
        
        /* Map virtual keys */
        switch (input_record.Event.KeyEvent.wVirtualKeyCode) {
            case VK_UP: event->data.key.key = TUNIX_KEY_ARROW_UP; break;
            case VK_DOWN: event->data.key.key = TUNIX_KEY_ARROW_DOWN; break;
            case VK_LEFT: event->data.key.key = TUNIX_KEY_ARROW_LEFT; break;
            case VK_RIGHT: event->data.key.key = TUNIX_KEY_ARROW_RIGHT; break;
            case VK_HOME: event->data.key.key = TUNIX_KEY_HOME; break;
            case VK_END: event->data.key.key = TUNIX_KEY_END; break;
            case VK_PRIOR: event->data.key.key = TUNIX_KEY_PAGE_UP; break;
            case VK_NEXT: event->data.key.key = TUNIX_KEY_PAGE_DOWN; break;
            case VK_DELETE: event->data.key.key = TUNIX_KEY_DELETE; break;
            case VK_INSERT: event->data.key.key = TUNIX_KEY_INSERT; break;
            case VK_F1: event->data.key.key = TUNIX_KEY_F1; break;
            case VK_F2: event->data.key.key = TUNIX_KEY_F2; break;
            case VK_F3: event->data.key.key = TUNIX_KEY_F3; break;
            case VK_F4: event->data.key.key = TUNIX_KEY_F4; break;
            case VK_F5: event->data.key.key = TUNIX_KEY_F5; break;
            case VK_F6: event->data.key.key = TUNIX_KEY_F6; break;
            case VK_F7: event->data.key.key = TUNIX_KEY_F7; break;
            case VK_F8: event->data.key.key = TUNIX_KEY_F8; break;
            case VK_F9: event->data.key.key = TUNIX_KEY_F9; break;
            case VK_F10: event->data.key.key = TUNIX_KEY_F10; break;
            case VK_F11: event->data.key.key = TUNIX_KEY_F11; break;
            case VK_F12: event->data.key.key = TUNIX_KEY_F12; break;
            case VK_RETURN: event->data.key.key = TUNIX_KEY_ENTER; break;
            case VK_ESCAPE: event->data.key.key = TUNIX_KEY_ESC; break;
            case VK_TAB: event->data.key.key = TUNIX_KEY_TAB; break;
            case VK_BACK: event->data.key.key = TUNIX_KEY_BACKSPACE; break;
            default:
                if (event->data.key.ch != 0) {
                    event->data.key.key = event->data.key.ch;
                }
                break;
        }
        
        return true;
    } else if (input_record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
        event->type = TUNIX_EVENT_RESIZE;
        event->data.resize.width = input_record.Event.WindowBufferSizeEvent.dwSize.X;
        event->data.resize.height = input_record.Event.WindowBufferSizeEvent.dwSize.Y;
        return true;
    }
    
    return false;
#else
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    if (result <= 0) {
        return false;
    }
    
    char buf[32];
    ssize_t nread = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (nread <= 0) {
        return false;
    }
    buf[nread] = '\0';
    
    event->type = TUNIX_EVENT_KEY;
    event->data.key.ctrl = false;
    event->data.key.alt = false;
    event->data.key.shift = false;
    
    /* Parse escape sequences */
    if (nread == 1) {
        event->data.key.ch = buf[0];
        event->data.key.key = buf[0];
        
        if (buf[0] == 27) {
            event->data.key.key = TUNIX_KEY_ESC;
        } else if (buf[0] == 127 || buf[0] == 8) {
            event->data.key.key = TUNIX_KEY_BACKSPACE;
        } else if (buf[0] == 9) {
            event->data.key.key = TUNIX_KEY_TAB;
        } else if (buf[0] == 13 || buf[0] == 10) {
            event->data.key.key = TUNIX_KEY_ENTER;
        } else if (buf[0] >= 1 && buf[0] <= 26) {
            event->data.key.ctrl = true;
            event->data.key.key = buf[0];
        }
    } else if (buf[0] == 27 && buf[1] == '[') {
        /* CSI sequence */
        switch (buf[2]) {
            case 'A': event->data.key.key = TUNIX_KEY_ARROW_UP; break;
            case 'B': event->data.key.key = TUNIX_KEY_ARROW_DOWN; break;
            case 'C': event->data.key.key = TUNIX_KEY_ARROW_RIGHT; break;
            case 'D': event->data.key.key = TUNIX_KEY_ARROW_LEFT; break;
            case 'H': event->data.key.key = TUNIX_KEY_HOME; break;
            case 'F': event->data.key.key = TUNIX_KEY_END; break;
            case '1': case '2': case '3': case '4': case '5': case '6':
                if (buf[3] == '~') {
                    switch (buf[2]) {
                        case '1': event->data.key.key = TUNIX_KEY_HOME; break;
                        case '2': event->data.key.key = TUNIX_KEY_INSERT; break;
                        case '3': event->data.key.key = TUNIX_KEY_DELETE; break;
                        case '4': event->data.key.key = TUNIX_KEY_END; break;
                        case '5': event->data.key.key = TUNIX_KEY_PAGE_UP; break;
                        case '6': event->data.key.key = TUNIX_KEY_PAGE_DOWN; break;
                    }
                } else if (buf[2] == '1' && nread > 3) {
                    /* F1-F4 */
                    if (buf[3] >= '1' && buf[3] <= '5' && buf[4] == '~') {
                        event->data.key.key = TUNIX_KEY_F1 + (buf[3] - '1');
                    }
                } else if (buf[2] >= '1' && buf[2] <= '2' && nread > 3) {
                    /* F5-F8 */
                    if (buf[3] >= '7' && buf[3] <= '9' && buf[4] == '~') {
                        event->data.key.key = TUNIX_KEY_F5 + (buf[3] - '7');
                    } else if (buf[2] == '2' && buf[3] >= '0' && buf[3] <= '4' && buf[4] == '~') {
                        event->data.key.key = TUNIX_KEY_F9 + (buf[3] - '0');
                    }
                }
                break;
        }
        event->data.key.ch = 0;
    }
    
    return true;
#endif
}

void tunix_platform_clear_screen(void) {
    tunix_platform_write("\033[2J", 4);
}

void tunix_platform_move_cursor(int x, int y) {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    tunix_platform_write(buf, len);
}

void tunix_platform_hide_cursor(void) {
    tunix_platform_write("\033[?25l", 6);
}

void tunix_platform_show_cursor(void) {
    tunix_platform_write("\033[?25h", 6);
}

void tunix_platform_write(const char *str, size_t len) {
#ifdef _WIN32
    DWORD written;
    WriteConsole(platform_state.hStdout, str, (DWORD)len, &written, NULL);
#else
    (void)write(STDOUT_FILENO, str, len);
#endif
}

void tunix_platform_flush(void) {
    fflush(stdout);
}

void tunix_platform_enter_alt_screen(void) {
    tunix_platform_write("\033[?1049h", 8);
}

void tunix_platform_exit_alt_screen(void) {
    tunix_platform_write("\033[?1049l", 8);
}

/* Advanced terminal control */
void tunix_platform_set_title(const char *title) {
#ifdef _WIN32
    SetConsoleTitleA(title);
#else
    char buf[512];
    int len = snprintf(buf, sizeof(buf), "\033]0;%s\007", title);
    if (len > 0 && len < (int)sizeof(buf)) {
        tunix_platform_write(buf, len);
    }
#endif
}

void tunix_platform_bell(void) {
    tunix_platform_write("\007", 1);
}

void tunix_platform_clear_line(void) {
    tunix_platform_write("\033[2K", 4);
}

void tunix_platform_clear_to_end_of_line(void) {
    tunix_platform_write("\033[K", 3);
}

void tunix_platform_clear_to_start_of_line(void) {
    tunix_platform_write("\033[1K", 4);
}

void tunix_platform_save_cursor(void) {
    tunix_platform_write("\033[s", 3);
}

void tunix_platform_restore_cursor(void) {
    tunix_platform_write("\033[u", 3);
}

void tunix_platform_set_cursor_style(tunix_cursor_style style) {
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "\033[%d q", style);
    if (len > 0 && len < (int)sizeof(buf)) {
        tunix_platform_write(buf, len);
    }
}

/* Terminal detection */
tunix_terminal_type tunix_platform_detect_terminal(void) {
#ifdef _WIN32
    const char* wt_session = getenv("WT_SESSION");
    const char* term_program = getenv("TERM_PROGRAM");
    
    if (wt_session != NULL) {
        return TUNIX_TERM_WINDOWS_TERMINAL;
    }
    
    if (term_program != NULL) {
        if (strstr(term_program, "vscode")) {
            return TUNIX_TERM_XTERM;
        }
    }
    
    const char* prompt = getenv("PROMPT");
    if (prompt && strstr(prompt, "PS")) {
        return TUNIX_TERM_POWERSHELL;
    }
    
    return TUNIX_TERM_WINDOWS_CONSOLE;
#else
    const char* term = getenv("TERM");
    const char* term_program = getenv("TERM_PROGRAM");
    const char* tmux = getenv("TMUX");
    
    if (tmux != NULL) {
        return TUNIX_TERM_TMUX;
    }
    
    if (term_program != NULL) {
        if (strcmp(term_program, "iTerm.app") == 0) {
            return TUNIX_TERM_ITERM2;
        }
        if (strcmp(term_program, "gnome-terminal") == 0) {
            return TUNIX_TERM_GNOME;
        }
    }
    
    if (term != NULL) {
        if (strstr(term, "xterm")) {
            return TUNIX_TERM_XTERM;
        }
        if (strstr(term, "screen")) {
            return TUNIX_TERM_SCREEN;
        }
        if (strstr(term, "konsole")) {
            return TUNIX_TERM_KONSOLE;
        }
        if (strstr(term, "vt100")) {
            return TUNIX_TERM_VT100;
        }
    }
    
    return TUNIX_TERM_UNKNOWN;
#endif
}

const char* tunix_platform_get_terminal_name(tunix_terminal_type type) {
    switch (type) {
        case TUNIX_TERM_XTERM: return "XTerm";
        case TUNIX_TERM_VT100: return "VT100";
        case TUNIX_TERM_WINDOWS_CONSOLE: return "Windows Console";
        case TUNIX_TERM_WINDOWS_TERMINAL: return "Windows Terminal";
        case TUNIX_TERM_POWERSHELL: return "PowerShell";
        case TUNIX_TERM_CMD: return "Command Prompt";
        case TUNIX_TERM_GNOME: return "GNOME Terminal";
        case TUNIX_TERM_KONSOLE: return "Konsole";
        case TUNIX_TERM_ITERM2: return "iTerm2";
        case TUNIX_TERM_TMUX: return "tmux";
        case TUNIX_TERM_SCREEN: return "GNU Screen";
        default: return "Unknown";
    }
}

bool tunix_platform_supports_unicode(void) {
#ifdef _WIN32
    return GetConsoleOutputCP() == CP_UTF8;
#else
    const char* lang = getenv("LANG");
    const char* lc_all = getenv("LC_ALL");
    const char* locale = lc_all ? lc_all : lang;
    
    if (locale) {
        return (strstr(locale, "UTF-8") != NULL || 
                strstr(locale, "utf8") != NULL);
    }
    return false;
#endif
}

bool tunix_platform_supports_truecolor(void) {
    tunix_terminal_type term = tunix_platform_detect_terminal();
    
    switch (term) {
        case TUNIX_TERM_WINDOWS_TERMINAL:
        case TUNIX_TERM_ITERM2:
        case TUNIX_TERM_GNOME:
        case TUNIX_TERM_KONSOLE:
            return true;
        case TUNIX_TERM_XTERM:
        case TUNIX_TERM_TMUX:
        case TUNIX_TERM_SCREEN: {
            const char* colorterm = getenv("COLORTERM");
            return colorterm && (strcmp(colorterm, "truecolor") == 0 || 
                                strcmp(colorterm, "24bit") == 0);
        }
        default:
            return false;
    }
}

bool tunix_platform_supports_mouse(void) {
    tunix_terminal_type term = tunix_platform_detect_terminal();
    
    switch (term) {
        case TUNIX_TERM_XTERM:
        case TUNIX_TERM_WINDOWS_TERMINAL:
        case TUNIX_TERM_GNOME:
        case TUNIX_TERM_KONSOLE:
        case TUNIX_TERM_ITERM2:
        case TUNIX_TERM_TMUX:
            return true;
        default:
            return false;
    }
}

/* External terminal support */
bool tunix_platform_can_spawn_terminal(void) {
#ifdef _WIN32
    return true;
#else
    return (getenv("DISPLAY") != NULL) || (getenv("WAYLAND_DISPLAY") != NULL);
#endif
}

bool tunix_platform_spawn_terminal(const char *command) {
#ifdef _WIN32
    char cmd_buf[1024];
    
    /* Try Windows Terminal first (best Unicode support) */
    snprintf(cmd_buf, sizeof(cmd_buf), "wt.exe -w 0 new-tab cmd /k \"%s\"", command);
    if (system(cmd_buf) == 0) {
        return true;
    }
    
    /* Fallback to PowerShell */
    snprintf(cmd_buf, sizeof(cmd_buf), "start powershell -NoExit -Command \"%s\"", command);
    if (system(cmd_buf) == 0) {
        return true;
    }
    
    /* Final fallback to cmd */
    snprintf(cmd_buf, sizeof(cmd_buf), "start cmd /k \"%s\"", command);
    return system(cmd_buf) == 0;
#else
    const char* term_exec = tunix_platform_get_terminal_executable();
    if (!term_exec) {
        return false;
    }
    
    char cmd_buf[1024];
    snprintf(cmd_buf, sizeof(cmd_buf), "%s -e \"%s\" &", term_exec, command);
    return system(cmd_buf) == 0;
#endif
}

const char* tunix_platform_get_terminal_executable(void) {
#ifdef _WIN32
    return "cmd.exe";
#else
    const char* terminals[] = {
        "x-terminal-emulator",
        "gnome-terminal",
        "konsole",
        "xfce4-terminal",
        "mate-terminal",
        "xterm",
        "urxvt",
        "rxvt",
        "terminator",
        "alacritty",
        "kitty",
        NULL
    };
    
    for (int i = 0; terminals[i] != NULL; i++) {
        char path_buf[256];
        snprintf(path_buf, sizeof(path_buf), "which %s > /dev/null 2>&1", terminals[i]);
        if (system(path_buf) == 0) {
            return terminals[i];
        }
    }
    
    return NULL;
#endif
}
