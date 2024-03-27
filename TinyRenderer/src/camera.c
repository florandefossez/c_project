#include "camera.h"


void init(Camera* camera, int width, int height) {
    camera->width = width;
    camera->height = height;
    camera->firstClick = false;
    camera->speed = 0.000000000001f;
    camera->sensitivity = 1.f;

    camera->Position[0] = 0.0f;
    camera->Position[1] = 0.0f;
    camera->Position[2] = 2.0f;

    camera->Orientation[0] = 0.0f;
    camera->Orientation[1] = 0.0f;
    camera->Orientation[2] = -1.0f;

    camera->Up[0] = 0.0f;
    camera->Up[1] = 1.0f;
    camera->Up[2] = 0.0f;

}


void export_matrix(Camera* camera, float FOVdeg, float nearPlane, float farPlane, GLuint shader, const char* uniform) {
	mat4 view;
	mat4 projection;
    mat4 cam;
    vec3 center;

    glm_vec3_add(camera->Position, camera->Orientation, center);
	glm_lookat(camera->Position, center, camera->Up, view);
    glm_perspective(glm_rad(FOVdeg), (float)camera->width / (float)camera->height, nearPlane, farPlane, projection);
    glm_mat4_mul(projection, view, cam);

	glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, cam[0]);
}



void camera_inputs(Camera* camera, GLFWwindow* window) {
    vec3 tmp;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(camera->Orientation, camera->speed, camera->Position);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_cross(camera->Orientation, camera->Up, tmp);
        glm_vec3_normalize(tmp);
        glm_vec3_mulsubs(tmp, camera->speed, camera->Position);
		// camera->Position += camera.speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_mulsubs(camera->Orientation, camera->speed, camera->Position);
		// camera->Position += camera.speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_cross(camera->Orientation, camera->Up, tmp);
        glm_vec3_normalize(tmp);
        glm_vec3_muladds(tmp, camera->speed, camera->Position);
		// camera->Position += camera.speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glm_vec3_muladds(camera->Up, camera->speed, camera->Position);
		// camera->Position += camera.speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        glm_vec3_mulsubs(camera->Up, camera->speed, camera->Position);
		// camera->Position += camera.speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera->speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		camera->speed = 0.1f;
	}


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (camera->firstClick)
		{
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
		float rotX = camera->sensitivity * (float)(mouseY - (camera->height / 2)) / camera->height;
		float rotY = camera->sensitivity * (float)(mouseX - (camera->width / 2)) / camera->width;

		// Calculates upcoming vertical change in the Orientation
        glm_vec3_cross(camera->Orientation, camera->Up, tmp);
        glm_vec3_normalize(tmp);
        // vec3 newOrientation;
        glm_vec3_rotate(camera->Orientation, glm_rad(-rotX), tmp);
		// vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		// if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		// {
		// 	Orientation = newOrientation;
		// }

		// Rotates the Orientation left and right
        glm_vec3_rotate(camera->Orientation, glm_rad(-rotY), camera->Up);
		// Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);camera->

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