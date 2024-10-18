cmake_minimum_required(VERSION 3.28)
project(sphere-renderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(sphere-renderer
        src/main.cpp
        src/application.cpp
        src/application.h
        src/camera.cpp
        src/camera.h
        src/shader.cpp
        src/shader.h
        src/sphere.cpp
        src/sphere.h
        src/imgui/imconfig.h
        src/imgui/imgui.cpp
        src/imgui/imgui.h
        src/imgui/imgui-SFML.cpp
        src/imgui/imgui-SFML.h
        src/imgui/imgui-SFML_export.h
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_internal.h
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imstb_rectpack.h
        src/imgui/imstb_textedit.h
        src/imgui/imstb_truetype.h)

find_package(glm CONFIG REQUIRED)
target_link_libraries(sphere-renderer PRIVATE glm::glm)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(sphere-renderer PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(GLEW REQUIRED)
target_link_libraries(sphere-renderer PRIVATE GLEW::GLEW)

#file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/shaders)

add_custom_target(copy_directory ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_BINARY_DIR}/shaders"
)

add_dependencies(sphere-renderer copy_directory)