#ifndef SHADER_H
#define SHADER_H

#include "glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>


const char* get_file_contents(const char* filename);
void compile_shader_from_file(const char* filename, GLuint shader);
GLuint get_shader_programme(const char* name);

#endif