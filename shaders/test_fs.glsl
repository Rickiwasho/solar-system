#version 130

in vec3 normal;
in vec2 st;

out vec4 frag_colour;

void main() {
	//frag_colour = vec4 (normal, 1.0);
    frag_colour = vec4 (1.0, 1.0, 1.0, 1.0);
}
