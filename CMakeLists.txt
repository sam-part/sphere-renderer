cmake_minimum_required(VERSION 3.28)
project(sphere-renderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(sphere-renderer main.cpp
        application.cpp
        application.h
        camera.cpp
        camera.h
        shader.cpp
        shader.h
        sphere.cpp
        sphere.h
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui-SFML.cpp
        imgui/imgui-SFML.h
        imgui/imgui-SFML_export.h
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h)

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