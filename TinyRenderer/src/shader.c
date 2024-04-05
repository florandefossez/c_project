#include "shader.h"

#define MAX_SHADER_LEN 1000


const char *get_file_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open the file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    char* content = (char*) malloc(MAX_SHADER_LEN);
    if (content == NULL) {
        fprintf(stderr, "malloc failed.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t shader_len = fread(content, 1, MAX_SHADER_LEN, file);
    fclose(file);

    if (shader_len >= MAX_SHADER_LEN) {
        fprintf(stderr, "shader program too big\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    content[shader_len] = '\0';

    return content;
}


void compile_shader_from_file(const char* filename, GLuint shader) {
    int success;
    char infoLog[512];

    const char* vertexShaderSource = get_file_contents(filename);
    glShaderSource(shader, 1, &vertexShaderSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    free((void*) vertexShaderSource);
}


GLuint get_shader_programme() {
    int success;
    char infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader_from_file("resources/default.vert", vertexShader);
    compile_shader_from_file("resources/default.frag", fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::LINKER\n");
        printf("%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}