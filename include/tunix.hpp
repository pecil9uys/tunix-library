#ifndef TUNIX_HPP
#define TUNIX_HPP

#include "tunix.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace tunix {

/* Forward declarations */
class Widget;
class App;

/* Callback types */
using Callback = std::function<void()>;
using ChangeCallback = std::function<void(int)>;

/* Color helper */
class Color {
public:
    static tunix_style_color Default() { return tunix_color_from_type(TUNIX_COLOR_DEFAULT); }
    static tunix_style_color Black() { return tunix_color_from_type(TUNIX_COLOR_BLACK); }
    static tunix_style_color Red() { return tunix_color_from_type(TUNIX_COLOR_RED); }
    static tunix_style_color Green() { return tunix_color_from_type(TUNIX_COLOR_GREEN); }
    static tunix_style_color Yellow() { return tunix_color_from_type(TUNIX_COLOR_YELLOW); }
    static tunix_style_color Blue() { return tunix_color_from_type(TUNIX_COLOR_BLUE); }
    static tunix_style_color Magenta() { return tunix_color_from_type(TUNIX_COLOR_MAGENTA); }
    static tunix_style_color Cyan() { return tunix_color_from_type(TUNIX_COLOR_CYAN); }
    static tunix_style_color White() { return tunix_color_from_type(TUNIX_COLOR_WHITE); }
    static tunix_style_color RGB(uint8_t r, uint8_t g, uint8_t b) { return tunix_color_from_rgb(r, g, b); }
};

/* Widget wrapper */
class Widget {
protected:
    tunix_widget* m_widget;
    bool m_owns_widget;
    std::vector<std::shared_ptr<Widget>> m_children;
    std::shared_ptr<Callback> m_click_callback;
    std::shared_ptr<ChangeCallback> m_change_callback;

public:
    Widget(tunix_widget* widget, bool owns = true) 
        : m_widget(widget), m_owns_widget(owns) {}
    
    virtual ~Widget() {
        if (m_owns_widget && m_widget) {
            tunix_widget_destroy(m_widget);
        }
    }

    tunix_widget* handle() const { return m_widget; }

    Widget& id(const std::string& id) {
        tunix_widget_set_id(m_widget, id.c_str());
        return *this;
    }

    Widget& bounds(int x, int y, int width, int height) {
        tunix_rect rect = {x, y, width, height};
        tunix_widget_set_bounds(m_widget, rect);
        return *this;
    }

    Widget& visible(bool v) {
        tunix_widget_set_visible(m_widget, v);
        return *this;
    }

    Widget& enabled(bool e) {
        tunix_widget_set_enabled(m_widget, e);
        return *this;
    }

    Widget& focusable(bool f) {
        tunix_widget_set_focusable(m_widget, f);
        return *this;
    }

    Widget& fg_color(tunix_style_color color) {
        m_widget->theme.fg = color;
        return *this;
    }

    Widget& bg_color(tunix_style_color color) {
        m_widget->theme.bg = color;
        return *this;
    }

    Widget& border_color(tunix_style_color color) {
        m_widget->theme.border_color = color;
        return *this;
    }

    Widget& border(tunix_border_style style) {
        m_widget->theme.border = style;
        return *this;
    }

    Widget& padding(int top, int right, int bottom, int left) {
        m_widget->theme.padding_top = top;
        m_widget->theme.padding_right = right;
        m_widget->theme.padding_bottom = bottom;
        m_widget->theme.padding_left = left;
        return *this;
    }

    Widget& margin(int top, int right, int bottom, int left) {
        m_widget->theme.margin_top = top;
        m_widget->theme.margin_right = right;
        m_widget->theme.margin_bottom = bottom;
        m_widget->theme.margin_left = left;
        return *this;
    }

    Widget& on_click(Callback callback) {
        m_click_callback = std::make_shared<Callback>(callback);
        tunix_widget_set_on_click(m_widget, 
            [](tunix_widget*, void* user_data) {
                auto* cb = static_cast<Callback*>(user_data);
                if (cb) (*cb)();
            }, 
            m_click_callback.get());
        return *this;
    }

    Widget& on_change(ChangeCallback callback) {
        m_change_callback = std::make_shared<ChangeCallback>(callback);
        tunix_widget_set_on_change(m_widget,
            [](tunix_widget* w, void* user_data) {
                auto* cb = static_cast<ChangeCallback*>(user_data);
                if (cb) {
                    int value = 0;
                    if (w->type == TUNIX_WIDGET_LIST) {
                        value = tunix_list_get_selected(w);
                    } else if (w->type == TUNIX_WIDGET_DROPDOWN) {
                        value = tunix_dropdown_get_selected(w);
                    }
                    (*cb)(value);
                }
            },
            m_change_callback.get());
        return *this;
    }

    template<typename T>
    T& add(std::shared_ptr<T> child) {
        m_children.push_back(child);
        tunix_widget_add_child(m_widget, child->handle());
        return *child;
    }
};

/* Specific widget classes */
class Label : public Widget {
public:
    Label(const std::string& text) 
        : Widget(tunix_label_create(text.c_str())) {}

    Label& text(const std::string& t) {
        tunix_label_set_text(m_widget, t.c_str());
        return *this;
    }
};

class Button : public Widget {
public:
    Button(const std::string& text) 
        : Widget(tunix_button_create(text.c_str())) {}

    Button& text(const std::string& t) {
        tunix_button_set_text(m_widget, t.c_str());
        return *this;
    }
};

class Checkbox : public Widget {
public:
    Checkbox(const std::string& text, bool checked = false) 
        : Widget(tunix_checkbox_create(text.c_str(), checked)) {}

    Checkbox& checked(bool c) {
        tunix_checkbox_set_checked(m_widget, c);
        return *this;
    }

    bool is_checked() const {
        return tunix_checkbox_get_checked(m_widget);
    }
};

class Radio : public Widget {
public:
    Radio(const std::string& text, int group_id) 
        : Widget(tunix_radio_create(text.c_str(), group_id)) {}
};

class Progress : public Widget {
public:
    Progress(float value = 0.0f) 
        : Widget(tunix_progress_create(value)) {}

    Progress& value(float v) {
        tunix_progress_set_value(m_widget, v);
        return *this;
    }
};

class Slider : public Widget {
public:
    Slider(float min, float max, float value) 
        : Widget(tunix_slider_create(min, max, value)) {}

    Slider& value(float v) {
        tunix_slider_set_value(m_widget, v);
        return *this;
    }

    float get_value() const {
        return tunix_slider_get_value(m_widget);
    }
};

class Textbox : public Widget {
public:
    Textbox(const std::string& text = "") 
        : Widget(tunix_textbox_create(text.c_str())) {}

    Textbox& text(const std::string& t) {
        tunix_textbox_set_text(m_widget, t.c_str());
        return *this;
    }

    std::string get_text() const {
        const char* t = tunix_textbox_get_text(m_widget);
        return t ? std::string(t) : "";
    }
};

class Editor : public Widget {
public:
    Editor(const std::string& text = "") 
        : Widget(tunix_editor_create(text.c_str())) {}

    Editor& text(const std::string& t) {
        tunix_editor_set_text(m_widget, t.c_str());
        return *this;
    }

    std::string get_text() const {
        const char* t = tunix_editor_get_text(m_widget);
        return t ? std::string(t) : "";
    }
};

class List : public Widget {
public:
    List(const std::vector<std::string>& items) 
        : Widget(nullptr), m_items(items) {
        std::vector<const char*> c_items;
        for (const auto& item : m_items) {
            c_items.push_back(item.c_str());
        }
        m_widget = tunix_list_create(c_items.data(), (int)c_items.size());
    }

    List& selected(int index) {
        tunix_list_set_selected(m_widget, index);
        return *this;
    }

    int get_selected() const {
        return tunix_list_get_selected(m_widget);
    }

private:
    std::vector<std::string> m_items;
};

class Dropdown : public Widget {
public:
    Dropdown(const std::vector<std::string>& items) 
        : Widget(nullptr), m_items(items) {
        std::vector<const char*> c_items;
        for (const auto& item : m_items) {
            c_items.push_back(item.c_str());
        }
        m_widget = tunix_dropdown_create(c_items.data(), (int)c_items.size());
    }

    Dropdown& selected(int index) {
        tunix_dropdown_set_selected(m_widget, index);
        return *this;
    }

    int get_selected() const {
        return tunix_dropdown_get_selected(m_widget);
    }

private:
    std::vector<std::string> m_items;
};

class Menu : public Widget {
public:
    Menu(const std::vector<std::string>& items) 
        : Widget(nullptr), m_items(items) {
        std::vector<const char*> c_items;
        for (const auto& item : m_items) {
            c_items.push_back(item.c_str());
        }
        m_widget = tunix_menu_create(c_items.data(), (int)c_items.size());
    }

private:
    std::vector<std::string> m_items;
};

class Tabs : public Widget {
public:
    Tabs(const std::vector<std::string>& tab_names) 
        : Widget(nullptr), m_tab_names(tab_names) {
        std::vector<const char*> c_names;
        for (const auto& name : m_tab_names) {
            c_names.push_back(name.c_str());
        }
        m_widget = tunix_tabs_create(c_names.data(), (int)c_names.size());
    }

    Tabs& active(int index) {
        tunix_tabs_set_active(m_widget, index);
        return *this;
    }

    int get_active() const {
        return tunix_tabs_get_active(m_widget);
    }

private:
    std::vector<std::string> m_tab_names;
};

class Table : public Widget {
public:
    Table(int rows, int cols) 
        : Widget(tunix_table_create(rows, cols)) {}

    Table& set_cell(int row, int col, const std::string& text) {
        tunix_table_set_cell(m_widget, row, col, text.c_str());
        return *this;
    }

    std::string get_cell(int row, int col) const {
        const char* text = tunix_table_get_cell(m_widget, row, col);
        return text ? std::string(text) : "";
    }
};

class Statusbar : public Widget {
public:
    Statusbar(const std::string& text = "") 
        : Widget(tunix_statusbar_create(text.c_str())) {}
};

class Notification : public Widget {
public:
    Notification(const std::string& text) 
        : Widget(tunix_notification_create(text.c_str())) {}
};

class Separator : public Widget {
public:
    Separator(bool horizontal = true) 
        : Widget(tunix_separator_create(horizontal)) {}
};

class Panel : public Widget {
public:
    Panel() : Widget(tunix_panel_create()) {}
};

class VStack : public Widget {
public:
    VStack() : Widget(tunix_vstack_create()) {}
};

class HStack : public Widget {
public:
    HStack() : Widget(tunix_hstack_create()) {}
};

class Grid : public Widget {
public:
    Grid(int rows, int cols) 
        : Widget(tunix_grid_create(rows, cols)) {}
};

/* Application builder */
class App {
private:
    tunix_app* m_app;
    std::shared_ptr<Widget> m_root_widget;

public:
    App(const std::string& title = "Tunix App") {
        m_app = tunix_app_create(title.c_str());
        m_root_widget = std::make_shared<Widget>(tunix_app_root(m_app), false);
    }

    ~App() {
        if (m_app) {
            tunix_app_destroy(m_app);
        }
    }

    template<typename T>
    T& add(std::shared_ptr<T> widget) {
        return m_root_widget->add(widget);
    }

    Widget& root() {
        return *m_root_widget;
    }

    bool run() {
        return tunix_app_run(m_app);
    }

    void quit() {
        tunix_app_quit(m_app);
    }
};

} // namespace tunix

#endif /* TUNIX_HPP */
