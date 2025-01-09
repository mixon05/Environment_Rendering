cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0072 NEW) # Preferuj GLVND

project(gkom_enviroment_rendering)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Dodaj ścieżkę do nagłówków GLEW
include_directories(includes)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(/usr/include)  # W razie potrzeby dopasuj ścieżkę do GLEW

add_library(STB_IMAGE "src/stb_image.cpp")
add_library(GLAD "src/glad.c")

set(LIBS ${LIBS} STB_IMAGE GLAD glfw OpenGL::GL GLEW)

add_executable(gkom_enviroment_rendering src/main.cpp src/envmap/envmap.cpp src/control/control.cpp)

# Poprawione linkowanie
target_link_libraries(gkom_enviroment_rendering ${LIBS} ${GLFW_LIBRARIES})
