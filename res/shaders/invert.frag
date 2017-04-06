#version 430 core

// Captured render
uniform sampler2D tex;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
	
	// Sample textures
	vec4 render_tex = texture(tex, tex_coord);

	colour = vec4(1 - render_tex.r, 1 - render_tex.g, 1 - render_tex.b, 1.0);

	// Ensure alpha is 1
	colour.a = 1.0;
	
}