/* interactive_demo.c - Full interactive controls demonstration */
#include "tunix.h"
#include <stdio.h>
#include <stdlib.h>

static tunix_app *app = NULL;
static tunix_widget *progress_bar = NULL;
static tunix_widget *slider_widget = NULL;
static tunix_widget *status_label = NULL;

static void on_slider_change(tunix_widget *widget, void *user_data) {
    (void)user_data;
    float value = tunix_slider_get_value(widget);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Slider: %.2f | Use arrow keys to adjust", value);
    tunix_label_set_text(status_label, buffer);
}

static void on_progress_change(tunix_widget *widget, void *user_data) {
    (void)user_data;
    float value = tunix_progress_get_value(widget);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Progress: %.0f%% | Use arrow keys or +/- to adjust", value * 100);
    tunix_label_set_text(status_label, buffer);
}

static void on_list_select(tunix_widget *widget, void *user_data) {
    (void)user_data;
    int selected = tunix_list_get_selected(widget);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Selected item: %d | Use arrow keys to navigate", selected);
    tunix_label_set_text(status_label, buffer);
}

static void on_dropdown_change(tunix_widget *widget, void *user_data) {
    (void)user_data;
    int selected = tunix_dropdown_get_selected(widget);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Selected option: %d | Press Enter/Space to toggle", selected);
    tunix_label_set_text(status_label, buffer);
}

static void on_quit(tunix_widget *widget, void *user_data) {
    (void)widget;
    (void)user_data;
    tunix_app_quit(app);
}

int main(void) {
    /* Create application */
    app = tunix_app_create("Tunix Interactive Controls Demo");
    if (!app) {
        fprintf(stderr, "Failed to create app\n");
        return 1;
    }

    tunix_widget *root = tunix_app_root(app);
    tunix_rect screen = {0, 0, 80, 25};
    tunix_widget_set_bounds(root, screen);

    /* Create main panel */
    tunix_widget *panel = tunix_panel_create();
    tunix_rect panel_bounds = {1, 1, 78, 23};
    tunix_widget_set_bounds(panel, panel_bounds);
    tunix_widget_add_child(root, panel);

    /* Title */
    tunix_widget *title = tunix_label_create("Interactive Terminal UI Controls - Press TAB to cycle focus");
    tunix_rect title_bounds = {2, 1, 74, 1};
    tunix_widget_set_bounds(title, title_bounds);
    tunix_widget_add_child(panel, title);

    /* Progress Bar (focusable, use arrow keys) */
    tunix_widget *progress_label = tunix_label_create("Progress Bar (Arrow keys or +/-):");
    tunix_rect progress_label_bounds = {2, 3, 35, 1};
    tunix_widget_set_bounds(progress_label, progress_label_bounds);
    tunix_widget_add_child(panel, progress_label);

    progress_bar = tunix_progress_create(0.5f);
    tunix_rect progress_bounds = {2, 4, 40, 1};
    tunix_widget_set_bounds(progress_bar, progress_bounds);
    tunix_widget_set_on_change(progress_bar, on_progress_change, NULL);
    tunix_widget_add_child(panel, progress_bar);

    /* Slider (focusable, use arrow keys) */
    tunix_widget *slider_label = tunix_label_create("Slider (Arrow keys):");
    tunix_rect slider_label_bounds = {2, 6, 25, 1};
    tunix_widget_set_bounds(slider_label, slider_label_bounds);
    tunix_widget_add_child(panel, slider_label);

    slider_widget = tunix_slider_create(0.0f, 100.0f, 50.0f);
    tunix_rect slider_bounds = {2, 7, 40, 1};
    tunix_widget_set_bounds(slider_widget, slider_bounds);
    tunix_widget_set_on_change(slider_widget, on_slider_change, NULL);
    tunix_widget_add_child(panel, slider_widget);

    /* Textbox (focusable, type to edit) */
    tunix_widget *textbox_label = tunix_label_create("Text Input (Type to edit, Backspace to delete):");
    tunix_rect textbox_label_bounds = {2, 9, 50, 1};
    tunix_widget_set_bounds(textbox_label, textbox_label_bounds);
    tunix_widget_add_child(panel, textbox_label);

    tunix_widget *textbox = tunix_textbox_create("Type here...");
    tunix_rect textbox_bounds = {2, 10, 50, 3};
    tunix_widget_set_bounds(textbox, textbox_bounds);
    tunix_widget_add_child(panel, textbox);

    /* List (focusable, arrow keys) */
    tunix_widget *list_label = tunix_label_create("List (Arrow keys, Enter to select):");
    tunix_rect list_label_bounds = {2, 13, 40, 1};
    tunix_widget_set_bounds(list_label, list_label_bounds);
    tunix_widget_add_child(panel, list_label);

    const char *items[] = {"Option 1", "Option 2", "Option 3", "Option 4", "Option 5"};
    tunix_widget *list = tunix_list_create(items, 5);
    tunix_rect list_bounds = {2, 14, 25, 7};
    tunix_widget_set_bounds(list, list_bounds);
    tunix_widget_set_on_change(list, on_list_select, NULL);
    tunix_widget_add_child(panel, list);

    /* Dropdown (focusable, Enter/Space to expand) */
    tunix_widget *dropdown_label = tunix_label_create("Dropdown (Enter/Space, Arrow keys):");
    tunix_rect dropdown_label_bounds = {30, 13, 40, 1};
    tunix_widget_set_bounds(dropdown_label, dropdown_label_bounds);
    tunix_widget_add_child(panel, dropdown_label);

    const char *dropdown_items[] = {"Choice A", "Choice B", "Choice C"};
    tunix_widget *dropdown = tunix_dropdown_create(dropdown_items, 3);
    tunix_rect dropdown_bounds = {30, 14, 25, 3};
    tunix_widget_set_bounds(dropdown, dropdown_bounds);
    tunix_widget_set_on_change(dropdown, on_dropdown_change, NULL);
    tunix_widget_add_child(panel, dropdown);

    /* Status bar */
    status_label = tunix_label_create("Press TAB to cycle focus. ESC to exit.");
    tunix_rect status_bounds = {2, 21, 74, 1};
    tunix_widget_set_bounds(status_label, status_bounds);
    tunix_widget_add_child(panel, status_label);

    /* Quit button */
    tunix_widget *quit_btn = tunix_button_create("Quit (or press ESC)");
    tunix_rect btn_bounds = {30, 18, 22, 3};
    tunix_widget_set_bounds(quit_btn, btn_bounds);
    tunix_widget_set_on_click(quit_btn, on_quit, NULL);
    tunix_widget_add_child(panel, quit_btn);

    /* Run application */
    bool result = tunix_app_run(app);
    
    /* Cleanup */
    tunix_app_destroy(app);
    return result ? 0 : 1;
}
