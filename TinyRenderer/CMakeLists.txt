cmake_minimum_required(VERSION 3.0.0)
project(my_exc)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(my_exc src/main.c src/glad.c src/glad.h)

target_link_libraries(my_exc glfw OpenGL::GL)