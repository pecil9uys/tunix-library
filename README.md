# Tunix - Terminal UI Framework

A powerful, cross-platform terminal UI framework written in pure C11 with an optional modern C++20 wrapper.

## Features

### ✨ Core Engine
- Pure C11 core library (zero dependencies)
- Cross-platform: Linux, macOS, Windows
- Raw mode input handling (POSIX + WinAPI)
- Double-buffered rendering (flicker-free)
- ANSI escape sequence control
- Complete event system (keyboard, resize, focus)
- Customizable themes and colors

### 🖥️ Advanced Terminal Controls
- Terminal type detection (12+ terminal emulators)
- Cursor style control (block, underline, bar, blinking variants)
- Terminal capability checking (truecolor, 256-color, 16-color)
- External terminal spawning (Windows Terminal, cmd, bash)
- Window title setting
- Line clearing modes
- Bell/beep notifications

### 🎨 Complete Widget Set
- **Text**: Label, Button
- **Input**: Textbox, Editor (multi-line)
- **Selection**: Checkbox, Radio, Dropdown, List
- **Display**: Progress Bar, Slider, Table, **Spinner (NEW!)**
- **Navigation**: Menu, Tabs, Status Bar
- **Layout**: Panel, VStack, HStack, Grid
- **Visual**: Separator, Notification, Window

### 🌀 Spinner Widget 
7 animation styles with ASCII-safe characters:
- **DOTS**: `| / - \` (8-frame rotation)
- **LINE**: `- \ | /` (4-frame rotation)
- **CIRCLE**: `O o . o` (pulsing effect)
- **ARROW**: `< ^ > v` (directional rotation)
- **BOUNCE**: Bouncing dot animation (6 frames)
- **CLOCK**: Clock-hand rotation (4 frames)
- **DOTS_SIMPLE**: `. .. ...` (simple dots)

### 🎨 Advanced Rendering
- Gradient colors (horizontal & vertical)
- Color interpolation (RGB blending)
- Visual effects (shadows, glows)
- Custom drawing primitives:
  - Circles (Bresenham algorithm)
  - Lines (with thickness)
  - Rectangles (filled & outlined)

### 🎯 Customization
Every widget supports:
- Custom colors (foreground, background, border)
- Custom border styles (single, double, rounded, thick)
- Custom padding and margins
- Custom callbacks and event handlers
- Custom rendering functions
- Custom themes (per-widget or global)

### 🔧 Dual API
- **C API**: Low-level, explicit, full control
- **C++ API**: High-level, fluent, modern convenience

## Terminal Compatibility

### Automatic ASCII Fallback
Tunix automatically detects terminal Unicode support and switches rendering modes:

**ASCII Mode** (auto-enabled for non-Unicode terminals):
- Box borders: `+ - | + - |` instead of `┌ ─ │ ┐ ─ │`
- Sliders: `O` and `-` instead of `●` and `─`
- Separators: `-` and `|` instead of `─` and `│`

**Unicode Mode** (auto-enabled when supported):
- Extended box-drawing characters
- Special symbols and effects
- Enhanced visual appearance

### Tested Terminals

**Full Unicode Support:**
- ✅ Windows Terminal
- ✅ PowerShell Core 7+
- ✅ Linux terminals (XTerm, GNOME Terminal, Konsole)
- ✅ macOS Terminal.app & iTerm2
- ✅ Multiplexers (tmux, screen)

**ASCII Fallback:**
- ✅ Windows Console (legacy)
- ✅ Windows PowerShell 5.1
- ✅ Windows cmd.exe
- ✅ Basic SSH clients

### Running in Windows Terminal
For best visual experience on Windows, use the provided launcher:
```batch
run_in_terminal.bat spinner_demo
run_in_terminal.bat dashboard_demo
```

This automatically opens the demo in Windows Terminal if available, or falls back to PowerShell.

## Quick Start

### C API Example
```c
#include "tunix.h"

void on_click(tunix_widget *widget, void *data) {
    tunix_app *app = (tunix_app*)data;
    tunix_app_quit(app);
}

int main(void) {
    tunix_app *app = tunix_app_create("My App");
    tunix_widget *root = tunix_app_root(app);
    
    tunix_widget *button = tunix_button_create("Click Me");
    tunix_widget_set_on_click(button, on_click, app);
    tunix_widget_add_child(root, button);
    
    tunix_app_run(app);
    tunix_app_destroy(app);
    return 0;
}
```

### C++ API Example
```cpp
#include "tunix.hpp"

int main() {
    tunix::App app("My App");
    
    auto button = std::make_shared<tunix::Button>("Click Me");
    button->on_click([&app]() { app.quit(); });
    
    app.root().add(button);
    return app.run() ? 0 : 1;
}
```

## Building

### Requirements
- C11 compiler (GCC, Clang, MSVC)
- C++20 compiler (for C++ wrapper)
- [xmake](https://xmake.io) OR [CMake](https://cmake.org) 3.15+

### Build with xmake
```bash
# Build everything
xmake

# Build specific target
xmake build tunix             # C library only
xmake build demo_c            # C example
xmake build demo_cpp          # C++ example
xmake build spinner_demo      # Spinner animation demo
xmake build advanced_demo     # Terminal controls demo
xmake build form_demo         # Form with validation
xmake build dashboard_demo    # System monitoring dashboard

# Run examples
xmake run demo_c
xmake run spinner_demo
xmake run advanced_demo

# Clean
xmake clean
```

### Build with CMake
```bash
# Configure and build
cmake -B build
cmake --build build

# Run examples
./build/demo_c
./build/spinner_demo
./build/advanced_demo
```

### Supported Platforms
- **Windows**: MSVC 2022, MinGW, Clang
- **Linux**: GCC 9+, Clang 10+
- **macOS**: Apple Clang (Xcode 12+)

## Project Structure
```
tunix/
├── include/          # C header files
│   ├── tunix.h
│   ├── tunix_types.h
│   ├── tunix_platform.h
│   ├── tunix_renderer.h
│   └── tunix_widget.h
|   ...
├── src/              # C implementation
│   ├── tunix_types.c
│   ├── tunix_platform.c
│   ├── tunix_renderer.c
│   └── tunix_widget.c
├── examples/         # Demo applications
│   ├── demo_c.c
│   └── demo_cpp.cpp
|    ...
└── xmake.lua        # Build configuration
```

## Widgets Reference

### Label
```c
tunix_widget *label = tunix_label_create("Hello");
tunix_label_set_text(label, "World");
```

### Button
```c
tunix_widget *btn = tunix_button_create("Click");
tunix_widget_set_on_click(btn, callback, data);
```

### Checkbox
```c
tunix_widget *check = tunix_checkbox_create("Accept", false);
bool checked = tunix_checkbox_get_checked(check);
```

### List
```c
const char *items[] = {"A", "B", "C"};
tunix_widget *list = tunix_list_create(items, 3);
int selected = tunix_list_get_selected(list);
```

### Progress Bar
```c
tunix_widget *progress = tunix_progress_create(0.5f);
tunix_progress_set_value(progress, 0.75f);
```

### Textbox
```c
tunix_widget *textbox = tunix_textbox_create("Default");
const char *text = tunix_textbox_get_text(textbox);
```

### Editor (Multi-line)
```c
tunix_widget *editor = tunix_editor_create("Line 1");
tunix_editor_set_text(editor, "New content");
```

### Spinner
```c
tunix_widget *spinner = tunix_spinner_create(TUNIX_SPINNER_DOTS);
tunix_spinner_start(spinner);

/* In your render loop */
tunix_spinner_tick(spinner);  /* Advance to next frame */

/* Change style dynamically */
tunix_spinner_set_style(spinner, TUNIX_SPINNER_CIRCLE);

/* Stop animation */
tunix_spinner_stop(spinner);
```

**Available Styles:**
- `TUNIX_SPINNER_DOTS` - Rotating line: `| / - \`
- `TUNIX_SPINNER_LINE` - Simple rotation
- `TUNIX_SPINNER_CIRCLE` - Pulsing circle
- `TUNIX_SPINNER_ARROW` - Directional arrows
- `TUNIX_SPINNER_BOUNCE` - Bouncing effect
- `TUNIX_SPINNER_CLOCK` - Clock hands
- `TUNIX_SPINNER_DOTS_SIMPLE` - Growing dots

## Advanced Features

### Terminal Detection
```c
tunix_terminal_type type = tunix_platform_get_terminal_type();

if (type == TUNIX_TERMINAL_WINDOWS_TERMINAL) {
    /* Use enhanced features */
}
```

**Detected Terminals:**
Windows Terminal, PowerShell, cmd, XTerm, GNOME Terminal, Konsole, iTerm2, Terminal.app, tmux, screen, Alacritty, Kitty, Terminator

### Cursor Styles
```c
tunix_platform_set_cursor_style(TUNIX_CURSOR_BLOCK);
tunix_platform_set_cursor_style(TUNIX_CURSOR_UNDERLINE);
tunix_platform_set_cursor_style(TUNIX_CURSOR_BAR);
tunix_platform_set_cursor_style(TUNIX_CURSOR_BLOCK_BLINK);
tunix_platform_set_cursor_style(TUNIX_CURSOR_UNDERLINE_BLINK);
tunix_platform_set_cursor_style(TUNIX_CURSOR_BAR_BLINK);
```

### Terminal Capabilities
```c
bool has_truecolor = tunix_platform_supports_truecolor();
bool has_256color = tunix_platform_supports_256color();
bool has_unicode = tunix_platform_supports_unicode();
```

### Gradient Rendering
```c
tunix_style_color start = {255, 0, 0, 255};  /* Red */
tunix_style_color end = {0, 0, 255, 255};    /* Blue */

/* Horizontal gradient */
tunix_renderer_draw_gradient_h(renderer, x, y, width, height, start, end);

/* Vertical gradient */
tunix_renderer_draw_gradient_v(renderer, x, y, width, height, start, end);
```

### Custom Drawing
```c
/* Draw circle */
tunix_renderer_draw_circle(renderer, center_x, center_y, radius, "O", fg, bg);

/* Draw line */
tunix_renderer_draw_line(renderer, x1, y1, x2, y2, "-", fg, bg);

/* Draw rectangle */
tunix_renderer_draw_rect(renderer, x, y, width, height, "+", fg, bg, true);
```

### Visual Effects
```c
/* Add shadow effect */
tunix_renderer_draw_shadow(renderer, x, y, width, height, intensity);

/* Add glow effect */
tunix_style_color glow_color = {0, 255, 255, 255};
tunix_renderer_draw_glow(renderer, x, y, width, height, glow_color, intensity);
```

## Theming

### Set Colors
```c
widget->theme.fg = tunix_color_from_type(TUNIX_COLOR_WHITE);
widget->theme.bg = tunix_color_from_rgb(50, 50, 50);
widget->theme.border_color = tunix_color_from_type(TUNIX_COLOR_BLUE);
```

### Set Border Style
```c
widget->theme.border = TUNIX_BORDER_ROUNDED;
// Options: SINGLE, DOUBLE, ROUNDED, THICK, CUSTOM, NONE
```

### Set Padding/Margin
```c
widget->theme.padding_top = 1;
widget->theme.padding_left = 2;
widget->theme.margin_top = 1;
```

## Event Handling

### Keyboard Events
```c
tunix_event event;
if (event.type == TUNIX_EVENT_KEY) {
    tunix_key key = event.data.key.key;
    char ch = event.data.key.ch;
    bool ctrl = event.data.key.ctrl;
}
```

### Widget Callbacks
```c
tunix_widget_set_on_click(widget, click_callback, data);
tunix_widget_set_on_change(widget, change_callback, data);
tunix_widget_set_on_focus(widget, focus_callback, data);
```

## License

this Project is licensed under the MIT License - See [LICENSE](LICENSE) file for details.

## Contributing

Contributions welcome! This is a complete, production-ready framework.
