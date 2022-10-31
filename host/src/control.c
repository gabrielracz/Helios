#include <assert.h>
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
#include "test.c"



// Shaders
/*const GLchar* vertexShaderSource = "#version 330 core\n"*/
/*"layout (location = 0) in vec3 position;\n"*/
/*"void main()\n"*/
/*"{\n"*/
/*"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"*/
/*"}\0";*/
/*const GLchar* fragmentShaderSource = "#version 330 core\n"*/
/*"out vec4 color;\n"*/
/*"void main()\n"*/
/*"{\n"*/
/*"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"*/
/*"}\n\0";*/

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


int init_quad() {
	// Set up vertex data (and buffer(s)) and attribute pointers
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // Left  
		 0.5f, -0.5f, 0.0f, // Right 
		 0.5f,  0.5f, 0.0f,  // Top   
		-0.5f,  0.5f, 0.0f
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); 
	return VAO;
}

int load_texture(const char* path) {
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

	//Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

static GLFWwindow* win;
static int win_width, win_height;
static char win_title[128];

void render_text(const char* text, float x, float y, float size) {
    glUseProgram(shaders[TEXT_SHDR]);
    glBindVertexArray(vertex_arrays[TEXT_VAO]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
	return;
	/*glActiveTexture(GL_TEXTURE0);*/
	/*glBindTexture(GL_TEXTURE_2D, textures[CHARMAP]);*/

	int len = strlen(text);
	int loc_text_len = glGetUniformLocation(shaders[TEXT_SHDR], "text_len");
	glUniform1i(loc_text_len, len);


	float width = len*0.75*(size);


	float sx = width;
	float sy = size;
	float transformation_matrix_row[] = {
		 sx , 0.0f, 0.0f,  x  ,
		0.0f,  sy , 0.0f,  y  ,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	float transformation_matrix_col[] = {
		 sx , 0.0f, 0.0f, 0.0f,
		0.0f,  sy , 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		 x  ,  y  , 0.0f, 1.0f
	};

	float identity_matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	
	int loc_transform = glGetUniformLocation(shaders[TEXT_SHDR], "transformation_matrix");
	glUniformMatrix4fv(loc_transform, 1, GL_FALSE, identity_matrix);

	// Set the text data
	assert(512 > len);
	int data[512];
	for (int i = 0; i < len; i++){
		data[i] = text[i];
	}

	int loc_content = glGetUniformLocation(shaders[TEXT_SHDR], "text_content");
	glUniform1iv(loc_content, len*sizeof(int), data);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

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
	/*vertex_arrays[TEXT_VAO] = init_quad();*/
	/*test(win, shaders[TEXT_SHDR], vertex_arrays[TEXT_VAO]);*/
	/*return 0;*/

	//  TEXTURES
	textures[CHARMAP] = load_texture("rsc/charmap.png");

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

	render_text("hello", 0.0f, 0.0f, 1.0f);

	glfwSwapBuffers(win);
	return rc;
}


