#include "tunix.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <vector>

using namespace tunix;

int main() {
    /* Display terminal capabilities */
    tunix_terminal_type term_type = tunix_platform_detect_terminal();
    std::cout << "=== Terminal Information ===" << std::endl;
    std::cout << "Type: " << tunix_platform_get_terminal_name(term_type) << std::endl;
    std::cout << "Unicode: " << (tunix_platform_supports_unicode() ? "Yes" : "No") << std::endl;
    std::cout << "Truecolor: " << (tunix_platform_supports_truecolor() ? "Yes" : "No") << std::endl;
    std::cout << "Mouse: " << (tunix_platform_supports_mouse() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    /* Create app with custom title */
    App app("Tunix C++ Advanced Demo");
    tunix_platform_set_title("Tunix - C++ Advanced Features");
    
    /* Main container */
    auto container = std::make_shared<Panel>();
    container->bounds(3, 2, 74, 19)
              .border(TUNIX_BORDER_ROUNDED)
              .border_color(Color::RGB(100, 200, 255));
    
    /* Gradient title */
    auto title = std::make_shared<Label>("▓▒░ ADVANCED C++ API ░▒▓");
    title->bounds(5, 3, 40, 1)
          .fg_color(Color::RGB(255, 150, 255));
    container->add(title);
    
    /* Feature showcase */
    auto feature_label = std::make_shared<Label>("✦ Enhanced Features:");
    feature_label->bounds(5, 5, 30, 1)
                  .fg_color(Color::Yellow());
    container->add(feature_label);
    
    /* List of features */
    std::vector<std::string> features = {
        "→ Terminal Detection",
        "→ Gradient Colors",
        "→ Custom Cursor Styles",
        "→ Advanced Drawing",
        "→ Shadow & Glow Effects",
        "→ External Terminal Spawn"
    };
    
    auto feature_list = std::make_shared<List>(features);
    feature_list->bounds(5, 6, 35, 8)
                 .border_color(Color::Cyan())
                 .fg_color(Color::RGB(150, 255, 150));
    container->add(feature_list);
    
    /* Interactive section */
    auto button = std::make_shared<Button>("✓ Test Features");
    button->bounds(45, 6, 25, 3)
          .fg_color(Color::RGB(0, 255, 128))
          .border_color(Color::RGB(255, 100, 0))
          .on_click([]() {
              std::cout << "Feature test activated!" << std::endl;
              tunix_platform_bell();
          });
    container->add(button);
    
    /* Slider with custom colors */
    auto slider = std::make_shared<Slider>(0.0f, 100.0f, 75.0f);
    slider->bounds(45, 10, 25, 1)
          .fg_color(Color::RGB(255, 200, 0));
    container->add(slider);
    
    /* Checkbox group */
    auto check1 = std::make_shared<Checkbox>("✦ Enable Shadows", true);
    check1->bounds(45, 12, 25, 1)
           .fg_color(Color::Magenta());
    container->add(check1);
    
    auto check2 = std::make_shared<Checkbox>("✦ Enable Gradients", true);
    check2->bounds(45, 13, 25, 1)
           .fg_color(Color::Magenta());
    container->add(check2);
    
    /* Progress indicator */
    auto progress = std::make_shared<Progress>(0.9f);
    progress->bounds(5, 15, 65, 1)
             .fg_color(Color::RGB(0, 255, 255));
    container->add(progress);
    
    /* Info panel */
    std::string term_info = "Terminal: " + 
                           std::string(tunix_platform_get_terminal_name(term_type));
    auto info = std::make_shared<Label>(term_info);
    info->bounds(5, 17, 65, 1)
        .fg_color(Color::RGB(200, 200, 200));
    container->add(info);
    
    /* Add container to root */
    app.root().add(container);
    
    /* Status bar */
    auto statusbar = std::make_shared<Statusbar>(
        "ESC: Exit | TAB: Navigate | C++20 Fluent API | Advanced Features"
    );
    statusbar->bounds(0, 23, 80, 1)
              .fg_color(Color::Black())
              .bg_color(Color::RGB(255, 255, 255));
    app.root().add(statusbar);
    
    /* Notification */
    auto notification = std::make_shared<Notification>("⚡ Advanced Mode Activated!");
    notification->bounds(25, 1, 30, 3)
                 .fg_color(Color::RGB(255, 255, 0))
                 .border_color(Color::RGB(255, 128, 0));
    app.root().add(notification);
    
    /* Set custom cursor style */
    tunix_platform_set_cursor_style(TUNIX_CURSOR_UNDERLINE_STEADY);
    
    /* Run application */
    return app.run() ? 0 : 1;
}
