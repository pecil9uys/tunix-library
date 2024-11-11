#include "tunix.hpp"
#include <memory>
#include <iostream>

using namespace tunix;

int main() {
    // Create application with fluent API
    App app("Tunix Demo - C++ API");
    
    // Create main panel
    auto panel = std::make_shared<Panel>();
    panel->bounds(2, 2, 76, 20)
          .border(TUNIX_BORDER_SINGLE)
          .border_color(Color::Cyan());
    
    // Add title
    auto title = std::make_shared<Label>("=== Tunix C++ Fluent API ===");
    title->bounds(4, 3, 40, 1)
          .fg_color(Color::RGB(100, 200, 255));
    panel->add(title);
    
    // Add button with callback
    auto button = std::make_shared<Button>("Click to Exit");
    button->bounds(4, 5, 20, 3)
          .fg_color(Color::White())
          .border_color(Color::Green())
          .on_click([&app]() {
              std::cout << "Button clicked! Exiting...\n";
              app.quit();
          });
    panel->add(button);
    
    // Add checkbox
    auto checkbox = std::make_shared<Checkbox>("Enable Dark Mode", false);
    checkbox->bounds(4, 9, 25, 1)
            .fg_color(Color::Yellow())
            .on_click([]() {
                std::cout << "Checkbox toggled!\n";
            });
    panel->add(checkbox);
    
    // Add radio buttons
    auto radio1 = std::make_shared<Radio>("Speed", 1);
    radio1->bounds(4, 11, 20, 1).fg_color(Color::Magenta());
    panel->add(radio1);
    
    auto radio2 = std::make_shared<Radio>("Quality", 1);
    radio2->bounds(4, 12, 20, 1).fg_color(Color::Magenta());
    panel->add(radio2);
    
    // Add progress bar
    auto progress = std::make_shared<Progress>(0.75f);
    progress->bounds(4, 14, 30, 1);
    panel->add(progress);
    
    // Add slider
    auto slider = std::make_shared<Slider>(0.0f, 100.0f, 65.0f);
    slider->bounds(4, 16, 30, 1).fg_color(Color::Cyan());
    panel->add(slider);
    
    // Add list with callback
    auto list = std::make_shared<List>(std::vector<std::string>{
        "Dashboard", "Settings", "Profile", "Messages", "Logout"
    });
    list->bounds(40, 5, 25, 10)
        .border_color(Color::Blue())
        .on_change([](int index) {
            std::cout << "Selected item: " << index << "\n";
        });
    panel->add(list);
    
    // Add textbox
    auto textbox = std::make_shared<Textbox>("Enter text...");
    textbox->bounds(4, 18, 30, 3)
           .border_color(Color::Yellow());
    panel->add(textbox);
    
    // Add dropdown
    auto dropdown = std::make_shared<Dropdown>(std::vector<std::string>{
        "English", "Spanish", "French", "German"
    });
    dropdown->bounds(40, 16, 20, 3)
            .border_color(Color::Green());
    panel->add(dropdown);
    
    // Add tabs
    auto tabs = std::make_shared<Tabs>(std::vector<std::string>{
        "Home", "Data", "Settings"
    });
    tabs->bounds(2, 0, 50, 1)
        .fg_color(Color::White());
    tabs->active(0);
    app.root().add(tabs);
    
    // Add panel to root
    app.root().add(panel);
    
    // Add status bar
    auto statusbar = std::make_shared<Statusbar>("Press ESC to exit | TAB to navigate | C++20 API");
    statusbar->bounds(0, 23, 80, 1);
    app.root().add(statusbar);
    
    // Add notification
    auto notification = std::make_shared<Notification>("Welcome to Tunix!");
    notification->bounds(25, 10, 30, 3);
    app.root().add(notification);
    
    // Run the application
    return app.run() ? 0 : 1;
}
