/* spinner_demo.c - Spinner Widget Animation Demo */
#include "tunix.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define tunix_sleep(ms) Sleep(ms)
#else
#include <unistd.h>
#define tunix_sleep(ms) usleep((ms) * 1000)
#endif

static bool running = true;

static void on_quit(tunix_widget *widget, void *user_data) {
    (void)widget;
    tunix_app *app = (tunix_app*)user_data;
    tunix_app_quit(app);
    running = false;
}

int main(void) {
    /* Create application */
    tunix_app *app = tunix_app_create("Tunix Spinner Demo");
    if (!app) {
        fprintf(stderr, "Failed to create app\n");
        return 1;
    }

    tunix_widget *root = tunix_app_root(app);
    tunix_rect screen = {0, 0, 80, 25};
    tunix_widget_set_bounds(root, screen);

    /* Create panel for layout */
    tunix_widget *panel = tunix_panel_create();
    tunix_rect panel_bounds = {2, 2, 76, 20};
    tunix_widget_set_bounds(panel, panel_bounds);
    tunix_widget_add_child(root, panel);

    /* Create title */
    tunix_widget *title = tunix_label_create("Tunix Spinner Widget Demo - 7 Animation Styles");
    tunix_rect title_bounds = {1, 0, 74, 1};
    tunix_widget_set_bounds(title, title_bounds);
    tunix_widget_add_child(panel, title);

    /* Create spinner widgets */
    tunix_widget *spinner_dots = tunix_spinner_create(TUNIX_SPINNER_DOTS);
    tunix_widget *spinner_line = tunix_spinner_create(TUNIX_SPINNER_LINE);
    tunix_widget *spinner_circle = tunix_spinner_create(TUNIX_SPINNER_CIRCLE);
    tunix_widget *spinner_arrow = tunix_spinner_create(TUNIX_SPINNER_ARROW);
    tunix_widget *spinner_bounce = tunix_spinner_create(TUNIX_SPINNER_BOUNCE);
    tunix_widget *spinner_clock = tunix_spinner_create(TUNIX_SPINNER_CLOCK);
    tunix_widget *spinner_simple = tunix_spinner_create(TUNIX_SPINNER_DOTS_SIMPLE);

    /* Position spinners and add labels */
    const char *labels[] = {"DOTS:", "LINE:", "CIRCLE:", "ARROW:", "BOUNCE:", "CLOCK:", "SIMPLE:"};
    tunix_widget *spinners[] = {
        spinner_dots, spinner_line, spinner_circle, spinner_arrow,
        spinner_bounce, spinner_clock, spinner_simple
    };

    for (int i = 0; i < 7; i++) {
        /* Create label */
        tunix_widget *label = tunix_label_create(labels[i]);
        tunix_rect label_bounds = {10, 3 + i * 2, 20, 1};
        tunix_widget_set_bounds(label, label_bounds);
        tunix_widget_add_child(panel, label);

        /* Position and start spinner */
        tunix_rect spinner_bounds = {5, 3 + i * 2, 2, 1};
        tunix_widget_set_bounds(spinners[i], spinner_bounds);
        tunix_widget_add_child(panel, spinners[i]);
        tunix_spinner_start(spinners[i]);
    }

    /* Add footer note */
    tunix_widget *footer = tunix_label_create("ASCII-safe characters for all terminal types");
    tunix_rect footer_bounds = {1, 17, 74, 1};
    tunix_widget_set_bounds(footer, footer_bounds);
    tunix_widget_add_child(panel, footer);

    /* Add quit button */
    tunix_widget *quit_btn = tunix_button_create("Quit");
    tunix_rect btn_bounds = {35, 19, 10, 1};
    tunix_widget_set_bounds(quit_btn, btn_bounds);
    tunix_widget_add_child(panel, quit_btn);

    /* Animation loop - tick spinners manually */
    int frame_count = 0;
    while (running && frame_count < 150) {
        /* Tick all spinners to advance animation */
        for (int i = 0; i < 7; i++) {
            tunix_spinner_tick(spinners[i]);
        }

        /* Render one frame (this would normally be in event loop) */
        tunix_app_run(app);
        
        tunix_sleep(100); /* 100ms = 10 FPS */
        frame_count++;
    }

    /* Cleanup */
    tunix_app_destroy(app);
    return 0;
}
