#ifndef TUNIX_RENDERER_H
#define TUNIX_RENDERER_H

#include "tunix_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Cell in the render buffer */
typedef struct tunix_cell {
    char ch[8];
    tunix_style_color fg;
    tunix_style_color bg;
    bool bold;
    bool underline;
} tunix_cell;

/* Renderer context */
typedef struct tunix_renderer tunix_renderer;

/* Create/destroy renderer */
tunix_renderer* tunix_renderer_create(int width, int height);
void tunix_renderer_destroy(tunix_renderer *renderer);

/* Resize renderer */
void tunix_renderer_resize(tunix_renderer *renderer, int width, int height);

/* Get dimensions */
void tunix_renderer_get_size(tunix_renderer *renderer, int *width, int *height);

/* Drawing operations */
void tunix_renderer_clear(tunix_renderer *renderer, tunix_style_color bg);
void tunix_renderer_set_cell(tunix_renderer *renderer, int x, int y, const char *ch, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_draw_text(tunix_renderer *renderer, int x, int y, const char *text, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_draw_rect(tunix_renderer *renderer, tunix_rect rect, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_draw_border(tunix_renderer *renderer, tunix_rect rect, tunix_border_style style, const tunix_border_chars *chars, tunix_style_color color);
void tunix_renderer_draw_hline(tunix_renderer *renderer, int x, int y, int width, const char *ch, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_draw_vline(tunix_renderer *renderer, int x, int y, int height, const char *ch, tunix_style_color fg, tunix_style_color bg);

/* Render to screen */
void tunix_renderer_present(tunix_renderer *renderer);

/* Color helpers */
tunix_style_color tunix_color_from_type(tunix_color_type type);
tunix_style_color tunix_color_from_rgb(uint8_t r, uint8_t g, uint8_t b);
tunix_style_color tunix_color_interpolate(tunix_style_color start, tunix_style_color end, float t);

/* Gradient support */
void tunix_renderer_draw_gradient_rect(tunix_renderer *renderer, tunix_rect rect, 
                                       tunix_style_color start, tunix_style_color end, bool vertical);
void tunix_renderer_draw_gradient_text(tunix_renderer *renderer, int x, int y, 
                                       const char *text, tunix_style_color start, tunix_style_color end);

/* Advanced drawing primitives */
void tunix_renderer_draw_circle(tunix_renderer *renderer, int cx, int cy, int radius, 
                                const char *ch, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_draw_line(tunix_renderer *renderer, int x1, int y1, int x2, int y2, 
                              const char *ch, tunix_style_color fg, tunix_style_color bg);
void tunix_renderer_fill_rect(tunix_renderer *renderer, tunix_rect rect, 
                              const char *ch, tunix_style_color fg, tunix_style_color bg);

/* Shadow and effects */
void tunix_renderer_draw_shadow(tunix_renderer *renderer, tunix_rect rect, int offset);
void tunix_renderer_draw_glow(tunix_renderer *renderer, int x, int y, int radius, tunix_style_color color);

#ifdef __cplusplus
}
#endif

#endif /* TUNIX_RENDERER_H */
