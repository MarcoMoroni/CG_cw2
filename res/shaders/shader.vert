#version 440

// Model transformation matrix
uniform mat4 M;
// Transformation matrix
uniform mat4 MVP;
// Normal matrix
uniform mat3 N;

// Incoming position
layout(location = 0) in vec3 position_in;
// Incoming normal
layout(location = 2) in vec3 normal_in;
// Incoming binormal
//layout(location = 3) in vec3 binormal;
// Incoming tangent
//layout(location = 4) in vec3 tangent;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;


// Outgoing position
layout(location = 0) out vec3 position;
// Outgoing transformed normal
layout(location = 1) out vec3 normal;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord;
// Outgoing tangent
//layout(location = 3) out vec3 tangent_out;
// Outgoing binormal
//layout(location = 4) out vec3 binormal_out;

void main() {

	// Calculate screen position
	gl_Position = MVP * vec4(position_in, 1.0);

	// Output other values to fragment shader
	position = vec3(M * vec4(position_in, 1.0f));

	// Transform normal
	normal = N * normal_in;

	// Pass through texture coordinate
	tex_coord = tex_coord_in;

	// Transform tangent
	//tangent_out = N * tangent;

	// Transform binormal
	//binormal_out = N * binormal;

}