cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0072 NEW) # Preferuj GLVND

project(gkom_environment_rendering)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Dodaj ścieżkę do nagłówków
include_directories(${CMAKE_SOURCE_DIR}/includes)      # Ścieżka do includes
include_directories(${GLFW_INCLUDE_DIRS})               # Ścieżka do GLFW
include_directories(${CMAKE_SOURCE_DIR}/src)            # Ścieżka do src
include_directories(/usr/include)                       # Ścieżka do systemowych nagłówków

add_library(STB_IMAGE "src/stb_image.cpp")
add_library(GLAD "src/glad.c")

set(LIBS ${LIBS} STB_IMAGE GLAD glfw OpenGL::GL GLEW)

add_executable(gkom_environment_rendering
    src/main.cpp
    src/envmap/envmap.cpp
    src/control/control.cpp
    src/nlohmann/json.hpp
    src/light/light.cpp
)

# Poprawione linkowanie
target_link_libraries(gkom_environment_rendering ${LIBS} ${GLFW_LIBRARIES})