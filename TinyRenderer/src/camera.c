#include "camera.h"


void init(Camera* camera, int width, int height) {
    camera->width = width;
    camera->height = height;
    camera->firstClick = true;
    camera->speed = 1.f;
    camera->sensitivity = 100000.f;

    camera->position[0] = -2.0f;
    camera->position[1] = 0.0f;
    camera->position[2] = 0.0f;

	camera->phi = 0.0f;
	camera->theta = GLM_PI_2f; // colatitude

}


void export_matrix(Camera* camera, float FOVdeg, float nearPlane, float farPlane, GLuint shader, const char* uniform) {
	CGLM_ALIGN_MAT mat4 view;
	CGLM_ALIGN_MAT mat4 projection;
    CGLM_ALIGN_MAT mat4 cam;

	// Right vector
	view[0][0] = -sinf(camera->phi);
	view[1][0] = 0.f;
	view[2][0] = cosf(camera->phi);
	view[3][0] = camera->position[0]*sinf(camera->phi) - camera->position[2]*cosf(camera->phi);

	// Up vector
	view[0][1] = -sinf(GLM_PI_2f - camera->theta)*cosf(camera->phi);
	view[1][1] = cosf(GLM_PI_2f - camera->theta);
	view[2][1] = -sinf(GLM_PI_2f - camera->theta)*sinf(camera->phi);
	view[3][1] = camera->position[0]*sinf(GLM_PI_2f - camera->theta)*cosf(camera->phi) - camera->position[1]*cosf(GLM_PI_2f - camera->theta) + camera->position[2]*sinf(GLM_PI_2f - camera->theta)*sinf(camera->phi);

	// Forward vector
	view[0][2] = -sinf(camera->theta)*cosf(camera->phi);
	view[1][2] = -cosf(camera->theta);
	view[2][2] = -sinf(camera->theta)*sinf(camera->phi);
	view[3][2] = camera->position[0]*sinf(camera->theta)*cosf(camera->phi) + camera->position[1]*cosf(camera->theta) + camera->position[2]*sinf(camera->theta)*sinf(camera->phi);

	view[0][3] = 0.f;
	view[1][3] = 0.f;
	view[2][3] = 0.f;
	view[3][3] = 1.f;
	
	glm_perspective(glm_rad(FOVdeg), (float)camera->width / (float)camera->height, nearPlane, farPlane, projection);
    glm_mat4_mul(projection, view, cam);

	glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, cam[0]);
}



void camera_inputs(Camera* camera, GLFWwindow* window, float deltaTime) {

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->position[0] += cosf(camera->phi)*camera->speed*deltaTime;
		camera->position[2] += sinf(camera->phi)*camera->speed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->position[0] += sinf(camera->phi)*camera->speed*deltaTime;
		camera->position[2] -= cosf(camera->phi)*camera->speed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->position[0] -= cosf(camera->phi)*camera->speed*deltaTime;
		camera->position[2] -= sinf(camera->phi)*camera->speed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->position[0] -= sinf(camera->phi)*camera->speed*deltaTime;
		camera->position[2] += cosf(camera->phi)*camera->speed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->position[1] += camera->speed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		camera->position[1] -= camera->speed*deltaTime;
	}



	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (camera->firstClick) {
			glfwSetCursorPos(window, (camera->width / 2), (camera->height / 2));
			camera->firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotY = camera->sensitivity * deltaTime * (float)(mouseY - (camera->height / 2)) / camera->height;
		float rotX = camera->sensitivity * deltaTime * (float)(mouseX - (camera->width / 2)) / camera->width;

		
		float angle_to_add = glm_rad(rotY);
		camera->theta += angle_to_add;
		while (camera->theta < 0) {
			camera->theta += 2*GLM_PIf;
		}
		while (camera->theta > 2*GLM_PIf) {
			camera->theta -= 2*GLM_PIf;
		}

		angle_to_add = glm_rad(rotX);
		camera->phi += angle_to_add;
		while (camera->phi < 0) {
			camera->phi += 2*GLM_PIf;
		}
		while (camera->phi > 2*GLM_PIf) {
			camera->phi -= 2*GLM_PIf;
		}
		

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (camera->width / 2), (camera->height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		camera->firstClick = true;
	}
}