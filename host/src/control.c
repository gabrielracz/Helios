#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../inc/control.h"
#include "../inc/defs.h"

static void callback_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	printf("scroll\n");
}

static void callback_mouse_move(GLFWwindow* window, double xpos, double ypos) {
	printf("mouse move\n");
}

static void callback_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	printf("click\n");
}

static void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			printf("W PRESS\n");
		} else if (action == GLFW_RELEASE) {
			printf("W RELEASE\n");
		}
	}
	else if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			printf("S PRESS\n");
		} else if (action == GLFW_RELEASE) {
			printf("S RELEASE\n");
		}
	}
	else if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			printf("A PRESS\n");
		} else if (action == GLFW_RELEASE) {
			printf("A RELEASE\n");
		}
	}
	else if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			printf("D PRESS\n");
		} else if (action == GLFW_RELEASE) {
			printf("D RELEASE\n");
		}
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

static void callback_resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static GLFWwindow* win;
static int win_width, win_height;
static char win_title[128];

int ctrl_init(const char* title, int w, int h) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//For MacOS:
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	win_width  = w;
	win_height = h;
	strcpy(win_title, title);
	win = glfwCreateWindow(win_width, win_height, win_title, NULL, NULL);
	if (win == NULL) {
		printf("ERROR: failed to create the GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);

	glViewport(0, 0, win_width, win_height);		//Perspective projection matrix
	glfwSetFramebufferSizeCallback(win, callback_resize);
	glewInit();



	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(win, callback_mouse_move);
	glfwSetScrollCallback(win, callback_scroll);
	glfwSetKeyCallback(win, callback_key);
	glfwSetMouseButtonCallback(win, callback_mouse_button);

	return 0;
}

static size_t counter = 0;
int ctrl_update() {

	glfwPollEvents();

	int rc = OK;
	if(glfwWindowShouldClose(win)) {
		printf("close\n");
		rc = CLOSE;
	}	

	/*usleep(5e5);*/
	/*printf("update %zu\n", counter++);*/

	return rc;
}


