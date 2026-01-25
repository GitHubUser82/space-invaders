cmake_minimum_required(VERSION 3.31)
project(SpaceInvaders LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This generates the compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GLFW
find_package(glfw3 CONFIG REQUIRED)

# GLAD (source-based)
add_library(glad src/gl.c)
target_include_directories(glad PUBLIC include)

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glad
)

# Platform-specific OpenGL
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
else ()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif ()
