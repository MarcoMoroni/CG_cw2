#version 440

// This shader requires direction.frag, point.frag and spot.frag

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir,
                         in vec4 tex_colour);
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour);
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);

// Directional light information
uniform directional_light dir_lights[4];
// Point lights being used in the scene
uniform point_light points[1];
// Spot lights being used in the scene
uniform spot_light spots[1];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Normal map to sample from
uniform sampler2D normal_map;
// Viewport resolution (in pixel)
//uniform vec2 window_size;
// Shader playback time (in seconds)
//uniforn float global_time
// Distortion map
// uniform sampler2D dist_map;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming tangent
layout(location = 3) in vec3 tangent;
// Incoming binormal
layout(location = 4) in vec3 binormal;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {

	// Calculate view direction
	vec3 view_dir = normalize(eye_pos - position);

	// Sample texture
	vec4 texture_colour = texture(tex, tex_coord);

	// Calculate normal from normal map
	vec3 final_normal = calc_normal(normal, tangent, binormal, normal_map, tex_coord * 2); // scale the normal a little bit

	// Calculate directional light colour
	for (int i = 0; i < 4; i++)
	{
		colour += calculate_direction(dir_lights[i],  mat, final_normal, view_dir, texture_colour);
	}

	// Sum point lights
	for (int i = 0; i < 1; ++i)
	{
		colour += calculate_point(points[i], mat, position, final_normal, view_dir, texture_colour);
	}

	// Sum spot lights
	for(int i = 0; i < 1; ++i)
	{
		colour += calculate_spot(spots[i], mat, position, final_normal, view_dir, texture_colour);
	}

	colour.a = 0.5;

	//// Water effect //// ?

	// https://www.shadertoy.com/view/MdtSz2

	float speed = 0.05;
	float amount = 0.02;
	float inv_scale = 1.0 / 3.0;

	//vec2 frag = position.xy / window_size.xy;
	//frag.x *= window_size.x / window_size.y;

	// Adjust speed
	//float time = ... * speed;

	// Create distortion vector
	//vec2 distort1 = texture(dist_map, frag * invscale + time * vec2(0.5, -0.7)).rg;
	//vec2 distort2 = texture(dist_map, frag * invscale + time * vec2(-0.3, 1.7)).gb;
	//vec2 distort = (2.0 * (distort1 + distort2) - 2.0) * amount;

	//colour += texture(..., frag + distort);

	//////////////////////
	
}