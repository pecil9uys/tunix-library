#ifndef TUNIX_WIDGET_H
#define TUNIX_WIDGET_H

#include "tunix_types.h"
#include "tunix_renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
typedef struct tunix_widget tunix_widget;
typedef struct tunix_app tunix_app;

/* Widget type */
typedef enum tunix_widget_type {
    TUNIX_WIDGET_WINDOW,
    TUNIX_WIDGET_PANEL,
    TUNIX_WIDGET_LABEL,
    TUNIX_WIDGET_BUTTON,
    TUNIX_WIDGET_CHECKBOX,
    TUNIX_WIDGET_RADIO,
    TUNIX_WIDGET_PROGRESS,
    TUNIX_WIDGET_SLIDER,
    TUNIX_WIDGET_TEXTBOX,
    TUNIX_WIDGET_EDITOR,
    TUNIX_WIDGET_LIST,
    TUNIX_WIDGET_DROPDOWN,
    TUNIX_WIDGET_MENU,
    TUNIX_WIDGET_TABS,
    TUNIX_WIDGET_TABLE,
    TUNIX_WIDGET_STATUSBAR,
    TUNIX_WIDGET_NOTIFICATION,
    TUNIX_WIDGET_SEPARATOR,
    TUNIX_WIDGET_SPINNER,
    TUNIX_WIDGET_VSTACK,
    TUNIX_WIDGET_HSTACK,
    TUNIX_WIDGET_GRID
} tunix_widget_type;

/* Layout direction */
typedef enum tunix_layout_direction {
    TUNIX_LAYOUT_VERTICAL,
    TUNIX_LAYOUT_HORIZONTAL
} tunix_layout_direction;

/* Callback types */
typedef void (*tunix_callback)(tunix_widget *widget, void *user_data);
typedef void (*tunix_render_callback)(tunix_widget *widget, tunix_renderer *renderer, void *user_data);

/* Widget base structure */
struct tunix_widget {
    tunix_widget_type type;
    char *id;
    
    /* Layout */
    tunix_rect bounds;
    bool visible;
    bool enabled;
    bool focusable;
    bool focused;
    
    /* Theme */
    tunix_theme theme;
    
    /* Children */
    tunix_widget **children;
    int child_count;
    int child_capacity;
    
    /* Parent */
    tunix_widget *parent;
    
    /* Callbacks */
    tunix_callback on_click;
    tunix_callback on_change;
    tunix_callback on_focus;
    tunix_callback on_blur;
    tunix_render_callback on_render;
    void *user_data;
    
    /* Widget-specific data */
    void *data;
};

/* Application */
tunix_app* tunix_app_create(const char *title);
void tunix_app_destroy(tunix_app *app);
bool tunix_app_run(tunix_app *app);
void tunix_app_quit(tunix_app *app);
tunix_widget* tunix_app_root(tunix_app *app);
void tunix_app_set_theme(tunix_app *app, tunix_theme theme);

/* Widget creation */
tunix_widget* tunix_widget_create(tunix_widget_type type);
void tunix_widget_destroy(tunix_widget *widget);

/* Widget tree management */
void tunix_widget_add_child(tunix_widget *parent, tunix_widget *child);
void tunix_widget_remove_child(tunix_widget *parent, tunix_widget *child);
tunix_widget* tunix_widget_find(tunix_widget *root, const char *id);

/* Widget properties */
void tunix_widget_set_id(tunix_widget *widget, const char *id);
void tunix_widget_set_bounds(tunix_widget *widget, tunix_rect bounds);
void tunix_widget_set_visible(tunix_widget *widget, bool visible);
void tunix_widget_set_enabled(tunix_widget *widget, bool enabled);
void tunix_widget_set_focusable(tunix_widget *widget, bool focusable);
void tunix_widget_set_theme(tunix_widget *widget, tunix_theme theme);

/* Widget callbacks */
void tunix_widget_set_on_click(tunix_widget *widget, tunix_callback callback, void *user_data);
void tunix_widget_set_on_change(tunix_widget *widget, tunix_callback callback, void *user_data);
void tunix_widget_set_on_focus(tunix_widget *widget, tunix_callback callback, void *user_data);
void tunix_widget_set_on_blur(tunix_widget *widget, tunix_callback callback, void *user_data);
void tunix_widget_set_on_render(tunix_widget *widget, tunix_render_callback callback, void *user_data);

/* Widget rendering */
void tunix_widget_render(tunix_widget *widget, tunix_renderer *renderer);

/* Event handling */
bool tunix_widget_handle_event(tunix_widget *widget, tunix_event *event);

/* Focus management */
void tunix_widget_focus(tunix_widget *widget);
void tunix_widget_blur(tunix_widget *widget);
tunix_widget* tunix_widget_get_focused(tunix_widget *root);

/* Specific widget creators */
tunix_widget* tunix_window_create(const char *title);
tunix_widget* tunix_panel_create(void);
tunix_widget* tunix_label_create(const char *text);
tunix_widget* tunix_button_create(const char *text);
tunix_widget* tunix_checkbox_create(const char *text, bool checked);
tunix_widget* tunix_radio_create(const char *text, int group_id);
tunix_widget* tunix_progress_create(float value);
tunix_widget* tunix_slider_create(float min, float max, float value);
tunix_widget* tunix_textbox_create(const char *text);
tunix_widget* tunix_editor_create(const char *text);
tunix_widget* tunix_list_create(const char **items, int item_count);
tunix_widget* tunix_dropdown_create(const char **items, int item_count);
tunix_widget* tunix_menu_create(const char **items, int item_count);
tunix_widget* tunix_tabs_create(const char **tab_names, int tab_count);
tunix_widget* tunix_table_create(int rows, int cols);
tunix_widget* tunix_statusbar_create(const char *text);
tunix_widget* tunix_notification_create(const char *text);
tunix_widget* tunix_separator_create(bool horizontal);
tunix_widget* tunix_vstack_create(void);
tunix_widget* tunix_hstack_create(void);
tunix_widget* tunix_grid_create(int rows, int cols);

/* Widget-specific setters */
void tunix_label_set_text(tunix_widget *widget, const char *text);
const char* tunix_label_get_text(tunix_widget *widget);

void tunix_button_set_text(tunix_widget *widget, const char *text);
const char* tunix_button_get_text(tunix_widget *widget);

void tunix_checkbox_set_checked(tunix_widget *widget, bool checked);
bool tunix_checkbox_get_checked(tunix_widget *widget);

void tunix_progress_set_value(tunix_widget *widget, float value);
float tunix_progress_get_value(tunix_widget *widget);

void tunix_slider_set_value(tunix_widget *widget, float value);
float tunix_slider_get_value(tunix_widget *widget);

void tunix_textbox_set_text(tunix_widget *widget, const char *text);
const char* tunix_textbox_get_text(tunix_widget *widget);

void tunix_editor_set_text(tunix_widget *widget, const char *text);
const char* tunix_editor_get_text(tunix_widget *widget);

void tunix_list_set_selected(tunix_widget *widget, int index);
int tunix_list_get_selected(tunix_widget *widget);

void tunix_dropdown_set_selected(tunix_widget *widget, int index);
int tunix_dropdown_get_selected(tunix_widget *widget);

void tunix_tabs_set_active(tunix_widget *widget, int index);
int tunix_tabs_get_active(tunix_widget *widget);

void tunix_table_set_cell(tunix_widget *widget, int row, int col, const char *text);
const char* tunix_table_get_cell(tunix_widget *widget, int row, int col);

/* Spinner widget */
tunix_widget* tunix_spinner_create(tunix_spinner_style style);
void tunix_spinner_set_style(tunix_widget *widget, tunix_spinner_style style);
void tunix_spinner_start(tunix_widget *widget);
void tunix_spinner_stop(tunix_widget *widget);
void tunix_spinner_tick(tunix_widget *widget); /* Call to advance animation frame */
bool tunix_spinner_is_running(tunix_widget *widget);

#ifdef __cplusplus
}
#endif

#endif /* TUNIX_WIDGET_H */
