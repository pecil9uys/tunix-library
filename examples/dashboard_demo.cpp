#include "tunix.hpp"
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

using namespace tunix;

std::string get_current_time() {
    time_t now = time(nullptr);
    tm* local_time = localtime(&now);
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << local_time->tm_hour << ":"
        << std::setw(2) << local_time->tm_min << ":"
        << std::setw(2) << local_time->tm_sec;
    return oss.str();
}

int main() {
    App app("System Dashboard");
    tunix_platform_set_title("Tunix - Dashboard Example");
    
    /* Header panel */
    auto header = std::make_shared<Panel>();
    header->bounds(1, 1, 78, 3)
           .border(TUNIX_BORDER_SINGLE)
           .border_color(Color::RGB(100, 200, 255));
    
    auto header_title = std::make_shared<Label>("▓▒░ SYSTEM DASHBOARD ░▒▓");
    header_title->bounds(25, 2, 30, 1)
                 .fg_color(Color::RGB(150, 200, 255));
    header->add(header_title);
    
    app.root().add(header);
    
    /* Left sidebar - System Info */
    auto sidebar = std::make_shared<Panel>();
    sidebar->bounds(1, 5, 25, 15)
            .border(TUNIX_BORDER_ROUNDED)
            .border_color(Color::Cyan());
    
    auto system_label = std::make_shared<Label>("SYSTEM INFO");
    system_label->bounds(3, 6, 20, 1)
                 .fg_color(Color::Yellow());
    sidebar->add(system_label);
    
    tunix_terminal_type term = tunix_platform_detect_terminal();
    std::string term_name = tunix_platform_get_terminal_name(term);
    auto term_info = std::make_shared<Label>("Terminal: " + term_name);
    term_info->bounds(3, 8, 22, 1)
              .fg_color(Color::Green());
    sidebar->add(term_info);
    
    auto unicode_info = std::make_shared<Label>(
        std::string("Unicode: ") + (tunix_platform_supports_unicode() ? "✓" : "✗")
    );
    unicode_info->bounds(3, 9, 22, 1)
                 .fg_color(Color::White());
    sidebar->add(unicode_info);
    
    auto color_info = std::make_shared<Label>(
        std::string("Truecolor: ") + (tunix_platform_supports_truecolor() ? "✓" : "✗")
    );
    color_info->bounds(3, 10, 22, 1)
               .fg_color(Color::White());
    sidebar->add(color_info);
    
    auto mouse_info = std::make_shared<Label>(
        std::string("Mouse: ") + (tunix_platform_supports_mouse() ? "✓" : "✗")
    );
    mouse_info->bounds(3, 11, 22, 1)
               .fg_color(Color::White());
    sidebar->add(mouse_info);
    
    app.root().add(sidebar);
    
    /* Center panel - Statistics */
    auto stats_panel = std::make_shared<Panel>();
    stats_panel->bounds(27, 5, 26, 15)
                .border(TUNIX_BORDER_DOUBLE)
                .border_color(Color::Magenta());
    
    auto stats_label = std::make_shared<Label>("STATISTICS");
    stats_label->bounds(29, 6, 20, 1)
                .fg_color(Color::Yellow());
    stats_panel->add(stats_label);
    
    auto cpu_label = std::make_shared<Label>("CPU Usage:");
    cpu_label->bounds(29, 8, 15, 1)
              .fg_color(Color::Cyan());
    stats_panel->add(cpu_label);
    
    auto cpu_progress = std::make_shared<Progress>(0.65f);
    cpu_progress->bounds(29, 9, 20, 1)
                 .fg_color(Color::RGB(0, 255, 0));
    stats_panel->add(cpu_progress);
    
    auto mem_label = std::make_shared<Label>("Memory:");
    mem_label->bounds(29, 11, 15, 1)
              .fg_color(Color::Cyan());
    stats_panel->add(mem_label);
    
    auto mem_progress = std::make_shared<Progress>(0.48f);
    mem_progress->bounds(29, 12, 20, 1)
                 .fg_color(Color::RGB(255, 200, 0));
    stats_panel->add(mem_progress);
    
    auto disk_label = std::make_shared<Label>("Disk:");
    disk_label->bounds(29, 14, 15, 1)
               .fg_color(Color::Cyan());
    stats_panel->add(disk_label);
    
    auto disk_progress = std::make_shared<Progress>(0.82f);
    disk_progress->bounds(29, 15, 20, 1)
                  .fg_color(Color::RGB(255, 128, 0));
    stats_panel->add(disk_progress);
    
    auto network_label = std::make_shared<Label>("Network:");
    network_label->bounds(29, 17, 15, 1)
                  .fg_color(Color::Cyan());
    stats_panel->add(network_label);
    
    auto network_progress = std::make_shared<Progress>(0.34f);
    network_progress->bounds(29, 18, 20, 1)
                     .fg_color(Color::RGB(0, 255, 255));
    stats_panel->add(network_progress);
    
    app.root().add(stats_panel);
    
    /* Right panel - Quick Actions */
    auto actions_panel = std::make_shared<Panel>();
    actions_panel->bounds(54, 5, 25, 15)
                  .border(TUNIX_BORDER_ROUNDED)
                  .border_color(Color::Green());
    
    auto actions_label = std::make_shared<Label>("QUICK ACTIONS");
    actions_label->bounds(56, 6, 20, 1)
                  .fg_color(Color::Yellow());
    actions_panel->add(actions_label);
    
    auto refresh_btn = std::make_shared<Button>("⟲ Refresh");
    refresh_btn->bounds(56, 8, 20, 2)
                .fg_color(Color::RGB(0, 255, 128))
                .border_color(Color::Green())
                .on_click([]() {
                    std::cout << "Refreshing dashboard..." << std::endl;
                    tunix_platform_bell();
                });
    actions_panel->add(refresh_btn);
    
    auto export_btn = std::make_shared<Button>("⬇ Export");
    export_btn->bounds(56, 11, 20, 2)
               .fg_color(Color::RGB(255, 200, 0))
               .border_color(Color::Yellow())
               .on_click([]() {
                   std::cout << "Exporting data..." << std::endl;
               });
    actions_panel->add(export_btn);
    
    auto settings_btn = std::make_shared<Button>("⚙ Settings");
    settings_btn->bounds(56, 14, 20, 2)
                 .fg_color(Color::RGB(200, 200, 200))
                 .border_color(Color::White())
                 .on_click([]() {
                     std::cout << "Opening settings..." << std::endl;
                 });
    actions_panel->add(settings_btn);
    
    auto quit_btn = std::make_shared<Button>("✗ Quit");
    quit_btn->bounds(56, 17, 20, 2)
             .fg_color(Color::RGB(255, 100, 100))
             .border_color(Color::Red())
             .on_click([&]() {
                 std::cout << "Exiting dashboard..." << std::endl;
                 app.quit();
             });
    actions_panel->add(quit_btn);
    
    app.root().add(actions_panel);
    
    /* Bottom status bar */
    std::string status_text = "System Time: " + get_current_time() + 
                             " | Press F5 to Refresh | ESC to Exit";
    auto statusbar = std::make_shared<Statusbar>(status_text);
    statusbar->bounds(0, 23, 80, 1)
              .fg_color(Color::Black())
              .bg_color(Color::RGB(200, 200, 200));
    app.root().add(statusbar);
    
    /* Alert notification */
    auto alert = std::make_shared<Notification>("⚠ System Running");
    alert->bounds(28, 21, 24, 2)
         .fg_color(Color::RGB(255, 255, 0))
         .border_color(Color::RGB(255, 128, 0));
    app.root().add(alert);
    
    std::cout << "Dashboard initialized at " << get_current_time() << std::endl;
    
    return app.run() ? 0 : 1;
}
