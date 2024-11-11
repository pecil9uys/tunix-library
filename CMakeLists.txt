cmake_minimum_required(VERSION 3.15)
project(tunix VERSION 1.0.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Core static library
add_library(tunix STATIC
    src/tunix_platform.c
    src/tunix_renderer.c
    src/tunix_types.c
    src/tunix_widget.c
)

target_include_directories(tunix
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link system libraries
if(WIN32)
    target_link_libraries(tunix PUBLIC user32)
endif()

# C Example - Basic demo
add_executable(demo_c examples/demo_c.c)
target_link_libraries(demo_c PRIVATE tunix)

# C++ Example - Basic demo
add_executable(demo_cpp examples/demo_cpp.cpp)
target_link_libraries(demo_cpp PRIVATE tunix)

# C Example - Advanced terminal controls
add_executable(advanced_demo examples/advanced_demo.c)
target_link_libraries(advanced_demo PRIVATE tunix)

# C++ Example - Advanced features
add_executable(advanced_demo_cpp examples/advanced_demo_cpp.cpp)
target_link_libraries(advanced_demo_cpp PRIVATE tunix)

# C++ Example - Form with validation
add_executable(form_demo examples/form_demo.cpp)
target_link_libraries(form_demo PRIVATE tunix)

# C++ Example - System monitoring dashboard
add_executable(dashboard_demo examples/dashboard_demo.cpp)
target_link_libraries(dashboard_demo PRIVATE tunix)

# C Example - Spinner animations
add_executable(spinner_demo examples/spinner_demo.c)
target_link_libraries(spinner_demo PRIVATE tunix)

# Installation rules
install(TARGETS tunix
    EXPORT tunixTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(EXPORT tunixTargets
    FILE tunixTargets.cmake
    NAMESPACE tunix::
    DESTINATION lib/cmake/tunix
)
