#include "tunix.h"
#include <stdio.h>
#include <stdlib.h>

static void on_button_click(tunix_widget *widget, void *user_data) {
    (void)widget;
    tunix_app *app = (tunix_app*)user_data;
    printf("Button clicked!\n");
    tunix_app_quit(app);
}

static void on_checkbox_change(tunix_widget *widget, void *user_data) {
    (void)user_data;
    bool checked = tunix_checkbox_get_checked(widget);
    printf("Checkbox changed: %s\n", checked ? "checked" : "unchecked");
}

static void on_list_select(tunix_widget *widget, void *user_data) {
    (void)user_data;
    int selected = tunix_list_get_selected(widget);
    printf("List item selected: %d\n", selected);
}

int main(void) {
    /* Create application */
    tunix_app *app = tunix_app_create("Tunix Demo - C API");
    if (!app) {
        fprintf(stderr, "Failed to create app\n");
        return 1;
    }
    
    tunix_widget *root = tunix_app_root(app);
    
    /* Create main panel */
    tunix_widget *panel = tunix_panel_create();
    tunix_rect panel_bounds = {2, 2, 76, 20};
    tunix_widget_set_bounds(panel, panel_bounds);
    tunix_widget_add_child(root, panel);
    
    /* Add title label */
    tunix_widget *title = tunix_label_create("=== Tunix Terminal UI Framework ===");
    tunix_rect title_bounds = {4, 3, 40, 1};
    tunix_widget_set_bounds(title, title_bounds);
    title->theme.fg = tunix_color_from_type(TUNIX_COLOR_BRIGHT_CYAN);
    tunix_widget_add_child(panel, title);
    
    /* Add button */
    tunix_widget *button = tunix_button_create("Click Me to Exit");
    tunix_rect button_bounds = {4, 5, 20, 3};
    tunix_widget_set_bounds(button, button_bounds);
    button->theme.fg = tunix_color_from_type(TUNIX_COLOR_WHITE);
    button->theme.border_color = tunix_color_from_type(TUNIX_COLOR_GREEN);
    tunix_widget_set_on_click(button, on_button_click, app);
    tunix_widget_focus(button);
    tunix_widget_add_child(panel, button);
    
    /* Add checkbox */
    tunix_widget *checkbox = tunix_checkbox_create("Enable Feature", false);
    tunix_rect checkbox_bounds = {4, 9, 25, 1};
    tunix_widget_set_bounds(checkbox, checkbox_bounds);
    checkbox->theme.fg = tunix_color_from_type(TUNIX_COLOR_YELLOW);
    tunix_widget_set_on_change(checkbox, on_checkbox_change, NULL);
    tunix_widget_add_child(panel, checkbox);
    
    /* Add radio buttons */
    tunix_widget *radio1 = tunix_radio_create("Option A", 1);
    tunix_rect radio1_bounds = {4, 11, 20, 1};
    tunix_widget_set_bounds(radio1, radio1_bounds);
    radio1->theme.fg = tunix_color_from_type(TUNIX_COLOR_MAGENTA);
    tunix_widget_add_child(panel, radio1);
    
    tunix_widget *radio2 = tunix_radio_create("Option B", 1);
    tunix_rect radio2_bounds = {4, 12, 20, 1};
    tunix_widget_set_bounds(radio2, radio2_bounds);
    radio2->theme.fg = tunix_color_from_type(TUNIX_COLOR_MAGENTA);
    tunix_widget_add_child(panel, radio2);
    
    /* Add progress bar */
    tunix_widget *progress = tunix_progress_create(0.6f);
    tunix_rect progress_bounds = {4, 14, 30, 1};
    tunix_widget_set_bounds(progress, progress_bounds);
    tunix_widget_add_child(panel, progress);
    
    /* Add slider */
    tunix_widget *slider = tunix_slider_create(0.0f, 100.0f, 50.0f);
    tunix_rect slider_bounds = {4, 16, 30, 1};
    tunix_widget_set_bounds(slider, slider_bounds);
    slider->theme.fg = tunix_color_from_type(TUNIX_COLOR_CYAN);
    tunix_widget_add_child(panel, slider);
    
    /* Add list */
    const char *list_items[] = {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"};
    tunix_widget *list = tunix_list_create(list_items, 5);
    tunix_rect list_bounds = {40, 5, 25, 10};
    tunix_widget_set_bounds(list, list_bounds);
    list->theme.border_color = tunix_color_from_type(TUNIX_COLOR_BLUE);
    tunix_widget_set_on_change(list, on_list_select, NULL);
    tunix_widget_add_child(panel, list);
    
    /* Add textbox */
    tunix_widget *textbox = tunix_textbox_create("Type here...");
    tunix_rect textbox_bounds = {4, 18, 30, 3};
    tunix_widget_set_bounds(textbox, textbox_bounds);
    textbox->theme.border_color = tunix_color_from_type(TUNIX_COLOR_YELLOW);
    tunix_widget_add_child(panel, textbox);
    
    /* Add status bar */
    tunix_widget *statusbar = tunix_statusbar_create("Press ESC to exit | TAB to navigate");
    tunix_rect statusbar_bounds = {0, 23, 80, 1};
    tunix_widget_set_bounds(statusbar, statusbar_bounds);
    tunix_widget_add_child(root, statusbar);
    
    /* Run application */
    bool result = tunix_app_run(app);
    
    /* Cleanup */
    tunix_app_destroy(app);
    
    return result ? 0 : 1;
}
