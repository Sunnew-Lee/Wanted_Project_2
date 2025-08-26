/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose:
	This file is the application of the graphics engine.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <scene.h>

void draw();
void update();
double update_time(double);
void init();
bool helper_init(GLint, GLint, std::string);
void cleanup();

GLFWwindow* ptr_window;
Scene scene;
double fps;
std::pair<int, int> window_size{ 1600,800 };

int main() {
	init();
	while (!glfwWindowShouldClose(ptr_window)) {
		update();
		draw();
	}
	cleanup();
}

void init() {
	int w = window_size.first;
	int h = window_size.second;

	if (!helper_init(w, h, "CS300_Assignment1")) {
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	scene.init(w, h);

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(ptr_window, true);
	const char* glsl_version = "#version 400";
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();
}

bool helper_init(GLint w, GLint h, std::string t)
{
	if (!glfwInit()) {
		std::cout << "GLFW init has failed - abort program!!!" << std::endl;
		return false;
	}
	// In case a GLFW function fails, an error is reported to callback function
	//glfwSetErrorCallback(GLApp::error_cb);

	// Before asking GLFW to create an OpenGL context, we specify the minimum constraints
	// in that context:

	// specify OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// specify modern OpenGL only - no compatibility with "old" OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// applications will be double-buffered ...
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	// default behavior: colorbuffer is 32-bit RGBA, depthbuffer is 24-bits
	// don't change size of window
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// size of viewport: width x height
	ptr_window = glfwCreateWindow(w, h, t.c_str(), nullptr, nullptr);
	if (!ptr_window) {
		std::cerr << "GLFW unable to create OpenGL context - abort program\n";
		glfwTerminate();
		return false;
	}

	// make the previously created OpenGL context current ...
	glfwMakeContextCurrent(ptr_window);
	// enable vsync
	glfwSwapInterval(1);

	// set callback for events associated with window size changes; keyboard;
   // mouse buttons, cursor position, and scroller
	//glfwSetFramebufferSizeCallback(GLApp::ptr_window, GLApp::fbsize_cb);
	//glfwSetKeyCallback(GLApp::ptr_window, GLApp::key_cb);
	//glfwSetMouseButtonCallback(GLApp::ptr_window, GLApp::mousebutton_cb);
	//glfwSetCursorPosCallback(GLApp::ptr_window, GLApp::mousepos_cb);
	//glfwSetScrollCallback(GLApp::ptr_window, GLApp::mousescroll_cb);

	// this is the default setting ...
	glfwSetInputMode(ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// initialize OpenGL (and extension) function loading library
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Unable to initialize GLEW - error: "
			<< glewGetErrorString(err) << " abort program" << std::endl;
		return false;
	}
	if (GLEW_VERSION_3_3) {
		std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
		std::cout << "Driver supports OpenGL 4.0\n" << std::endl;
	}
	else {
		std::cerr << "Driver doesn't support OpenGL 4.0 - abort program" << std::endl;
		return false;
	}

	// we'll use the entire window as viewport ...
	GLint width, height;
	glfwGetFramebufferSize(ptr_window, &width, &height);
	//GLApp::fbsize_cb(ptr_window, width, height);

	return true;
}

void update() {
	// time between previous and current frame
	double delta_time = update_time(1.0);
	// write window title with current fps ...
	//std::stringstream sstr{ "CS300_Assignment1" };
	//sstr << std::fixed << std::setprecision(2) << "CS300_Assignment1" << ": " << fps;
	//glfwSetWindowTitle(ptr_window, sstr.str().c_str());

	glfwPollEvents();

	

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	scene.update(delta_time);
	// Display FPS in another viewport
	//ImGui::Begin("Scene");
	//ImGui::Text("Application average %0.3f ms/frame (%.01f FPS)", (1.f / GLApp::fps) * 1000.f, GLApp::fps);
	//ImGui::End();
}

double update_time(double /*fps_calc_interval*/) {
	// get elapsed time (in seconds) between previous and current frames
	static double prev_time = glfwGetTime();
	double curr_time = glfwGetTime();
	double delta_time = curr_time - prev_time;
	//prev_time = curr_time;

	////// fps calculations
	//static double count = 0.0; // number of game loop iterations
	//static double start_time = glfwGetTime();
	//// get elapsed time since very beginning (in seconds) ...
	//double elapsed_time = curr_time - start_time;

	//++count;

	////// update fps at least every 10 seconds ...
	//fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
	//fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
	//if (elapsed_time > fps_calc_interval) {
	//	fps = count / elapsed_time;
	//	start_time = curr_time;
	//	count = 0.0;
	//}

	// done calculating fps ...
	return delta_time;
}


void draw() {
	// clear colorbuffer with RGBA value in glClearColor ...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);

	scene.draw();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// swap buffers: front <-> back
	glfwSwapBuffers(ptr_window);
}


void cleanup() {
	scene.cleanup();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
