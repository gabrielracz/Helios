#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include "../inc/control.h"
#include "../inc/defs.h"


static inline float clampf(uint8_t b) {
	return b / 255.0f;
}

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

static size_t read_file(const char* path, char** buffer) {
	FILE* f = fopen(path, "r");
	if(f == NULL) {
		printf("ERROR: fopen file %s\n", path);
		return 0;
	}
	size_t filesize;
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	*buffer = malloc(filesize + 1);
	size_t br = fread(*buffer, 1, filesize, f);
	if(br < filesize) {
		printf("ERROR: fread file %s\n", path);
		return 0;
	}
	return br;
}

static int load_shader(const char* vert_path, const char* frag_path) {

	char* vert_src;
	char* frag_src;
	if(!read_file(vert_path, &vert_src))
		return -1;
	if(!read_file(frag_path, &frag_src))
		return -1;

	int rc;
	char infolog[1024];
	unsigned int vert_id;
	unsigned int frag_id;

	vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_id, 1, (const char**)&vert_src, NULL);
	glCompileShader(vert_id);
	glGetShaderiv(vert_id, GL_COMPILE_STATUS, &rc);
	if (!rc) {
		glGetShaderInfoLog(vert_id, 512, NULL, infolog);
		printf("ERROR: vertex compilation failed %s\n", infolog);
		return -2;
	}

	frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_id, 1, (const char**)&frag_src, NULL);
	glCompileShader(frag_id);
	glGetShaderiv(frag_id, GL_COMPILE_STATUS, &rc);
	if (!rc) {
		glGetShaderInfoLog(frag_id, 512, NULL, infolog);
		printf("ERROR: fragment compilation failed %s\n", infolog);
		return -2;
	}

	unsigned int shader_id;
	shader_id = glCreateProgram();
	glAttachShader(shader_id, vert_id);
	glAttachShader(shader_id, frag_id);
	glLinkProgram(shader_id);

	glGetProgramiv(shader_id, GL_LINK_STATUS, &rc);
	if (!rc) {
		glGetProgramInfoLog(shader_id, 512, NULL, infolog);
		printf("ERROR: shader linking failed %s\n", infolog);
		return -3;
	}

	glDeleteShader(vert_id);
	glDeleteShader(frag_id);
	return 0;
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

	glfwSetFramebufferSizeCallback(win, callback_resize);
	glfwSetCursorPosCallback(win,		callback_mouse_move);
	glfwSetScrollCallback(win,			callback_scroll);
	glfwSetKeyCallback(win,				callback_key);
	glfwSetMouseButtonCallback(win,		callback_mouse_button);

	glewInit();
	unsigned int txtshader;
	txtshader = load_shader("src/text_vertex.glsl", "src/text_fragment.glsl");
	if(txtshader < 0) {
		printf("ERROR: text shader not loaded\n");
		return -1;
	}

	return 0;
}

int ctrl_update() {
	int rc = OK;
	glfwPollEvents();
	if(glfwWindowShouldClose(win)) {
		printf("close\n");
		rc = CLOSE;
	}	

	glClearColor(clampf(0x00), clampf(0x00), clampf(0x00), clampf(0xff));
	glClear(GL_COLOR_BUFFER_BIT);


	glfwSwapBuffers(win);
	return rc;
}


