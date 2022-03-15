cmake_minimum_required(VERSION 3.0.0)
project(learn_opengl VERSION 0.1.0)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Add variables to use High Performance Graphics Card if available

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/glfw ${CMAKE_CURRENT_BINARY_DIR}/vendor/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/glm/glm ${CMAKE_CURRENT_BINARY_DIR}/vendor/glm/glm)



include_directories(
    ../vendor/glfw/include
    ../vendor/glad/include
    ../vendor/glm/
    ../vendor/imgui/
    ../vendor/
    ../vendor/utils/stb/
)


set(CMAKE_SOURCE_DIR src)

set(IMGUI_SOURCES
        ../vendor/imgui/imgui.cpp
        ../vendor/imgui/imgui_demo.cpp
        ../vendor/imgui/imgui_draw.cpp
        ../vendor/imgui/imgui_widgets.cpp
        ../vendor/imgui/imgui_impl/imgui_impl_glfw.cpp
        ../vendor/imgui/imgui_impl/imgui_impl_opengl3.cpp
        )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    ${PROJECT_NAME}
    ../vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/main.cpp 
    ${CMAKE_SOURCE_DIR}/Input.cpp
    ${CMAKE_SOURCE_DIR}/Camera.cpp
    ${IMGUI_SOURCES}
)
target_link_libraries(${PROJECT_NAME} glfw)