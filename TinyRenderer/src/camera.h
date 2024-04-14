#ifndef CAMERA_H
#define CAMERA_H

#include "glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#include "shader.h"
#include "cglm/common.h"
#include "cglm/affine.h"
#include "cglm/cam.h"
#include "cglm/mat4.h"
#include "cglm/vec2.h"


typedef struct {
	vec3 position;
	// vec3 direction;
	// vec3 up;

	float theta;
	float phi;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed;
	float sensitivity;
} Camera;


void init(Camera* camera, int width, int height);
void export_matrix(Camera* camera, float FOVdeg, float nearPlane, float farPlane, GLuint shader, const char* uniform);
void camera_inputs(Camera* camera, GLFWwindow* window, float deltaTime);

#endif