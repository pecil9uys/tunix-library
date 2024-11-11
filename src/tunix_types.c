#include "tunix_types.h"
#include "tunix_platform.h"
#include <string.h>

#ifdef _MSC_VER
#define tunix_strcpy(dst, src) strcpy_s(dst, sizeof(dst), src)
#else
#define tunix_strcpy(dst, src) strcpy(dst, src)
#endif

tunix_theme tunix_theme_default(void) {
    tunix_theme theme = {0};
    
    theme.fg.type = TUNIX_COLOR_DEFAULT;
    theme.bg.type = TUNIX_COLOR_DEFAULT;
    theme.border_color.type = TUNIX_COLOR_DEFAULT;
    theme.border = TUNIX_BORDER_SINGLE;
    
    /* Check Unicode support and use ASCII fallback if needed */
    bool use_unicode = tunix_platform_supports_unicode();
    
    if (use_unicode) {
        /* Unicode box drawing characters */
        tunix_strcpy(theme.border_chars.top_left, "┌");
        tunix_strcpy(theme.border_chars.top_right, "┐");
        tunix_strcpy(theme.border_chars.bottom_left, "└");
        tunix_strcpy(theme.border_chars.bottom_right, "┘");
        tunix_strcpy(theme.border_chars.horizontal, "─");
        tunix_strcpy(theme.border_chars.vertical, "│");
    } else {
        /* ASCII fallback characters */
        tunix_strcpy(theme.border_chars.top_left, "+");
        tunix_strcpy(theme.border_chars.top_right, "+");
        tunix_strcpy(theme.border_chars.bottom_left, "+");
        tunix_strcpy(theme.border_chars.bottom_right, "+");
        tunix_strcpy(theme.border_chars.horizontal, "-");
        tunix_strcpy(theme.border_chars.vertical, "|");
    }
    
    theme.padding_top = 0;
    theme.padding_right = 0;
    theme.padding_bottom = 0;
    theme.padding_left = 0;
    
    theme.margin_top = 0;
    theme.margin_right = 0;
    theme.margin_bottom = 0;
    theme.margin_left = 0;
    
    return theme;
}
