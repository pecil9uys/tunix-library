#include "tunix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void on_gradient_button(tunix_widget *widget, void *user_data) {
    (void)widget;
    (void)user_data;
    printf("Gradient button clicked!\n");
}

int main(void) {
    /* Create application */
    tunix_app *app = tunix_app_create("Advanced Features Demo");
    if (!app) {
        fprintf(stderr, "Failed to create app\n");
        return 1;
    }
    
    /* Detect and display terminal information */
    tunix_terminal_type term_type = tunix_platform_detect_terminal();
    printf("Terminal: %s\n", tunix_platform_get_terminal_name(term_type));
    printf("Unicode Support: %s\n", tunix_platform_supports_unicode() ? "Yes" : "No");
    printf("Truecolor Support: %s\n", tunix_platform_supports_truecolor() ? "Yes" : "No");
    printf("Mouse Support: %s\n", tunix_platform_supports_mouse() ? "Yes" : "No");
    
    /* Set custom window title */
    tunix_platform_set_title("Tunix - Advanced Features Demo");
    
    tunix_widget *root = tunix_app_root(app);
    
    /* Create main panel with shadow effect */
    tunix_widget *panel = tunix_panel_create();
    tunix_rect panel_bounds = {5, 3, 70, 18};
    tunix_widget_set_bounds(panel, panel_bounds);
    panel->theme.border = TUNIX_BORDER_DOUBLE;
    panel->theme.border_color = tunix_color_from_rgb(100, 150, 255);
    tunix_widget_add_child(root, panel);
    
    /* Title with gradient effect */
    tunix_widget *title = tunix_label_create("=== ADVANCED FEATURES ===");
    tunix_rect title_bounds = {7, 4, 40, 1};
    tunix_widget_set_bounds(title, title_bounds);
    title->theme.fg = tunix_color_from_rgb(255, 100, 200);
    tunix_widget_add_child(panel, title);
    
    /* Gradient demo section */
    tunix_widget *gradient_label = tunix_label_create("Gradient Colors:");
    tunix_rect gl_bounds = {7, 6, 20, 1};
    tunix_widget_set_bounds(gradient_label, gl_bounds);
    gradient_label->theme.fg = tunix_color_from_type(TUNIX_COLOR_YELLOW);
    tunix_widget_add_child(panel, gradient_label);
    
    /* Custom rendering for gradient button */
    tunix_widget *gradient_btn = tunix_button_create("Gradient Button");
    tunix_rect gb_bounds = {7, 7, 20, 3};
    tunix_widget_set_bounds(gradient_btn, gb_bounds);
    gradient_btn->theme.fg = tunix_color_from_rgb(0, 255, 128);
    gradient_btn->theme.border_color = tunix_color_from_rgb(128, 0, 255);
    tunix_widget_set_on_click(gradient_btn, on_gradient_button, NULL);
    tunix_widget_add_child(panel, gradient_btn);
    
    /* Terminal info display */
    char term_info[256];
    snprintf(term_info, sizeof(term_info), "Terminal: %s", 
             tunix_platform_get_terminal_name(term_type));
    tunix_widget *info1 = tunix_label_create(term_info);
    tunix_rect info1_bounds = {7, 11, 60, 1};
    tunix_widget_set_bounds(info1, info1_bounds);
    info1->theme.fg = tunix_color_from_type(TUNIX_COLOR_CYAN);
    tunix_widget_add_child(panel, info1);
    
    snprintf(term_info, sizeof(term_info), "Unicode: %s | Truecolor: %s | Mouse: %s",
             tunix_platform_supports_unicode() ? "✓" : "✗",
             tunix_platform_supports_truecolor() ? "✓" : "✗",
             tunix_platform_supports_mouse() ? "✓" : "✗");
    tunix_widget *info2 = tunix_label_create(term_info);
    tunix_rect info2_bounds = {7, 12, 60, 1};
    tunix_widget_set_bounds(info2, info2_bounds);
    info2->theme.fg = tunix_color_from_type(TUNIX_COLOR_GREEN);
    tunix_widget_add_child(panel, info2);
    
    /* Cursor style demo */
    tunix_widget *cursor_label = tunix_label_create("Cursor Styles Available:");
    tunix_rect cl_bounds = {7, 14, 30, 1};
    tunix_widget_set_bounds(cursor_label, cl_bounds);
    cursor_label->theme.fg = tunix_color_from_type(TUNIX_COLOR_MAGENTA);
    tunix_widget_add_child(panel, cursor_label);
    
    tunix_widget *cursor_info = tunix_label_create("Block, Underline, Bar (Blinking/Steady)");
    tunix_rect ci_bounds = {7, 15, 45, 1};
    tunix_widget_set_bounds(cursor_info, ci_bounds);
    cursor_info->theme.fg = tunix_color_from_type(TUNIX_COLOR_WHITE);
    tunix_widget_add_child(panel, cursor_info);
    
    /* Progress bar with custom color */
    tunix_widget *progress = tunix_progress_create(0.8f);
    tunix_rect prog_bounds = {7, 17, 50, 1};
    tunix_widget_set_bounds(progress, prog_bounds);
    progress->theme.fg = tunix_color_from_rgb(0, 255, 0);
    progress->theme.bg = tunix_color_from_rgb(50, 50, 50);
    tunix_widget_add_child(panel, progress);
    
    /* Status bar */
    tunix_widget *statusbar = tunix_statusbar_create("Press ESC to exit | F1 for help | Advanced Features v1.0");
    tunix_rect status_bounds = {0, 23, 80, 1};
    tunix_widget_set_bounds(statusbar, status_bounds);
    statusbar->theme.fg = tunix_color_from_type(TUNIX_COLOR_BLACK);
    statusbar->theme.bg = tunix_color_from_type(TUNIX_COLOR_BRIGHT_WHITE);
    tunix_widget_add_child(root, statusbar);
    
    /* Set cursor style to bar */
    tunix_platform_set_cursor_style(TUNIX_CURSOR_BAR_STEADY);
    
    /* Ring bell on startup */
    tunix_platform_bell();
    
    /* Run application */
    bool result = tunix_app_run(app);
    
    /* Cleanup */
    tunix_app_destroy(app);
    
    return result ? 0 : 1;
}
