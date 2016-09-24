#include "model.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>
#include <math.h>

struct IsometricCamera
{
	float scale = 2.0, near = 10.0, far = -10.0;
	bool positive_hor = true, positive_ver = true;
};

void key_callback(GLFWwindow*, int, int, int, int);
void desenharEixos();
glm::mat4 isometric(float, float, float, bool, bool);

int main(void) {
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return 1;
	}

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;
	// Make the window's context current
	glfwMakeContextCurrent(window);

	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	//Opções de OpenGL
	glClearColor(1.,1.,1.,1.);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		//glfwSetCursorPosCallback(window, mouse_callback);
	

	//Camera initalization
	IsometricCamera camera;

	tnw::octree::Tree* oct = new tnw::octree::Tree();
	//tnw::octree::Classifier f = circle;
	tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(-1,-1,1), 2);
	//oct->classify(f, bb, 6, 0);
	tnw::octree::Sphere s(glm::vec3(0,0,0), 0.5);
	oct->classify(s, bb, 6, 0);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);

		glPushMatrix();

		glm::mat4 view = isometric(camera.scale, camera.near, camera.far, camera.positive_hor, camera.positive_ver);
		glLoadMatrixf(glm::value_ptr(view));
		oct->draw(bb); 
		desenharEixos();

		glPopMatrix();

		//Interface construction	
		ImGui::Begin("Menu");

		if (ImGui::Button("+ Scale")) {
			camera.scale += 1;
		}
		if (ImGui::Button("- Scale")) {
			camera.scale -= 1;
		} 
		if (ImGui::Button("Flip X")) {
			camera.positive_hor = !camera.positive_hor;
		}
		if (ImGui::Button("Flip Y")) {
			camera.positive_ver = !camera.positive_ver;
		}
		ImGui::End();

		//ImGui::ShowTestWindow();

		// Swap front and back buffers
		ImGui::Render();
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    	glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
        
}

void desenharEixos(){
	glBegin(GL_LINES);
		glColor3f(1.000000f, 0.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(1, 0, 0);
		glColor3f(0.000000f, 1.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 1, 0);
		glColor3f(0.000000f, 0.000000f, 1.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 1);
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

glm::mat4 isometric(float scale, float near, float far, bool positive_hor, bool positive_ver)
{
	float rot_y = glm::radians(45.0); 
	float rot_x = std::asin(std::tan(glm::radians(30.0f)));
	
	if (!positive_ver){
		rot_y = -rot_y;
	}
	if (!positive_hor){
		rot_x = -rot_x;
	}
	glm::mat4 a = glm::rotate(glm::mat4(), rot_y, glm::vec3(0.0,1.0,0.0));
	glm::mat4 b = glm::rotate(glm::mat4(), rot_x, glm::vec3(1.0,0.0,0.0));

	glm::mat4 o = glm::ortho(-scale, scale, -scale, scale, near, far);

	return o*b*a;
}