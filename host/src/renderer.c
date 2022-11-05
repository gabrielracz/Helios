
#include <assert.h>
#include <bits/time.h>
#include <bits/types/struct_timespec.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include "../inc/renderer.h"
#include "../inc/defs.h"
#include "../inc/message.h"
#include "../inc/serial.h"
#include "../../global.h"

int input_counter = 0;
float font_size = 0.3f;

Packet input     = {0};
Packet finalized = {0};
Packet response  = {0};

void rndr_send_msg();

static GLFWwindow* win;
static int win_width, win_height;
static char win_title[128];
static float aspect_ratio;

struct timespec clk_start, clk_end;

static inline float clampf(uint8_t b) {
	return b / 255.0f;
}

static void callback_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	font_size += yoffset * 0.025f;
	if(font_size < 0)
		font_size = 0.025;
}

static void callback_mouse_move(GLFWwindow* window, double xpos, double ypos) {
	/*printf("mouse move\n");*/
}

static void callback_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	printf("click\n");
}

static void callback_character(GLFWwindow* window, unsigned int code) {
	if(code > 255) return;
	char ch = (char) code;

	input.msg.buf[input.msg.len++] = ch;
}

static void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
		rndr_send_msg();
	}
	if(key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS && input.msg.len > 0) {
		input.msg.buf[input.msg.len-1] = 0;
		input.msg.len -= 1;
	}
	return;
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
	printf("resize\n");
	win_width = width;
	win_height = height;
	aspect_ratio = width/height;
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
	(*buffer)[filesize] = 0;
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
	char infolog[512];
	unsigned int vert_id;
	unsigned int frag_id;

	vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_id, 1, (const char**) &vert_src, NULL);
	glCompileShader(vert_id);
	glGetShaderiv(vert_id, GL_COMPILE_STATUS, &rc);
	if (!rc) {
		glGetShaderInfoLog(vert_id, 512, NULL, infolog);
		printf("ERROR: vertex compilation failed %s\n", infolog);
		return -2;
	}

	frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_id, 1, (const char**) &frag_src, NULL);
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

	/*shader = shader_id;*/

	glGetProgramiv(shader_id, GL_LINK_STATUS, &rc);
	if (!rc) {
		glGetProgramInfoLog(shader_id, 512, NULL, infolog);
		printf("ERROR: shader linking failed %s\n", infolog);
		return -3;
	}


	free(vert_src);
	free(frag_src);
	glDeleteShader(vert_id);
	glDeleteShader(frag_id);
	glUseProgram(shader_id);
	return shader_id;
}


static int init_quad() {
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); 
	return VAO;
}

static int load_texture(const char* path) {
	stbi_set_flip_vertically_on_load(1);

	//Texture
	int width, height, num_channels;
	unsigned char* data = stbi_load(path, &width, &height, &num_channels, 0);
	if (!data) {
		printf("ERROR: failed to load image %s\n", path);
		return -1;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return texture;
}


#define NUM_SHADERS 5
unsigned int shaders[NUM_SHADERS];
enum Shaders {
	TEXT_SHDR
};

#define NUM_VAO 5
unsigned int vertex_arrays[NUM_VAO];
enum VertexArrays {
	TEXT_VAO
};

#define NUM_TEXTURES 5
unsigned int textures[NUM_TEXTURES];
enum TEXTURES {
	CHARMAP
};


void rndr_text(const char* text, float x, float y, float size) {
    glUseProgram(shaders[TEXT_SHDR]);
    glBindVertexArray(vertex_arrays[TEXT_VAO]);

	/*glActiveTexture(GL_TEXTURE0);*/
	glBindTexture(GL_TEXTURE_2D, textures[CHARMAP]);

	int len = strlen(text);
	int loc_text_len = glGetUniformLocation(shaders[TEXT_SHDR], "text_len");
	glUniform1i(loc_text_len, len);


	float width = len*8/15.0f*(size);

	float sx = width;
	float sy = size;

	float transformation_matrix[] = {
		 sx , 0.0f, 0.0f, 0.0f,
		0.0f,  sy , 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x, y, 0.0f, 1.0f
	};

	float asp = 1.0f/aspect_ratio;
	float view_matrix[] = {
		 asp, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	
	int loc_transform = glGetUniformLocation(shaders[TEXT_SHDR], "transformation_matrix");
	glUniformMatrix4fv(loc_transform, 1, GL_FALSE, transformation_matrix);

	int loc_view = glGetUniformLocation(shaders[TEXT_SHDR], "view_matrix");
	glUniformMatrix4fv(loc_view, 1, GL_FALSE, view_matrix);

	// Set the text data
	assert(512 > len);
	int data[512] = {0};
	for (int i = 0; i < len; i++){
		data[i] = text[i];
	}

	int loc_content = glGetUniformLocation(shaders[TEXT_SHDR], "text_content");
	glUniform1iv(loc_content, len, data);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int rndr_init(const char* title, int w, int h) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//For MacOS:
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	win_width  = w;
	win_height = h;
	aspect_ratio = (float)w/(float)h;
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
	glfwSetCharCallback(win,            callback_character);

    glewExperimental = GL_TRUE;
    glewInit();
	GLenum err = glewInit();
	if(err) {
		printf("ERROR: glew init %d\n", err);
	}
    glViewport(0, 0, win_width, win_height);
    /*glEnable(GL_DEPTH_TEST);*/
    /*glDepthFunc(GL_LESS);*/
	
	//  SHADERS
	shaders[TEXT_SHDR] = load_shader("src/text_vertex.glsl", "src/text_fragment.glsl");
	if(shaders[TEXT_SHDR] < 0) {
		printf("ERROR: text shader not loaded\n");
		return -1;
	}

	//  VERTEX ARRAYS
	vertex_arrays[TEXT_VAO] = init_quad();

	//  TEXTURES
	textures[CHARMAP] = load_texture("rsc/fixedsys.png");

	return 0;
}

void rndr_send_msg() {
	clock_gettime(CLOCK_MONOTONIC, &clk_start);
	serial_send(&input);
	memset(input.data, 0, sizeof(input.data));
	input.msg.len = 0;
}

int rndr_update() {
	int rc = OK;
	glfwPollEvents();
	if(glfwWindowShouldClose(win)) {
		printf("close\n");
		 return CLOSE;
	}	


	glClearColor(clampf(0x00), clampf(0x00), clampf(0x00), clampf(0xff));
	glClear(GL_COLOR_BUFFER_BIT);

	rndr_text(input.msg.buf, 0.0f, 0.2f, font_size);
	if(response.msg.len > 0) {
		char response_str[256];
		for(int i = 0; i < response.msg.len; i++) {
			sprintf(response_str + i*3, "%02x ", response.msg.buf[i]);
		}
		rndr_text(response_str, 0.0f, -0.2f, font_size/1.25);
	}


	if(serial_available()) {
		memset(response.msg.buf, 0, sizeof(Packet));
		response.msg.len = 0;
		serial_receive(&response);
		clock_gettime(CLOCK_MONOTONIC, &clk_end);
		double delta = (double)(clk_end.tv_nsec - clk_start.tv_nsec)/1e9;
		delta += clk_end.tv_sec - clk_start.tv_sec;
		printf("message-delta: %fs\n", delta);

	}

	glfwSwapBuffers(win);

	return rc;
}

int rndr_quit() {
	glfwTerminate();
	return OK;
}
