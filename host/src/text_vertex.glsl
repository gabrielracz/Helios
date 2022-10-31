// Source code of vertex shader
#version 330

// Vertex buffer
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uv;

// Uniform (global) buffer
uniform mat4 transformation_matrix;

// Attributes forwarded to the fragment shader
out vec2 uv_interp;

void test() {
    float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

    gl_Position = vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0f);
    uv_interp = vec2(x, y);
}

void main()
{
    // Transform vertex
	vec4 vertex_pos = vec4(vertex.xyz, 1.0);
    //gl_Position = transformation_matrix * vertex_pos;
    gl_Position = vertex_pos;
    
    // Pass attributes to fragment shader
    //color_interp = vec4(color, 1.0);
    uv_interp = uv;
}
