#include "tunix_widget.h"
#include "tunix_platform.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Portable strdup */
#ifdef _MSC_VER
#define tunix_strdup _strdup
#else
#define tunix_strdup strdup
#endif

/* Widget-specific data structures */
typedef struct {
    char *text;
} label_data;

typedef struct {
    char *text;
} button_data;

typedef struct {
    char *text;
    bool checked;
} checkbox_data;

typedef struct {
    char *text;
    int group_id;
    bool selected;
} radio_data;

typedef struct {
    float value;
} progress_data;

typedef struct {
    float min;
    float max;
    float value;
} slider_data;

typedef struct {
    char *text;
    int cursor_pos;
} textbox_data;

typedef struct {
    char **lines;
    int line_count;
    int cursor_row;
    int cursor_col;
    int scroll_offset;
} editor_data;

typedef struct {
    char **items;
    int item_count;
    int selected_index;
    int scroll_offset;
} list_data;

typedef struct {
    char **items;
    int item_count;
    int selected_index;
    bool expanded;
} dropdown_data;

typedef struct {
    char **items;
    int item_count;
    int selected_index;
} menu_data;

typedef struct {
    char **tab_names;
    int tab_count;
    int active_tab;
} tabs_data;

typedef struct {
    char ***cells;
    int rows;
    int cols;
} table_data;

typedef struct {
    char *text;
} statusbar_data;

typedef struct {
    char *text;
    int timeout;
} notification_data;

typedef struct {
    bool horizontal;
} separator_data;

typedef struct {
    tunix_spinner_style style;
    int frame;
    int frame_count;
    bool running;
    const char **frames;
} spinner_data;

typedef struct {
    char *title;
} window_data;

/* Application structure */
struct tunix_app {
    char *title;
    tunix_widget *root;
    tunix_renderer *renderer;
    tunix_theme global_theme;
    bool running;
    bool needs_redraw;
};

/* Application functions */
tunix_app* tunix_app_create(const char *title) {
    tunix_app *app = (tunix_app*)malloc(sizeof(tunix_app));
    if (!app) {
        return NULL;
    }
    
    app->title = title ? tunix_strdup(title) : tunix_strdup("Tunix App");
    app->root = tunix_window_create(app->title);
    app->renderer = NULL;
    app->global_theme = tunix_theme_default();
    app->running = false;
    app->needs_redraw = true;
    
    return app;
}

void tunix_app_destroy(tunix_app *app) {
    if (!app) {
        return;
    }
    
    tunix_widget_destroy(app->root);
    tunix_renderer_destroy(app->renderer);
    free(app->title);
    free(app);
}

bool tunix_app_run(tunix_app *app) {
    if (!app) {
        return false;
    }
    
    if (!tunix_platform_init()) {
        return false;
    }
    
    tunix_platform_enable_raw_mode();
    tunix_platform_hide_cursor();
    tunix_platform_enter_alt_screen();
    
    int width, height;
    tunix_platform_get_terminal_size(&width, &height);
    
    app->renderer = tunix_renderer_create(width, height);
    if (!app->renderer) {
        tunix_platform_shutdown();
        return false;
    }
    
    app->root->bounds.x = 0;
    app->root->bounds.y = 0;
    app->root->bounds.width = width;
    app->root->bounds.height = height;
    
    app->running = true;
    
    while (app->running) {
        tunix_event event;
        
        if (tunix_platform_poll_event(&event, 16)) {
            if (event.type == TUNIX_EVENT_KEY && event.data.key.key == TUNIX_KEY_ESC) {
                app->running = false;
            } else if (event.type == TUNIX_EVENT_RESIZE) {
                tunix_renderer_resize(app->renderer, event.data.resize.width, event.data.resize.height);
                app->root->bounds.width = event.data.resize.width;
                app->root->bounds.height = event.data.resize.height;
                app->needs_redraw = true;
            } else {
                if (tunix_widget_handle_event(app->root, &event)) {
                    app->needs_redraw = true;
                }
            }
        }
        
        if (app->needs_redraw) {
            tunix_renderer_clear(app->renderer, app->global_theme.bg);
            tunix_widget_render(app->root, app->renderer);
            tunix_renderer_present(app->renderer);
            app->needs_redraw = false;
        }
    }
    
    tunix_platform_shutdown();
    return true;
}

void tunix_app_quit(tunix_app *app) {
    if (app) {
        app->running = false;
    }
}

tunix_widget* tunix_app_root(tunix_app *app) {
    return app ? app->root : NULL;
}

void tunix_app_set_theme(tunix_app *app, tunix_theme theme) {
    if (app) {
        app->global_theme = theme;
        app->needs_redraw = true;
    }
}

/* Widget base functions */
tunix_widget* tunix_widget_create(tunix_widget_type type) {
    tunix_widget *widget = (tunix_widget*)calloc(1, sizeof(tunix_widget));
    if (!widget) {
        return NULL;
    }
    
    widget->type = type;
    widget->id = NULL;
    widget->bounds.x = 0;
    widget->bounds.y = 0;
    widget->bounds.width = 10;
    widget->bounds.height = 3;
    widget->visible = true;
    widget->enabled = true;
    widget->focusable = false;
    widget->focused = false;
    widget->theme = tunix_theme_default();
    widget->children = NULL;
    widget->child_count = 0;
    widget->child_capacity = 0;
    widget->parent = NULL;
    widget->on_click = NULL;
    widget->on_change = NULL;
    widget->on_focus = NULL;
    widget->on_blur = NULL;
    widget->on_render = NULL;
    widget->user_data = NULL;
    widget->data = NULL;
    
    return widget;
}

void tunix_widget_destroy(tunix_widget *widget) {
    if (!widget) {
        return;
    }
    
    free(widget->id);
    
    for (int i = 0; i < widget->child_count; i++) {
        tunix_widget_destroy(widget->children[i]);
    }
    free(widget->children);
    
    /* Free widget-specific data */
    switch (widget->type) {
        case TUNIX_WIDGET_LABEL: {
            label_data *data = (label_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_BUTTON: {
            button_data *data = (button_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_CHECKBOX: {
            checkbox_data *data = (checkbox_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_RADIO: {
            radio_data *data = (radio_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_TEXTBOX: {
            textbox_data *data = (textbox_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_EDITOR: {
            editor_data *data = (editor_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->line_count; i++) {
                    free(data->lines[i]);
                }
                free(data->lines);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_LIST: {
            list_data *data = (list_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->item_count; i++) {
                    free(data->items[i]);
                }
                free(data->items);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_DROPDOWN: {
            dropdown_data *data = (dropdown_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->item_count; i++) {
                    free(data->items[i]);
                }
                free(data->items);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_MENU: {
            menu_data *data = (menu_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->item_count; i++) {
                    free(data->items[i]);
                }
                free(data->items);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_TABS: {
            tabs_data *data = (tabs_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->tab_count; i++) {
                    free(data->tab_names[i]);
                }
                free(data->tab_names);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_TABLE: {
            table_data *data = (table_data*)widget->data;
            if (data) {
                for (int i = 0; i < data->rows; i++) {
                    for (int j = 0; j < data->cols; j++) {
                        free(data->cells[i][j]);
                    }
                    free(data->cells[i]);
                }
                free(data->cells);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_STATUSBAR: {
            statusbar_data *data = (statusbar_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_NOTIFICATION: {
            notification_data *data = (notification_data*)widget->data;
            if (data) {
                free(data->text);
                free(data);
            }
            break;
        }
        case TUNIX_WIDGET_WINDOW: {
            window_data *data = (window_data*)widget->data;
            if (data) {
                free(data->title);
                free(data);
            }
            break;
        }
        default:
            free(widget->data);
            break;
    }
    
    free(widget);
}

void tunix_widget_add_child(tunix_widget *parent, tunix_widget *child) {
    if (!parent || !child) {
        return;
    }
    
    if (parent->child_count >= parent->child_capacity) {
        int new_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        tunix_widget **new_children = (tunix_widget**)realloc(parent->children, 
                                                               new_capacity * sizeof(tunix_widget*));
        if (!new_children) {
            return;
        }
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void tunix_widget_remove_child(tunix_widget *parent, tunix_widget *child) {
    if (!parent || !child) {
        return;
    }
    
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            child->parent = NULL;
            break;
        }
    }
}

tunix_widget* tunix_widget_find(tunix_widget *root, const char *id) {
    if (!root || !id) {
        return NULL;
    }
    
    if (root->id && strcmp(root->id, id) == 0) {
        return root;
    }
    
    for (int i = 0; i < root->child_count; i++) {
        tunix_widget *found = tunix_widget_find(root->children[i], id);
        if (found) {
            return found;
        }
    }
    
    return NULL;
}

void tunix_widget_set_id(tunix_widget *widget, const char *id) {
    if (!widget) {
        return;
    }
    
    free(widget->id);
    widget->id = id ? tunix_strdup(id) : NULL;
}

void tunix_widget_set_bounds(tunix_widget *widget, tunix_rect bounds) {
    if (widget) {
        widget->bounds = bounds;
    }
}

void tunix_widget_set_visible(tunix_widget *widget, bool visible) {
    if (widget) {
        widget->visible = visible;
    }
}

void tunix_widget_set_enabled(tunix_widget *widget, bool enabled) {
    if (widget) {
        widget->enabled = enabled;
    }
}

void tunix_widget_set_focusable(tunix_widget *widget, bool focusable) {
    if (widget) {
        widget->focusable = focusable;
    }
}

void tunix_widget_set_theme(tunix_widget *widget, tunix_theme theme) {
    if (widget) {
        widget->theme = theme;
    }
}

void tunix_widget_set_on_click(tunix_widget *widget, tunix_callback callback, void *user_data) {
    if (widget) {
        widget->on_click = callback;
        widget->user_data = user_data;
    }
}

void tunix_widget_set_on_change(tunix_widget *widget, tunix_callback callback, void *user_data) {
    if (widget) {
        widget->on_change = callback;
        widget->user_data = user_data;
    }
}

void tunix_widget_set_on_focus(tunix_widget *widget, tunix_callback callback, void *user_data) {
    if (widget) {
        widget->on_focus = callback;
        widget->user_data = user_data;
    }
}

void tunix_widget_set_on_blur(tunix_widget *widget, tunix_callback callback, void *user_data) {
    if (widget) {
        widget->on_blur = callback;
        widget->user_data = user_data;
    }
}

void tunix_widget_set_on_render(tunix_widget *widget, tunix_render_callback callback, void *user_data) {
    if (widget) {
        widget->on_render = callback;
        widget->user_data = user_data;
    }
}

void tunix_widget_focus(tunix_widget *widget) {
    if (!widget || !widget->focusable) {
        return;
    }
    
    tunix_widget *root = widget;
    while (root->parent) {
        root = root->parent;
    }
    
    tunix_widget *current = tunix_widget_get_focused(root);
    if (current && current != widget) {
        tunix_widget_blur(current);
    }
    
    widget->focused = true;
    if (widget->on_focus) {
        widget->on_focus(widget, widget->user_data);
    }
}

void tunix_widget_blur(tunix_widget *widget) {
    if (!widget) {
        return;
    }
    
    widget->focused = false;
    if (widget->on_blur) {
        widget->on_blur(widget, widget->user_data);
    }
}

tunix_widget* tunix_widget_get_focused(tunix_widget *root) {
    if (!root) {
        return NULL;
    }
    
    if (root->focused) {
        return root;
    }
    
    for (int i = 0; i < root->child_count; i++) {
        tunix_widget *focused = tunix_widget_get_focused(root->children[i]);
        if (focused) {
            return focused;
        }
    }
    
    return NULL;
}

/* Widget rendering */
void tunix_widget_render(tunix_widget *widget, tunix_renderer *renderer) {
    if (!widget || !renderer || !widget->visible) {
        return;
    }
    
    tunix_rect bounds = widget->bounds;
    tunix_style_color fg = widget->theme.fg;
    tunix_style_color bg = widget->theme.bg;
    tunix_style_color border_color = widget->theme.border_color;
    
    if (fg.type == TUNIX_COLOR_DEFAULT) {
        fg = tunix_color_from_type(TUNIX_COLOR_WHITE);
    }
    if (bg.type == TUNIX_COLOR_DEFAULT) {
        bg = tunix_color_from_type(TUNIX_COLOR_BLACK);
    }
    if (border_color.type == TUNIX_COLOR_DEFAULT) {
        border_color = tunix_color_from_type(TUNIX_COLOR_WHITE);
    }
    
    /* Custom render callback */
    if (widget->on_render) {
        widget->on_render(widget, renderer, widget->user_data);
        return;
    }
    
    /* Render based on widget type */
    switch (widget->type) {
        case TUNIX_WIDGET_WINDOW: {
            window_data *data = (window_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->title) {
                tunix_renderer_draw_text(renderer, bounds.x + 2, bounds.y, 
                                       data->title, fg, bg);
            }
            break;
        }
        
        case TUNIX_WIDGET_PANEL: {
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            break;
        }
        
        case TUNIX_WIDGET_LABEL: {
            label_data *data = (label_data*)widget->data;
            if (data && data->text) {
                tunix_renderer_draw_text(renderer, bounds.x, bounds.y, 
                                       data->text, fg, bg);
            }
            break;
        }
        
        case TUNIX_WIDGET_BUTTON: {
            button_data *data = (button_data*)widget->data;
            tunix_style_color btn_bg = widget->focused ? 
                tunix_color_from_type(TUNIX_COLOR_BLUE) : bg;
            tunix_renderer_draw_rect(renderer, bounds, fg, btn_bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->text) {
                int text_x = bounds.x + (bounds.width - (int)strlen(data->text)) / 2;
                int text_y = bounds.y + bounds.height / 2;
                tunix_renderer_draw_text(renderer, text_x, text_y, 
                                       data->text, fg, btn_bg);
            }
            break;
        }
        
        case TUNIX_WIDGET_CHECKBOX: {
            checkbox_data *data = (checkbox_data*)widget->data;
            char checkbox_str[64];
            snprintf(checkbox_str, sizeof(checkbox_str), "[%c] %s", 
                    data && data->checked ? 'X' : ' ', 
                    data && data->text ? data->text : "");
            tunix_renderer_draw_text(renderer, bounds.x, bounds.y, 
                                   checkbox_str, fg, bg);
            break;
        }
        
        case TUNIX_WIDGET_RADIO: {
            radio_data *data = (radio_data*)widget->data;
            char radio_str[64];
            snprintf(radio_str, sizeof(radio_str), "(%c) %s", 
                    data && data->selected ? '*' : ' ', 
                    data && data->text ? data->text : "");
            tunix_renderer_draw_text(renderer, bounds.x, bounds.y, 
                                   radio_str, fg, bg);
            break;
        }
        
        case TUNIX_WIDGET_PROGRESS: {
            progress_data *data = (progress_data*)widget->data;
            float value = data ? data->value : 0.0f;
            if (value < 0.0f) value = 0.0f;
            if (value > 1.0f) value = 1.0f;
            
            int filled_width = (int)(bounds.width * value);
            tunix_rect filled = {bounds.x, bounds.y, filled_width, bounds.height};
            tunix_rect empty = {bounds.x + filled_width, bounds.y, 
                              bounds.width - filled_width, bounds.height};
            
            tunix_renderer_draw_rect(renderer, filled, fg, 
                                   tunix_color_from_type(TUNIX_COLOR_GREEN));
            tunix_renderer_draw_rect(renderer, empty, fg, bg);
            break;
        }
        
        case TUNIX_WIDGET_SLIDER: {
            slider_data *data = (slider_data*)widget->data;
            if (data) {
                float normalized = (data->value - data->min) / (data->max - data->min);
                int slider_pos = (int)(bounds.width * normalized);
                bool use_unicode = tunix_platform_supports_unicode();
                
                for (int x = 0; x < bounds.width; x++) {
                    const char *ch;
                    if (x == slider_pos) {
                        ch = use_unicode ? "●" : "O";
                    } else {
                        ch = use_unicode ? "─" : "-";
                    }
                    tunix_renderer_set_cell(renderer, bounds.x + x, bounds.y, 
                                          ch, fg, bg);
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_TEXTBOX: {
            textbox_data *data = (textbox_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->text) {
                tunix_renderer_draw_text(renderer, bounds.x + 1, bounds.y + 1, 
                                       data->text, fg, bg);
            }
            break;
        }
        
        case TUNIX_WIDGET_EDITOR: {
            editor_data *data = (editor_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->lines) {
                int visible_lines = bounds.height - 2;
                for (int i = 0; i < visible_lines && i + data->scroll_offset < data->line_count; i++) {
                    tunix_renderer_draw_text(renderer, bounds.x + 1, bounds.y + 1 + i, 
                                           data->lines[i + data->scroll_offset], fg, bg);
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_LIST: {
            list_data *data = (list_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->items) {
                int visible_items = bounds.height - 2;
                for (int i = 0; i < visible_items && i + data->scroll_offset < data->item_count; i++) {
                    int index = i + data->scroll_offset;
                    tunix_style_color item_bg = (index == data->selected_index) ? 
                        tunix_color_from_type(TUNIX_COLOR_BLUE) : bg;
                    tunix_renderer_draw_text(renderer, bounds.x + 1, bounds.y + 1 + i, 
                                           data->items[index], fg, item_bg);
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_DROPDOWN: {
            dropdown_data *data = (dropdown_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->items && data->item_count > 0) {
                if (data->selected_index >= 0 && data->selected_index < data->item_count) {
                    tunix_renderer_draw_text(renderer, bounds.x + 1, bounds.y + 1, 
                                           data->items[data->selected_index], fg, bg);
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_MENU: {
            menu_data *data = (menu_data*)widget->data;
            int x_offset = bounds.x;
            if (data && data->items) {
                for (int i = 0; i < data->item_count; i++) {
                    tunix_style_color item_bg = (i == data->selected_index) ? 
                        tunix_color_from_type(TUNIX_COLOR_BLUE) : bg;
                    tunix_renderer_draw_text(renderer, x_offset, bounds.y, 
                                           data->items[i], fg, item_bg);
                    x_offset += (int)strlen(data->items[i]) + 2;
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_TABS: {
            tabs_data *data = (tabs_data*)widget->data;
            int x_offset = bounds.x;
            if (data && data->tab_names) {
                for (int i = 0; i < data->tab_count; i++) {
                    tunix_style_color tab_bg = (i == data->active_tab) ? 
                        tunix_color_from_type(TUNIX_COLOR_BLUE) : bg;
                    char tab_text[64];
                    snprintf(tab_text, sizeof(tab_text), " %s ", data->tab_names[i]);
                    tunix_renderer_draw_text(renderer, x_offset, bounds.y, 
                                           tab_text, fg, tab_bg);
                    x_offset += (int)strlen(tab_text);
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_TABLE: {
            table_data *data = (table_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, bg);
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->cells) {
                int cell_width = (bounds.width - 2) / data->cols;
                for (int row = 0; row < data->rows && row < bounds.height - 2; row++) {
                    for (int col = 0; col < data->cols; col++) {
                        int cell_x = bounds.x + 1 + col * cell_width;
                        int cell_y = bounds.y + 1 + row;
                        if (data->cells[row][col]) {
                            tunix_renderer_draw_text(renderer, cell_x, cell_y, 
                                                   data->cells[row][col], fg, bg);
                        }
                    }
                }
            }
            break;
        }
        
        case TUNIX_WIDGET_STATUSBAR: {
            statusbar_data *data = (statusbar_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, 
                                   tunix_color_from_type(TUNIX_COLOR_BRIGHT_BLACK));
            if (data && data->text) {
                tunix_renderer_draw_text(renderer, bounds.x, bounds.y, 
                                       data->text, fg, 
                                       tunix_color_from_type(TUNIX_COLOR_BRIGHT_BLACK));
            }
            break;
        }
        
        case TUNIX_WIDGET_NOTIFICATION: {
            notification_data *data = (notification_data*)widget->data;
            tunix_renderer_draw_rect(renderer, bounds, fg, 
                                   tunix_color_from_type(TUNIX_COLOR_YELLOW));
            if (widget->theme.border != TUNIX_BORDER_NONE) {
                tunix_renderer_draw_border(renderer, bounds, widget->theme.border, 
                                         &widget->theme.border_chars, border_color);
            }
            if (data && data->text) {
                tunix_renderer_draw_text(renderer, bounds.x + 1, bounds.y + 1, 
                                       data->text, 
                                       tunix_color_from_type(TUNIX_COLOR_BLACK), 
                                       tunix_color_from_type(TUNIX_COLOR_YELLOW));
            }
            break;
        }
        
        case TUNIX_WIDGET_SEPARATOR: {
            separator_data *data = (separator_data*)widget->data;
            bool use_unicode = tunix_platform_supports_unicode();
            if (data && data->horizontal) {
                const char *ch = use_unicode ? "─" : "-";
                tunix_renderer_draw_hline(renderer, bounds.x, bounds.y, 
                                        bounds.width, ch, fg, bg);
            } else {
                const char *ch = use_unicode ? "│" : "|";
                tunix_renderer_draw_vline(renderer, bounds.x, bounds.y, 
                                        bounds.height, ch, fg, bg);
            }
            break;
        }
        
        case TUNIX_WIDGET_SPINNER: {
            spinner_data *data = (spinner_data*)widget->data;
            if (data && data->frames && data->frame_count > 0) {
                const char *frame = data->frames[data->frame];
                tunix_renderer_draw_text(renderer, bounds.x, bounds.y, 
                                       frame, fg, bg);
            }
            break;
        }
        
        default:
            break;
    }
    
    /* Render children */
    for (int i = 0; i < widget->child_count; i++) {
        tunix_widget_render(widget->children[i], renderer);
    }
}

/* Event handling */
bool tunix_widget_handle_event(tunix_widget *widget, tunix_event *event) {
    if (!widget || !event || !widget->visible || !widget->enabled) {
        return false;
    }
    
    /* Pass event to children first */
    for (int i = widget->child_count - 1; i >= 0; i--) {
        if (tunix_widget_handle_event(widget->children[i], event)) {
            return true;
        }
    }
    
    if (event->type == TUNIX_EVENT_KEY) {
        if (event->data.key.key == TUNIX_KEY_TAB) {
            /* Focus next focusable widget */
            return true;
        }
        
        if (widget->focused) {
            switch (widget->type) {
                case TUNIX_WIDGET_BUTTON:
                    if (event->data.key.key == TUNIX_KEY_ENTER) {
                        if (widget->on_click) {
                            widget->on_click(widget, widget->user_data);
                            return true;
                        }
                    }
                    break;
                    
                case TUNIX_WIDGET_CHECKBOX: {
                    if (event->data.key.key == TUNIX_KEY_SPACE) {
                        checkbox_data *data = (checkbox_data*)widget->data;
                        if (data) {
                            data->checked = !data->checked;
                            if (widget->on_change) {
                                widget->on_change(widget, widget->user_data);
                            }
                            return true;
                        }
                    }
                    break;
                }
                
                case TUNIX_WIDGET_TEXTBOX: {
                    textbox_data *data = (textbox_data*)widget->data;
                    if (data) {
                        if (event->data.key.ch >= 32 && event->data.key.ch < 127) {
                            size_t len = data->text ? strlen(data->text) : 0;
                            char *new_text = (char*)realloc(data->text, len + 2);
                            if (new_text) {
                                data->text = new_text;
                                data->text[len] = event->data.key.ch;
                                data->text[len + 1] = '\0';
                                data->cursor_pos = (int)len + 1;
                                if (widget->on_change) {
                                    widget->on_change(widget, widget->user_data);
                                }
                                return true;
                            }
                        } else if (event->data.key.key == TUNIX_KEY_BACKSPACE && data->text) {
                            size_t len = strlen(data->text);
                            if (len > 0) {
                                data->text[len - 1] = '\0';
                                data->cursor_pos = (int)len - 1;
                                if (widget->on_change) {
                                    widget->on_change(widget, widget->user_data);
                                }
                                return true;
                            }
                        }
                    }
                    break;
                }
                
                case TUNIX_WIDGET_LIST: {
                    list_data *data = (list_data*)widget->data;
                    if (data && data->items) {
                        if (event->data.key.key == TUNIX_KEY_ARROW_DOWN) {
                            if (data->selected_index < data->item_count - 1) {
                                data->selected_index++;
                                if (widget->on_change) {
                                    widget->on_change(widget, widget->user_data);
                                }
                                return true;
                            }
                        } else if (event->data.key.key == TUNIX_KEY_ARROW_UP) {
                            if (data->selected_index > 0) {
                                data->selected_index--;
                                if (widget->on_change) {
                                    widget->on_change(widget, widget->user_data);
                                }
                                return true;
                            }
                        } else if (event->data.key.key == TUNIX_KEY_ENTER) {
                            if (widget->on_click) {
                                widget->on_click(widget, widget->user_data);
                                return true;
                            }
                        }
                    }
                    break;
                }
                
                default:
                    break;
            }
        }
    }
    
    return false;
}

/* Widget creators */
tunix_widget* tunix_window_create(const char *title) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_WINDOW);
    if (!widget) {
        return NULL;
    }
    
    window_data *data = (window_data*)calloc(1, sizeof(window_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->title = title ? tunix_strdup(title) : NULL;
    widget->data = data;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    
    return widget;
}

tunix_widget* tunix_panel_create(void) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_PANEL);
    if (widget) {
        widget->theme.border = TUNIX_BORDER_SINGLE;
    }
    return widget;
}

tunix_widget* tunix_label_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_LABEL);
    if (!widget) {
        return NULL;
    }
    
    label_data *data = (label_data*)calloc(1, sizeof(label_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : NULL;
    widget->data = data;
    widget->bounds.height = 1;
    widget->bounds.width = text ? (int)strlen(text) : 0;
    
    return widget;
}

tunix_widget* tunix_button_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_BUTTON);
    if (!widget) {
        return NULL;
    }
    
    button_data *data = (button_data*)calloc(1, sizeof(button_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : NULL;
    widget->data = data;
    widget->focusable = true;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.width = text ? (int)strlen(text) + 4 : 10;
    widget->bounds.height = 3;
    
    return widget;
}

tunix_widget* tunix_checkbox_create(const char *text, bool checked) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_CHECKBOX);
    if (!widget) {
        return NULL;
    }
    
    checkbox_data *data = (checkbox_data*)calloc(1, sizeof(checkbox_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : NULL;
    data->checked = checked;
    widget->data = data;
    widget->focusable = true;
    widget->bounds.height = 1;
    widget->bounds.width = 4 + (text ? (int)strlen(text) : 0);
    
    return widget;
}

tunix_widget* tunix_radio_create(const char *text, int group_id) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_RADIO);
    if (!widget) {
        return NULL;
    }
    
    radio_data *data = (radio_data*)calloc(1, sizeof(radio_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : NULL;
    data->group_id = group_id;
    data->selected = false;
    widget->data = data;
    widget->focusable = true;
    widget->bounds.height = 1;
    widget->bounds.width = 4 + (text ? (int)strlen(text) : 0);
    
    return widget;
}

tunix_widget* tunix_progress_create(float value) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_PROGRESS);
    if (!widget) {
        return NULL;
    }
    
    progress_data *data = (progress_data*)calloc(1, sizeof(progress_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->value = value;
    widget->data = data;
    widget->bounds.height = 1;
    widget->bounds.width = 20;
    
    return widget;
}

tunix_widget* tunix_slider_create(float min, float max, float value) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_SLIDER);
    if (!widget) {
        return NULL;
    }
    
    slider_data *data = (slider_data*)calloc(1, sizeof(slider_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->min = min;
    data->max = max;
    data->value = value;
    widget->data = data;
    widget->focusable = true;
    widget->bounds.height = 1;
    widget->bounds.width = 20;
    
    return widget;
}

tunix_widget* tunix_textbox_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_TEXTBOX);
    if (!widget) {
        return NULL;
    }
    
    textbox_data *data = (textbox_data*)calloc(1, sizeof(textbox_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : tunix_strdup("");
    data->cursor_pos = text ? (int)strlen(text) : 0;
    widget->data = data;
    widget->focusable = true;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = 3;
    widget->bounds.width = 30;
    
    return widget;
}

tunix_widget* tunix_editor_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_EDITOR);
    if (!widget) {
        return NULL;
    }
    
    editor_data *data = (editor_data*)calloc(1, sizeof(editor_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->lines = (char**)calloc(1, sizeof(char*));
    data->lines[0] = text ? tunix_strdup(text) : tunix_strdup("");
    data->line_count = 1;
    data->cursor_row = 0;
    data->cursor_col = 0;
    data->scroll_offset = 0;
    widget->data = data;
    widget->focusable = true;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = 10;
    widget->bounds.width = 40;
    
    return widget;
}

tunix_widget* tunix_list_create(const char **items, int item_count) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_LIST);
    if (!widget) {
        return NULL;
    }
    
    list_data *data = (list_data*)calloc(1, sizeof(list_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->items = (char**)calloc(item_count, sizeof(char*));
    for (int i = 0; i < item_count; i++) {
        data->items[i] = items[i] ? tunix_strdup(items[i]) : tunix_strdup("");
    }
    data->item_count = item_count;
    data->selected_index = 0;
    data->scroll_offset = 0;
    widget->data = data;
    widget->focusable = true;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = 10;
    widget->bounds.width = 30;
    
    return widget;
}

tunix_widget* tunix_dropdown_create(const char **items, int item_count) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_DROPDOWN);
    if (!widget) {
        return NULL;
    }
    
    dropdown_data *data = (dropdown_data*)calloc(1, sizeof(dropdown_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->items = (char**)calloc(item_count, sizeof(char*));
    for (int i = 0; i < item_count; i++) {
        data->items[i] = items[i] ? tunix_strdup(items[i]) : tunix_strdup("");
    }
    data->item_count = item_count;
    data->selected_index = 0;
    data->expanded = false;
    widget->data = data;
    widget->focusable = true;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = 3;
    widget->bounds.width = 20;
    
    return widget;
}

tunix_widget* tunix_menu_create(const char **items, int item_count) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_MENU);
    if (!widget) {
        return NULL;
    }
    
    menu_data *data = (menu_data*)calloc(1, sizeof(menu_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->items = (char**)calloc(item_count, sizeof(char*));
    for (int i = 0; i < item_count; i++) {
        data->items[i] = items[i] ? tunix_strdup(items[i]) : tunix_strdup("");
    }
    data->item_count = item_count;
    data->selected_index = -1;
    widget->data = data;
    widget->focusable = true;
    widget->bounds.height = 1;
    widget->bounds.width = 50;
    
    return widget;
}

tunix_widget* tunix_tabs_create(const char **tab_names, int tab_count) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_TABS);
    if (!widget) {
        return NULL;
    }
    
    tabs_data *data = (tabs_data*)calloc(1, sizeof(tabs_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->tab_names = (char**)calloc(tab_count, sizeof(char*));
    for (int i = 0; i < tab_count; i++) {
        data->tab_names[i] = tab_names[i] ? tunix_strdup(tab_names[i]) : tunix_strdup("");
    }
    data->tab_count = tab_count;
    data->active_tab = 0;
    widget->data = data;
    widget->focusable = true;
    widget->bounds.height = 1;
    widget->bounds.width = 50;
    
    return widget;
}

tunix_widget* tunix_table_create(int rows, int cols) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_TABLE);
    if (!widget) {
        return NULL;
    }
    
    table_data *data = (table_data*)calloc(1, sizeof(table_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->cells = (char***)calloc(rows, sizeof(char**));
    for (int i = 0; i < rows; i++) {
        data->cells[i] = (char**)calloc(cols, sizeof(char*));
        for (int j = 0; j < cols; j++) {
            data->cells[i][j] = tunix_strdup("");
        }
    }
    data->rows = rows;
    data->cols = cols;
    widget->data = data;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = rows + 2;
    widget->bounds.width = cols * 10 + 2;
    
    return widget;
}

tunix_widget* tunix_statusbar_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_STATUSBAR);
    if (!widget) {
        return NULL;
    }
    
    statusbar_data *data = (statusbar_data*)calloc(1, sizeof(statusbar_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : tunix_strdup("");
    widget->data = data;
    widget->bounds.height = 1;
    widget->bounds.width = 80;
    
    return widget;
}

tunix_widget* tunix_notification_create(const char *text) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_NOTIFICATION);
    if (!widget) {
        return NULL;
    }
    
    notification_data *data = (notification_data*)calloc(1, sizeof(notification_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->text = text ? tunix_strdup(text) : tunix_strdup("");
    data->timeout = 3000;
    widget->data = data;
    widget->theme.border = TUNIX_BORDER_SINGLE;
    widget->bounds.height = 3;
    widget->bounds.width = 40;
    
    return widget;
}

tunix_widget* tunix_separator_create(bool horizontal) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_SEPARATOR);
    if (!widget) {
        return NULL;
    }
    
    separator_data *data = (separator_data*)calloc(1, sizeof(separator_data));
    if (!data) {
        tunix_widget_destroy(widget);
        return NULL;
    }
    
    data->horizontal = horizontal;
    widget->data = data;
    widget->bounds.height = horizontal ? 1 : 10;
    widget->bounds.width = horizontal ? 40 : 1;
    
    return widget;
}

tunix_widget* tunix_vstack_create(void) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_VSTACK);
    return widget;
}

tunix_widget* tunix_hstack_create(void) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_HSTACK);
    return widget;
}

tunix_widget* tunix_grid_create(int rows, int cols) {
    tunix_widget *widget = tunix_widget_create(TUNIX_WIDGET_GRID);
    if (widget) {
        widget->bounds.width = cols * 10;
        widget->bounds.height = rows * 3;
    }
    return widget;
}

/* Widget-specific getters and setters */
void tunix_label_set_text(tunix_widget *widget, const char *text) {
    if (widget && widget->type == TUNIX_WIDGET_LABEL) {
        label_data *data = (label_data*)widget->data;
        if (data) {
            free(data->text);
            data->text = text ? tunix_strdup(text) : NULL;
        }
    }
}

const char* tunix_label_get_text(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_LABEL) {
        label_data *data = (label_data*)widget->data;
        return data ? data->text : NULL;
    }
    return NULL;
}

void tunix_button_set_text(tunix_widget *widget, const char *text) {
    if (widget && widget->type == TUNIX_WIDGET_BUTTON) {
        button_data *data = (button_data*)widget->data;
        if (data) {
            free(data->text);
            data->text = text ? tunix_strdup(text) : NULL;
        }
    }
}

const char* tunix_button_get_text(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_BUTTON) {
        button_data *data = (button_data*)widget->data;
        return data ? data->text : NULL;
    }
    return NULL;
}

void tunix_checkbox_set_checked(tunix_widget *widget, bool checked) {
    if (widget && widget->type == TUNIX_WIDGET_CHECKBOX) {
        checkbox_data *data = (checkbox_data*)widget->data;
        if (data) {
            data->checked = checked;
        }
    }
}

bool tunix_checkbox_get_checked(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_CHECKBOX) {
        checkbox_data *data = (checkbox_data*)widget->data;
        return data ? data->checked : false;
    }
    return false;
}

void tunix_progress_set_value(tunix_widget *widget, float value) {
    if (widget && widget->type == TUNIX_WIDGET_PROGRESS) {
        progress_data *data = (progress_data*)widget->data;
        if (data) {
            data->value = value;
        }
    }
}

float tunix_progress_get_value(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_PROGRESS) {
        progress_data *data = (progress_data*)widget->data;
        return data ? data->value : 0.0f;
    }
    return 0.0f;
}

void tunix_slider_set_value(tunix_widget *widget, float value) {
    if (widget && widget->type == TUNIX_WIDGET_SLIDER) {
        slider_data *data = (slider_data*)widget->data;
        if (data) {
            data->value = value;
        }
    }
}

float tunix_slider_get_value(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_SLIDER) {
        slider_data *data = (slider_data*)widget->data;
        return data ? data->value : 0.0f;
    }
    return 0.0f;
}

void tunix_textbox_set_text(tunix_widget *widget, const char *text) {
    if (widget && widget->type == TUNIX_WIDGET_TEXTBOX) {
        textbox_data *data = (textbox_data*)widget->data;
        if (data) {
            free(data->text);
            data->text = text ? tunix_strdup(text) : tunix_strdup("");
            data->cursor_pos = (int)strlen(data->text);
        }
    }
}

const char* tunix_textbox_get_text(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_TEXTBOX) {
        textbox_data *data = (textbox_data*)widget->data;
        return data ? data->text : NULL;
    }
    return NULL;
}

void tunix_editor_set_text(tunix_widget *widget, const char *text) {
    if (widget && widget->type == TUNIX_WIDGET_EDITOR) {
        editor_data *data = (editor_data*)widget->data;
        if (data) {
            for (int i = 0; i < data->line_count; i++) {
                free(data->lines[i]);
            }
            free(data->lines);
            data->lines = (char**)calloc(1, sizeof(char*));
            data->lines[0] = text ? tunix_strdup(text) : tunix_strdup("");
            data->line_count = 1;
            data->cursor_row = 0;
            data->cursor_col = 0;
            data->scroll_offset = 0;
        }
    }
}

const char* tunix_editor_get_text(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_EDITOR) {
        editor_data *data = (editor_data*)widget->data;
        return (data && data->line_count > 0) ? data->lines[0] : NULL;
    }
    return NULL;
}

void tunix_list_set_selected(tunix_widget *widget, int index) {
    if (widget && widget->type == TUNIX_WIDGET_LIST) {
        list_data *data = (list_data*)widget->data;
        if (data && index >= 0 && index < data->item_count) {
            data->selected_index = index;
        }
    }
}

int tunix_list_get_selected(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_LIST) {
        list_data *data = (list_data*)widget->data;
        return data ? data->selected_index : -1;
    }
    return -1;
}

void tunix_dropdown_set_selected(tunix_widget *widget, int index) {
    if (widget && widget->type == TUNIX_WIDGET_DROPDOWN) {
        dropdown_data *data = (dropdown_data*)widget->data;
        if (data && index >= 0 && index < data->item_count) {
            data->selected_index = index;
        }
    }
}

int tunix_dropdown_get_selected(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_DROPDOWN) {
        dropdown_data *data = (dropdown_data*)widget->data;
        return data ? data->selected_index : -1;
    }
    return -1;
}

void tunix_tabs_set_active(tunix_widget *widget, int index) {
    if (widget && widget->type == TUNIX_WIDGET_TABS) {
        tabs_data *data = (tabs_data*)widget->data;
        if (data && index >= 0 && index < data->tab_count) {
            data->active_tab = index;
        }
    }
}

int tunix_tabs_get_active(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_TABS) {
        tabs_data *data = (tabs_data*)widget->data;
        return data ? data->active_tab : -1;
    }
    return -1;
}

void tunix_table_set_cell(tunix_widget *widget, int row, int col, const char *text) {
    if (widget && widget->type == TUNIX_WIDGET_TABLE) {
        table_data *data = (table_data*)widget->data;
        if (data && row >= 0 && row < data->rows && col >= 0 && col < data->cols) {
            free(data->cells[row][col]);
            data->cells[row][col] = text ? tunix_strdup(text) : tunix_strdup("");
        }
    }
}

const char* tunix_table_get_cell(tunix_widget *widget, int row, int col) {
    if (widget && widget->type == TUNIX_WIDGET_TABLE) {
        table_data *data = (table_data*)widget->data;
        if (data && row >= 0 && row < data->rows && col >= 0 && col < data->cols) {
            return data->cells[row][col];
        }
    }
    return NULL;
}

/* Spinner frames for different styles */
static const char *spinner_dots_frames[] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};
static const char *spinner_line_frames[] = {"-", "\\", "|", "/"};
static const char *spinner_circle_frames[] = {"O", "o", ".", "o"};
static const char *spinner_arrow_frames[] = {"<", "^", ">", "v"};
static const char *spinner_bounce_frames[] = {"[    ]", "[ .  ]", "[  . ]", "[   .]", "[  . ]", "[ .  ]"};
static const char *spinner_clock_frames[] = {"|", "/", "-", "\\"};
static const char *spinner_dots_simple_frames[] = {".  ", ".. ", "..."};

tunix_widget* tunix_spinner_create(tunix_spinner_style style) {
    tunix_widget *widget = (tunix_widget*)calloc(1, sizeof(tunix_widget));
    if (!widget) return NULL;
    
    widget->type = TUNIX_WIDGET_SPINNER;
    widget->visible = true;
    widget->enabled = true;
    widget->focusable = false;
    widget->theme = tunix_theme_default();
    
    spinner_data *data = (spinner_data*)calloc(1, sizeof(spinner_data));
    if (!data) {
        free(widget);
        return NULL;
    }
    
    data->style = style;
    data->frame = 0;
    data->running = false;
    
    /* Set frame arrays based on style */
    switch (style) {
        case TUNIX_SPINNER_DOTS:
            data->frames = spinner_dots_frames;
            data->frame_count = 8;
            break;
        case TUNIX_SPINNER_LINE:
            data->frames = spinner_line_frames;
            data->frame_count = 4;
            break;
        case TUNIX_SPINNER_CIRCLE:
            data->frames = spinner_circle_frames;
            data->frame_count = 4;
            break;
        case TUNIX_SPINNER_ARROW:
            data->frames = spinner_arrow_frames;
            data->frame_count = 4;
            break;
        case TUNIX_SPINNER_BOUNCE:
            data->frames = spinner_bounce_frames;
            data->frame_count = 6;
            break;
        case TUNIX_SPINNER_CLOCK:
            data->frames = spinner_clock_frames;
            data->frame_count = 4;
            break;
        case TUNIX_SPINNER_DOTS_SIMPLE:
            data->frames = spinner_dots_simple_frames;
            data->frame_count = 3;
            break;
    }
    
    widget->data = data;
    return widget;
}

void tunix_spinner_set_style(tunix_widget *widget, tunix_spinner_style style) {
    if (widget && widget->type == TUNIX_WIDGET_SPINNER) {
        spinner_data *data = (spinner_data*)widget->data;
        if (data) {
            data->style = style;
            data->frame = 0;
            
            /* Update frame arrays */
            switch (style) {
                case TUNIX_SPINNER_DOTS:
                    data->frames = spinner_dots_frames;
                    data->frame_count = 8;
                    break;
                case TUNIX_SPINNER_LINE:
                    data->frames = spinner_line_frames;
                    data->frame_count = 4;
                    break;
                case TUNIX_SPINNER_CIRCLE:
                    data->frames = spinner_circle_frames;
                    data->frame_count = 4;
                    break;
                case TUNIX_SPINNER_ARROW:
                    data->frames = spinner_arrow_frames;
                    data->frame_count = 4;
                    break;
                case TUNIX_SPINNER_BOUNCE:
                    data->frames = spinner_bounce_frames;
                    data->frame_count = 6;
                    break;
                case TUNIX_SPINNER_CLOCK:
                    data->frames = spinner_clock_frames;
                    data->frame_count = 4;
                    break;
                case TUNIX_SPINNER_DOTS_SIMPLE:
                    data->frames = spinner_dots_simple_frames;
                    data->frame_count = 3;
                    break;
            }
        }
    }
}

void tunix_spinner_start(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_SPINNER) {
        spinner_data *data = (spinner_data*)widget->data;
        if (data) {
            data->running = true;
            data->frame = 0;
        }
    }
}

void tunix_spinner_stop(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_SPINNER) {
        spinner_data *data = (spinner_data*)widget->data;
        if (data) {
            data->running = false;
        }
    }
}

void tunix_spinner_tick(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_SPINNER) {
        spinner_data *data = (spinner_data*)widget->data;
        if (data && data->running) {
            data->frame = (data->frame + 1) % data->frame_count;
        }
    }
}

bool tunix_spinner_is_running(tunix_widget *widget) {
    if (widget && widget->type == TUNIX_WIDGET_SPINNER) {
        spinner_data *data = (spinner_data*)widget->data;
        if (data) {
            return data->running;
        }
    }
    return false;
}
