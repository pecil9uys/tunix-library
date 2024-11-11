#include "tunix.hpp"
#include <memory>
#include <iostream>
#include <string>

using namespace tunix;

/* Form state */
struct FormData {
    std::string name;
    std::string email;
    std::string country;
    bool newsletter = false;
    bool terms = false;
};

int main() {
    App app("User Registration Form");
    tunix_platform_set_title("Tunix - Form Example");
    
    FormData form_data;
    
    /* Main form panel */
    auto form_panel = std::make_shared<Panel>();
    form_panel->bounds(10, 2, 60, 18)
               .border(TUNIX_BORDER_DOUBLE)
               .border_color(Color::RGB(100, 150, 255));
    
    /* Form title */
    auto title = std::make_shared<Label>("═══ USER REGISTRATION ═══");
    title->bounds(12, 3, 40, 1)
          .fg_color(Color::RGB(150, 200, 255));
    form_panel->add(title);
    
    /* Name field */
    auto name_label = std::make_shared<Label>("Full Name:");
    name_label->bounds(12, 5, 15, 1)
               .fg_color(Color::Yellow());
    form_panel->add(name_label);
    
    auto name_input = std::make_shared<Textbox>("");
    name_input->bounds(12, 6, 45, 3)
               .border_color(Color::Cyan());
    form_panel->add(name_input);
    
    /* Email field */
    auto email_label = std::make_shared<Label>("Email Address:");
    email_label->bounds(12, 9, 20, 1)
                .fg_color(Color::Yellow());
    form_panel->add(email_label);
    
    auto email_input = std::make_shared<Textbox>("");
    email_input->bounds(12, 10, 45, 3)
                .border_color(Color::Cyan());
    form_panel->add(email_input);
    
    /* Country dropdown */
    auto country_label = std::make_shared<Label>("Country:");
    country_label->bounds(12, 13, 15, 1)
                  .fg_color(Color::Yellow());
    form_panel->add(country_label);
    
    auto country_dropdown = std::make_shared<Dropdown>(std::vector<std::string>{
        "United States", "Canada", "United Kingdom", "Germany", "France", 
        "Japan", "Australia", "Brazil", "India", "Other"
    });
    country_dropdown->bounds(12, 14, 30, 3)
                     .border_color(Color::Green());
    form_panel->add(country_dropdown);
    
    /* Checkboxes */
    auto newsletter_check = std::make_shared<Checkbox>("Subscribe to newsletter", false);
    newsletter_check->bounds(12, 17, 35, 1)
                     .fg_color(Color::Magenta());
    form_panel->add(newsletter_check);
    
    auto terms_check = std::make_shared<Checkbox>("I agree to terms & conditions", false);
    terms_check->bounds(12, 18, 35, 1)
                .fg_color(Color::Magenta());
    form_panel->add(terms_check);
    
    /* Buttons */
    auto submit_btn = std::make_shared<Button>("✓ Submit");
    submit_btn->bounds(12, 20, 15, 3)
               .fg_color(Color::RGB(0, 255, 0))
               .border_color(Color::Green())
               .on_click([&]() {
                   form_data.name = name_input->get_text();
                   form_data.email = email_input->get_text();
                   form_data.country = std::to_string(country_dropdown->get_selected());
                   form_data.newsletter = newsletter_check->is_checked();
                   form_data.terms = terms_check->is_checked();
                   
                   std::cout << "\n=== Form Submitted ===" << std::endl;
                   std::cout << "Name: " << form_data.name << std::endl;
                   std::cout << "Email: " << form_data.email << std::endl;
                   std::cout << "Newsletter: " << (form_data.newsletter ? "Yes" : "No") << std::endl;
                   std::cout << "Terms: " << (form_data.terms ? "Yes" : "No") << std::endl;
                   
                   if (!form_data.terms) {
                       std::cout << "ERROR: Must accept terms & conditions!" << std::endl;
                       tunix_platform_bell();
                   }
               });
    form_panel->add(submit_btn);
    
    auto cancel_btn = std::make_shared<Button>("✗ Cancel");
    cancel_btn->bounds(30, 20, 15, 3)
               .fg_color(Color::RGB(255, 100, 100))
               .border_color(Color::Red())
               .on_click([&]() {
                   std::cout << "Form cancelled!" << std::endl;
                   app.quit();
               });
    form_panel->add(cancel_btn);
    
    auto clear_btn = std::make_shared<Button>("⟲ Clear");
    clear_btn->bounds(48, 20, 15, 3)
              .fg_color(Color::Yellow())
              .border_color(Color::RGB(255, 200, 0))
              .on_click([&]() {
                  std::cout << "Form cleared!" << std::endl;
                  name_input->text("");
                  email_input->text("");
                  country_dropdown->selected(0);
                  newsletter_check->checked(false);
                  terms_check->checked(false);
              });
    form_panel->add(clear_btn);
    
    /* Add form to root */
    app.root().add(form_panel);
    
    /* Status bar */
    auto statusbar = std::make_shared<Statusbar>(
        "TAB: Next Field | SHIFT+TAB: Previous | ENTER: Submit | ESC: Cancel"
    );
    statusbar->bounds(0, 23, 80, 1);
    app.root().add(statusbar);
    
    /* Info box */
    auto info = std::make_shared<Notification>("Fill all fields to continue");
    info->bounds(20, 0, 40, 2)
         .fg_color(Color::Cyan())
         .border_color(Color::Blue());
    app.root().add(info);
    
    return app.run() ? 0 : 1;
}
