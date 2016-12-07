#version 330

in (location = 0) vec3 vertex_position;

void main () {
	gl_Position = vec4(vertex_position, 1);
};
