#include "glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#include "shader.h"
#include "cglm/mat4.h"
#include "cglm/common.h"
#include "cglm/affine.h"
#include "cglm/cam.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport((width-480)/2, (height-480)/2, 480, 480);
}




int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //only use modern core functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    glViewport(0,0,480,480);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // use the shader program and delete the individual shaders
    GLuint shaderProgram = get_shader_programme();
    glUseProgram(shaderProgram);


    // GLfloat vertices[] =    { //               COORDINATES                  /     COLORS           //
    //     -0.5f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
    //     0.5f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
    //     0.0f,  0.5f * (float)sqrt(3) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
    //     -0.25f, 0.5f * (float)sqrt(3) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
    //     0.25f, 0.5f * (float)sqrt(3) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
    //     0.0f, -0.5f * (float)sqrt(3) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
    // };

	// // Indices for vertices order
	// GLuint indices[] = {
	// 	0, 3, 5, // Lower left triangle
	// 	3, 2, 4, // Upper triangle
	// 	5, 4, 1 // Lower right triangle
	// };

    // // Vertices coordinates
    // GLfloat vertices[] =
    // { //     COORDINATES     /        COLORS      /   TexCoord  //
    //     -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
    //     -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
    //     0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
    //     0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
    // };

    // // Indices for vertices order
    // GLuint indices[] =
    // {
    //     0, 2, 1, // Upper triangle
    //     0, 3, 2 // Lower triangle
    // };

    // Vertices coordinates
    GLfloat vertices[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
        0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
    };

    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    int imgW, imgH, colCh;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* bytes = stbi_load("../resources/R.png", &imgW, &imgH, &colCh, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glm_translated(view, (vec3){0.0f, -0.1f, -2.0f});
    glm_perspective(glm_rad(45.f), 1.f, 0.1f, 100.f, proj);


    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "proj");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");

    GLuint uniformID = glGetUniformLocation(shaderProgram, "scale");
    GLuint tex0 = glGetUniformLocation(shaderProgram, "tex0");

    float lastTime = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.1f, 0.5f, 1.f);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glUniform1f(uniformID, 1.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model[0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj[0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view[0]);

        glm_rotate(model, 0.01f, (vec3) {0,1,0});

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0, GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        printf("%d\n", (int)(1 / deltaTime));
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
