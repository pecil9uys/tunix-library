-- Tunix Terminal UI Framework
-- Pure C11 core + Optional C++20 wrapper

set_project("tunix")
set_version("1.0.0")
set_languages("c11", "cxx20")
set_warnings("all", "error")

-- Disable secure warnings on MSVC
if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

-- Core C library (includes both C and C++ headers)
target("tunix")
    set_kind("static")
    add_files("src/*.c")
    add_headerfiles("include/(**.h)", "include/(**.hpp)")
    add_includedirs("include", {public = true})
    
    if is_plat("windows") then
        add_syslinks("user32")
    end
target_end()

-- C Example
target("demo_c")
    set_kind("binary")
    add_files("examples/demo_c.c")
    add_deps("tunix")
target_end()

-- C++ Example
target("demo_cpp")
    set_kind("binary")
    set_languages("cxx20")
    add_files("examples/demo_cpp.cpp")
    add_deps("tunix")
target_end()

-- Advanced C Example
target("advanced_demo")
    set_kind("binary")
    add_files("examples/advanced_demo.c")
    add_deps("tunix")
target_end()

-- Advanced C++ Example
target("advanced_demo_cpp")
    set_kind("binary")
    set_languages("cxx20")
    add_files("examples/advanced_demo_cpp.cpp")
    add_deps("tunix")
target_end()

-- Form Example
target("form_demo")
    set_kind("binary")
    set_languages("cxx20")
    add_files("examples/form_demo.cpp")
    add_deps("tunix")
target_end()

-- Dashboard Example
target("dashboard_demo")
    set_kind("binary")
    set_languages("cxx20")
    add_files("examples/dashboard_demo.cpp")
    add_deps("tunix")
target_end()

-- Spinner Example
target("spinner_demo")
    set_kind("binary")
    add_files("examples/spinner_demo.c")
    add_deps("tunix")
target_end()

-- Interactive Controls Example
target("interactive_demo")
    set_kind("binary")
    add_files("examples/interactive_demo.c")
    add_deps("tunix")
target_end()
