#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
map<string, mesh> water_meshes;
mesh skybox;

effect main_eff;
effect water_eff;
effect sky_eff;

cubemap cube_map;

map<string, texture> textures;
map<string, texture> normal_maps;

map<string, string> textures_link; // map.first is the mesh name
                                   // map.second is the texture name

vector<directional_light> dir_lights(3);
vector<spot_light> spots(1);

int camera_switch = 1;
target_camera target_cam;   // camera_switch = 1
free_camera free_cam;       // camera_switch = 0
double cursor_x = 0.0;
double cursor_y = 0.0;

float t = 0.0f;

vec2 uv_scroll;

float zoom = 65.0f;

// For the moving box position
float previous_moving_box_position = 0.0f;

// For the invert colours and vignette effect
effect mask_eff;
effect invert_eff;
texture alpha_map;
frame_buffer temp_frame;
frame_buffer frame;
geometry screen_quad;

// Activate / deactivate invert colours
bool invertColours = false;

// Activate / deactivate orthgraphical camera
bool ortho_proj = true;

bool initialise() {

	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Capture initial mouse position
	glfwSetCursorPos(renderer::get_window(), cursor_x, cursor_y);

	return true;
}

string random_brick_wall_texture()
{
	// Use a different texture for some faces
	// By using a number higher than 10, the brick textures will be more rare
	int wall_tex_number = rand() % 10;
	switch (wall_tex_number)
	{
	case(1):
		return "wall_brick_1";
		break;
	case(2):
		return "wall_brick_2";
		break;
	case(3):
		return "wall_brick_3";
		break;
	default:
		return "wall";
		break;
	}
}

bool load_content() {



	//// Frame

	// Create frame buffer - use screen width and height
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	temp_frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

	// Create screen quad
	vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);



	//// Meshes

	// Water box
	water_meshes["box"] = mesh(geometry_builder::create_box(vec3(7.0f, 6.0f, 7.0f)));
	water_meshes["box"].get_transform().translate(vec3(8.5f, 1.0f, 7.5f));

	// Moving boxes
	meshes["moving_box"] = mesh(geometry_builder::create_box());
	meshes["moving_box2"] = mesh(geometry_builder::create_box());

	int floor_box_count = 0;

	// Floor bottom
	for (int x = 0; x <= 6; x++)
	{
		for (int z = 0; z <= 6; z++)
		{
			if ((x == 0 || x == 6) || (z == 0 || z == 6))
			{
				string mesh_name = "floor_" + to_string(floor_box_count);

				meshes[mesh_name] = mesh(geometry_builder::create_box());
				meshes[mesh_name].get_transform().translate(vec3((float)(x)-2.5f, -2.5, (float)(z)-3.5f));

				floor_box_count++;
			}
		}
	}

	// Floor top
	for (int x = 0; x <= 6; x++)
	{
		for (int z = 0; z <= 6; z++)
		{
			if ((x == 0 || x == 6) || (z == 0 || z == 6))
			{
				string mesh_name = "floor_" + to_string(floor_box_count);

				meshes[mesh_name] = mesh(geometry_builder::create_box());
				meshes[mesh_name].get_transform().translate(vec3((float)(x)-9.5f, -3.5, (float)(z)+3.5f));

				floor_box_count++;
			}
		}
	}

	int col_plane_count = 0;

	// Columns (made by planes)
	for (int y = 0; y <= 4; y++)
	{
		////// Left column //////

		string mesh_name = "plane_" + to_string(col_plane_count);

		// Right
		meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
		meshes[mesh_name].get_transform().scale *= 0.5f;
		meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
		if (y == 4)
		{
			float back = 10.0f;
			meshes[mesh_name].get_transform().translate(vec3(5.75f - back, (float)(y)+0.75f - back, -0.25f + back)); // has to stay at the bottom
		} 
		else
		{
			meshes[mesh_name].get_transform().translate(vec3(5.75f, (float)(y)+0.75f, -0.25f)); // has to stay at the front
		}

		col_plane_count++;

		mesh_name = "plane_" + to_string(col_plane_count);

		// Left
		meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
		meshes[mesh_name].get_transform().scale *= 0.5f;
		meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
		meshes[mesh_name].get_transform().translate(vec3(6.25f, (float)(y)+0.75f, 0.25f));
		
		col_plane_count++;

		////// Right column //////

		mesh_name = "plane_" + to_string(col_plane_count);

		// Right
		meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
		meshes[mesh_name].get_transform().scale *= 0.5f;
		meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
		meshes[mesh_name].get_transform().translate(vec3(-0.25f, (float)(y)+0.75f, -6.25f));


		col_plane_count++;

		mesh_name = "plane_" + to_string(col_plane_count);

		// Left
		meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
		meshes[mesh_name].get_transform().scale *= 0.5f;
		meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
		if (y == 4)
		{
			float back = 10.0f;
			meshes[mesh_name].get_transform().translate(vec3(0.25f - back, (float)(y)+0.75f - back, -5.75f + back)); // has to stay at the bottom
		}
		else
		{
			meshes[mesh_name].get_transform().translate(vec3(0.25f, (float)(y)+0.75f, -5.75f)); // has to stay at the front
		}

		col_plane_count++;
	}

	// Left floor
	int left_floor_box_count = 0;
	for (int x = 0; x <= 6; x++)
	{
		for (int z = 0; z <= 6; z++)
		{
			string mesh_name = "left_floor_" + to_string(left_floor_box_count);

			meshes[mesh_name] = mesh(geometry_builder::create_box());
			meshes[mesh_name].get_transform().translate(vec3((float)(x)+5.5f, -2.5, (float)(z)+4.5f));

			left_floor_box_count++;
		}
	}

	// Right meshes
	int right_plane_count = 0;
	for (int x = 0; x < 3; x++)
	{
		for (int z = 0; z < 3; z++)
		{
			//// Planes facing up

			string mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-9.0f, 5.5f, (float)(z)-9.0f)); // top

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-9.0f, 5.5f, (float)(z)-13.0f)); // top right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-5.0f, 5.5f, (float)(z)-9.0f)); // top left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-5.0f, 5.5f, (float)(z)-17.0f)); // bottom right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-1.0f, 5.5f, (float)(z)-13.0f)); // bottom left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x) - 1.0f, 5.5f, (float)(z)-17.0f)); // bottom

			right_plane_count++;
		}
	}
	for (int z = 0; z < 3; z++)
	{
		for (int y = 0; y < 3; y++)
		{
			//// Planes facing left

			string mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			float depth = 10.0f; // positive: forward, negative: backwards
			meshes[mesh_name].get_transform().translate(vec3(-6.5f + depth, (float)(y)+3.0f + depth, (float)(z)-9.0f - depth)); // top

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			depth = 0.0f;
			meshes[mesh_name].get_transform().translate(vec3(-6.5f + depth, (float)(y)+3.0f + depth, (float)(z)-13.0f - depth)); // top right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			depth = 5.0f;
			meshes[mesh_name].get_transform().translate(vec3(-2.5f + depth, (float)(y)+3.0f + depth, (float)(z)-9.0f - depth)); // top left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			depth = -5.0f;
			meshes[mesh_name].get_transform().translate(vec3(-2.5f + depth, (float)(y)+3.0f + depth, (float)(z)-17.0f - depth)); // bottom right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			depth = 0.0f;
			meshes[mesh_name].get_transform().translate(vec3(1.5f + depth, (float)(y)+3.0f + depth, (float)(z)-13.0f - depth)); // bottom left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(0.0f, 0.0f, -half_pi<float>()));
			depth = -5.0f;
			meshes[mesh_name].get_transform().translate(vec3(1.5f + depth, (float)(y)+3.0f + depth, (float)(z)-17.0f - depth)); // top

			right_plane_count++;
		}
	}
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			//// Planes facing right

			string mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			float depth = 0.0f; // positive: forward, negative: backwards
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-9.0f + depth, (float)(y)+3.0f + depth, -9.5f - depth)); // top

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			depth = 0.0f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-9.0f + depth, (float)(y)+3.0f + depth, -13.5f - depth)); // top right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			depth = 5.0f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-5.0f + depth, (float)(y)+3.0f + depth, -9.5f - depth)); // top left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			depth = 0.0f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-5.0f + depth, (float)(y)+3.0f + depth, -17.5f - depth)); // bottom right

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			depth = 5.0f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-1.0f + depth, (float)(y)+3.0f + depth, -13.5f - depth)); // bottom left

			right_plane_count++;

			mesh_name = "right_plane_" + to_string(right_plane_count);

			meshes[mesh_name] = mesh(geometry_builder::create_plane(2.0f));
			meshes[mesh_name].get_transform().scale *= 0.5f;
			meshes[mesh_name].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
			depth = 0.0f;
			meshes[mesh_name].get_transform().translate(vec3((float)(x)-1.0f + depth, (float)(y)+3.0f + depth, -17.5f - depth)); // bottom 

			right_plane_count++;
		}
	}

	// Left tower
	meshes["left_tower_1"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); // |
	meshes["left_tower_1"].get_transform().translate(vec3(-3.05f, -1.5f, 9.95f));
	meshes["left_tower_2"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); //     |
	meshes["left_tower_2"].get_transform().translate(vec3(-3.95f, -1.5f, 9.05f));
	meshes["left_tower_3"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); //   |
	meshes["left_tower_3"].get_transform().translate(vec3(-3.05f, -1.5f, 9.05f));
	meshes["left_roof"] = mesh(geometry_builder::create_pyramid(vec3(1.2f, 2.0f, 1.2f)));
	meshes["left_roof"].get_transform().translate(vec3(-3.5f, 1.0f, 9.5f));

	// Right tower
	meshes["right_tower_1"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); // |
	meshes["right_tower_1"].get_transform().translate(vec3(-9.05f, -1.5f, 3.95f));
	meshes["right_tower_2"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); //     |
	meshes["right_tower_2"].get_transform().translate(vec3(-9.95f, -1.5f, 3.05f));
	meshes["right_tower_3"] = mesh(geometry_builder::create_box(vec3(0.1f, 3.0f, 0.1f))); //   |
	meshes["right_tower_3"].get_transform().translate(vec3(-9.05f, -1.5f, 3.05f));
	meshes["right_roof"] = mesh(geometry_builder::create_pyramid(vec3(1.2f, 2.0f, 1.2f)));
	meshes["right_roof"].get_transform().translate(vec3(-9.5f, 1.0f, 3.5f));

	// Upper tower
	meshes["upper_tower_box"] = mesh(geometry_builder::create_box(vec3(1.0f, 2.0f, 1.0f)));
	meshes["upper_tower_box"].get_transform().translate(vec3(-9.5f, -2.0f, 9.5f));
	meshes["upper_roof"] = mesh(geometry_builder::create_pyramid(vec3(1.2f, 2.0f, 1.2f)));
	meshes["upper_roof"].get_transform().translate(vec3(-9.5f, 0.0f, 9.5f));

	// Lighthouse
	meshes["lighthouse"] = mesh(geometry_builder::create_box(vec3(0.2f, 2.0f, 1.0f)));
	meshes["lighthouse"].get_transform().translate(vec3(-0.1f, 1.5f, 0.0f));
	meshes["lighthouse_wall"] = mesh(geometry_builder::create_box(vec3(0.2f, 2.0f, 1.0f)));
	meshes["lighthouse_wall"].get_transform().translate(vec3(0.1f, 1.5f, 0.0f));

	// Create box geometry for skybox
	skybox = mesh(geometry_builder::create_box());

	// Scale box by 100
	skybox.get_transform().scale *= vec3(100);

	// Load the cubemap
	array<string, 6> filenames = { "textures/cubemap_all.png", "textures/cubemap_all.png", "textures/cubemap_all.png",
		"textures/cubemap_all.png", "textures/cubemap_all.png", "textures/cubemap_all.png" };

	// Create cube_map
	cube_map = cubemap(filenames);



	//// Set materials
	
	{
		material mat;

		mat.set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));

		water_meshes["box"].set_material(mat);
		meshes["moving_box"].set_material(mat);
		meshes["moving_box2"].set_material(mat);
		for (int i = 0; i <= right_plane_count - 1; i++)
		{
			string mesh_name = "right_plane_" + to_string(i);
			meshes[mesh_name].set_material(mat);
		}
		for (int i = 0; i <= floor_box_count - 1; i++)
		{
			string mesh_name = "floor_" + to_string(i);
			meshes[mesh_name].set_material(mat);
		}
		for (int i = 0; i <= col_plane_count - 1; i++)
		{
			string mesh_name = "plane_" + to_string(i);
			meshes[mesh_name].set_material(mat);
		}
		for (int i = 0; i <= left_floor_box_count - 1; i++)
		{
			string mesh_name = "left_floor_" + to_string(i);
			meshes[mesh_name].set_material(mat);
		}
		meshes["left_tower_1"].set_material(mat);
		meshes["left_tower_2"].set_material(mat);
		meshes["left_tower_3"].set_material(mat);
		meshes["left_roof"].set_material(mat);
		meshes["right_tower_1"].set_material(mat);
		meshes["right_tower_2"].set_material(mat);
		meshes["right_tower_3"].set_material(mat);
		meshes["right_roof"].set_material(mat);
		meshes["upper_tower_box"].set_material(mat);
		meshes["upper_roof"].set_material(mat);
		meshes["lighthouse_wall"].set_material(mat);

		mat.set_emissive(vec4(1.0f, 1.0f, 1.0f, 1.0f));

		meshes["lighthouse"].set_material(mat);
	}



	//// Textures

	// Load textures
	textures["test"] = texture("textures/checker.png");
	textures["water"] = texture("textures/water.jpg");
	textures["wall"] = texture("textures/wall.png");
	textures["wall_brick_1"] = texture("textures/wall_brick_1.png");
	textures["wall_brick_2"] = texture("textures/wall_brick_2.png");
	textures["wall_brick_3"] = texture("textures/wall_brick_3.png");
	textures["orange"] = texture("textures/orange.jpg");
	textures["roof"] = texture("textures/roof.png");
	textures["lighthouse"] = texture("textures/lighthouse.png");
	alpha_map = texture("textures/vignette.png");

	// Link textures to meshes
	textures_link["box"] = "water";
	for (int i = 0; i <= floor_box_count - 1; i++)
	{
		string mesh_name = "floor_" + to_string(i);
		textures_link[mesh_name] = random_brick_wall_texture();
	}
	for (int i = 0; i <= col_plane_count - 1; i++)
	{
		string mesh_name = "plane_" + to_string(i);
		textures_link[mesh_name] = random_brick_wall_texture();
	}
	textures_link["moving_box"] = "orange";
	textures_link["moving_box2"] = "wall";
	for (int i = 0; i <= left_floor_box_count - 1; i++)
	{
		string mesh_name = "left_floor_" + to_string(i);
		textures_link[mesh_name] = random_brick_wall_texture();
	}
	for (int i = 0; i <= right_plane_count - 1; i++)
	{
		string mesh_name = "right_plane_" + to_string(i);
		textures_link[mesh_name] = "orange";
	}
	textures_link["left_tower_1"] = "wall";
	textures_link["left_tower_2"] = "wall";
	textures_link["left_tower_3"] = "wall";
	textures_link["left_roof"] = "roof";
	textures_link["right_tower_1"] = "wall";
	textures_link["right_tower_2"] = "wall";
	textures_link["right_tower_3"] = "wall";
	textures_link["right_roof"] = "roof";
	textures_link["upper_tower_box"] = "wall";
	textures_link["upper_roof"] = "roof";
	textures_link["lighthouse"] = "lighthouse";
	textures_link["lighthouse_wall"] = "wall";

	// Normal map
	normal_maps["water"] = texture("textures/water_norm.jpg");



	//// Set lighting values

	// Directional 0 - top
	dir_lights[0].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//dir_lights[0].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	dir_lights[0].set_light_colour(vec4(1.0f, 0.9f, 0.9f, 1.0f));
	dir_lights[0].set_direction(normalize(vec3(0.0f, 1.0f, 0.0f)));

	// Directional 1 - right
	dir_lights[1].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//dir_lights[1].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	dir_lights[1].set_light_colour(vec4(0.9215f, 0.82156f, 0.821569f, 1.0f));
	dir_lights[1].set_direction(normalize(vec3(0.0f, 0.0f, -1.0f)));

	// Directional 2 - left
	dir_lights[2].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//dir_lights[2].set_light_colour(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	dir_lights[2].set_light_colour(vec4(0.88f, 0.78f, 0.78f, 1.0f));
	dir_lights[2].set_direction(normalize(vec3(1.0f, 0.0f, 0.0f)));

	// Spot 0
	spots[0].set_position(vec3(0.0f, 3.0f, 0.0f));
	spots[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spots[0].set_direction(normalize(vec3(-1.0f, 0.0f, 0.0f)));
	spots[0].set_range(2.0f);
	spots[0].set_power(0.0001f);



	// Load in shaders
	main_eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	main_eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/direction.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/normal_map.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/water_shader.vert", GL_VERTEX_SHADER);
	water_eff.add_shader("shaders/water_shader.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/direction.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/normal_map.frag", GL_FRAGMENT_SHADER);
	sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
	mask_eff.add_shader("shaders/mask.vert", GL_VERTEX_SHADER);
	mask_eff.add_shader("shaders/mask.frag", GL_FRAGMENT_SHADER);
	invert_eff.add_shader("shaders/invert.vert", GL_VERTEX_SHADER);
	invert_eff.add_shader("shaders/invert.frag", GL_FRAGMENT_SHADER);

	// Build effects
	main_eff.build();
	water_eff.build();
	sky_eff.build();
	invert_eff.build();
	mask_eff.build();

	// Set camera properties
	free_cam.set_position(vec3(25.0f, 20.0f, -20.0f));
	free_cam.rotate(-quarter_pi<float>() * 3.0f, -quarter_pi<float>());
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	free_cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
	target_cam.set_position(vec3(20.0f, 20.0f, -20.0f));
	target_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	target_cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  
	return true;

}

bool update(float delta_time) {

	// DEBUG: Print fps
	//cout << 1 / delta_time << endl;

	// Activate / deactivate invert colour
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I))
	{
		invertColours = !invertColours;
	}

	// Activate / deactivate orthographical camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O))
	{
		ortho_proj = !ortho_proj;
	}

	// Change cameras
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		camera_switch = 0; // free camera
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		camera_switch = 1; // target camera
		target_cam.set_position(vec3(20.0f, 20.0f, -20.0f));
	}

	// Change controls for different cameras
	switch (camera_switch)
	{
	case(0): // Free camera
	{
		// The ratio of pixels to rotation - remember the fov
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height =
			(quarter_pi<float>() *
			(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
			static_cast<float>(renderer::get_screen_height());

		double current_x;
		double current_y;

		// Get the current cursor position
		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

		// Calculate delta of cursor positions from last frame
		double delta_x = current_x - cursor_x;
		double delta_y = current_y - cursor_y;

		// Multiply deltas by ratios - gets actual change in orientation
		delta_x *= ratio_width;
		delta_y *= ratio_height;

		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation
		free_cam.rotate(-delta_x, delta_y);

		// Use keyboard to move the camera - WSAD
		vec3 dir;
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			if (ortho_proj) dir += vec3(0.0f, 0.5f, 0.0f);
			else dir += vec3(0.0f, 0.0f, 0.5f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			if(ortho_proj) dir += vec3(-0.5f, 0.0f, -0.5f);
			else dir += vec3(-0.5f, 0.0f, 0.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			if (ortho_proj) dir += vec3(0.5f, 0.0f, 0.5f);
			else dir += vec3(0.5f, 0.0f, 0.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			if (ortho_proj) dir += vec3(0.0f, -0.5f, 0.0f);
			else dir += vec3(0.0f, 0.0f, -0.5f);
		}

		// Zoom
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_Z)) {
			zoom += 5.0f;
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_X)) {
			zoom -= 5.0f;
		}

		// Move camera
		free_cam.move(dir);

		// Update the camera
		free_cam.update(delta_time);

		// Update cursor pos
		cursor_x = current_x;
		cursor_y = current_y;

		break;
	}

	case(1): // Target camera
	{

		// Zoom
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_Z)) {
			zoom += 5.0f;
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_X)) {
			zoom -= 5.0f;
		}

		// Update the camera
		target_cam.update(delta_time);

		break;
	}

	default:
		break;

	}

	// Move and rotate the moving box 1
	t += delta_time;
	meshes["moving_box"].get_transform().rotate(vec3(0.0f, cos(t) * half_pi<float>(), 0.0f) * delta_time);
	if (sin(t) > previous_moving_box_position)
	{
		meshes["moving_box"].get_transform().position = vec3(-10.0f, -10.0f + (-sin(t)) * 3, 10.0f); // move back
		previous_moving_box_position = sin(t);
	}
	else if (sin(t) <= previous_moving_box_position)                            
	{
		meshes["moving_box"].get_transform().position = vec3(0.0f, -sin(t) * 3, 0.0f); // move forward
		previous_moving_box_position = sin(t);
	}

	// Move and rotate the moving box 2 - left
	meshes["moving_box2"].get_transform().rotate(vec3(0.0f, half_pi<float>() * 0.3, 0.0f) * delta_time);
	meshes["moving_box2"].get_transform().position = vec3(8.0f, sin(t) * 0.1f + 4.0f, 8.0f);

	// UV scroll for water texture
	uv_scroll += vec2(delta_time * 0.04, -delta_time * 0.05);

	// Set skybox position to camera position (camera in centre of skybox)
	if (camera_switch == 0)
	{
		skybox.get_transform().position = free_cam.get_position();
	}
	else
	{
		skybox.get_transform().position = target_cam.get_position();
	}

	// Move and rotate the spot light
	spots[0].set_position(meshes["lighthouse"].get_transform().position + vec3(8.0f, 5.0f, 8.0f));
	spots[0].rotate(vec3(0.0f, half_pi<float>() * 0.3, 0.0f) * delta_time);

	return true;

}

mat4 getV()
{
	switch (camera_switch)
	{
	case(0): // Free camera
		return free_cam.get_view();
		break;
	case(1): // Target camera
		return target_cam.get_view();
		break;
	default:
		return free_cam.get_view();
		break;
	}

}

mat4 getP()
{
	if (ortho_proj)
	{
		return glm::ortho(-static_cast<float>(renderer::get_screen_width()) / zoom, static_cast<float>(renderer::get_screen_width()) / zoom, -static_cast<float>(renderer::get_screen_height()) / zoom, static_cast<float>(renderer::get_screen_height()) / zoom, 2.414f, 1000.0f);	
	}
	else
	{
		switch (camera_switch)
		{
		case(0): // Free camera
			return free_cam.get_projection();
		case(1): // Target camera
			return target_cam.get_projection();
		default:
			return free_cam.get_projection();
			break;
		}
	}
}

bool render() {

	// Set render target to frame buffer
	renderer::set_render_target(frame);

	// Clear frame
	renderer::clear();

	//// Skybox

	// Disable depth test,depth mask,face culling
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Bind skybox effect
	renderer::bind(sky_eff);

	// Calculate PV (used in all the render function)
	auto V = getV();
	auto P = getP();
	auto PV = P * V;

	// Calculate MVP for the skybox
	auto M = skybox.get_transform().get_transform_matrix();	
	auto MVP = PV * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// Bind cube maps
	renderer::bind(cube_map, 0);

	// Set cubemap uniform
	glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);

	// Render skybox
	renderer::render(skybox);

	// Enable depth test,depth mask,face culling
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	//// Main effect

	// Bind effect
	renderer::bind(main_eff);
	
	// Render meshes
	for (auto &e : meshes) {

		auto m = e.second;

		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		if (e.first == "lighthouse" || e.first == "lighthouse_wall")
		{
			M = meshes["moving_box2"].get_transform().get_transform_matrix() * M;
		}
		auto MVP = PV * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		auto N = m.get_transform().get_normal_matrix();
		if (e.first == "lighthouse" || e.first == "lighthouse_wall")
		{
			N = meshes["moving_box2"].get_transform().get_normal_matrix() * N;
		}
		glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind light
		renderer::bind(dir_lights[0], "dir_lights[0]");
		renderer::bind(dir_lights[1], "dir_lights[1]");
		renderer::bind(dir_lights[2], "dir_lights[2]");

		// Bind texture
		if (textures_link.find(e.first) != textures_link.end())
		{
			renderer::bind(textures[textures_link[e.first]], 0);
		}
		else
		{
			renderer::bind(textures["test"], 0);
		}

		// Set tex uniform
		glUniform1i(main_eff.get_uniform_location("tex"), 0);
		
		// Set eye position - Get this from active camera
		if (camera_switch == 0)
		{
			glUniform3fv(main_eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
		}
		else if (camera_switch == 1)
		{
			glUniform3fv(main_eff.get_uniform_location("eye_pos"), 1, value_ptr(target_cam.get_position()));
		}

		// Render mesh
		renderer::render(m);
		
	}

	//// Water box

	// Bind effect
	renderer::bind(water_eff);

	// Render meshes wit water effect
	for (auto &e : water_meshes) {

		auto m = e.second;

		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto MVP = PV * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(water_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(water_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		auto N = m.get_transform().get_normal_matrix();
		glUniformMatrix3fv(water_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind light
		renderer::bind(dir_lights[0], "dir_lights[0]");
		renderer::bind(dir_lights[1], "dir_lights[1]");
		renderer::bind(dir_lights[2], "dir_lights[2]");

		// Bind texture
		if (textures_link.find(e.first) != textures_link.end())
		{
			renderer::bind(textures[textures_link[e.first]], 0);
		}
		else
		{
			renderer::bind(textures["test"], 0);
		}

		// Set tex uniform
		glUniform1i(water_eff.get_uniform_location("tex"), 0);

		// Bind Normal map
		renderer::bind(normal_maps["water"], 1);

		// Set normal_map uniform
		glUniform1i(water_eff.get_uniform_location("normal_map"), 1);

		// Set eye position - Get this from active camera
		if (camera_switch == 0)
		{
			glUniform3fv(water_eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
		}
		else if (camera_switch == 1)
		{
			glUniform3fv(water_eff.get_uniform_location("eye_pos"), 1, value_ptr(target_cam.get_position()));
		}

		// Set UV_scroll uniform
		glUniform2fv(water_eff.get_uniform_location("UV_SCROLL"), 1, value_ptr(uv_scroll));

		// Render mesh
		renderer::render(m);

	}

	// MVP is now the identity matrix
	MVP = mat4(1);
	
	if (invertColours)
	{
		// Set render target back to the screen
		renderer::set_render_target(temp_frame);

		// Clear frame
		renderer::clear();

		// Bind bloom effect
		renderer::bind(invert_eff);

		// Set MVP matrix uniform     
		glUniformMatrix4fv(invert_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set the tex uniform, 0
		glUniform1i(invert_eff.get_uniform_location("tex"), 0);

		// Set inverse width Uniform
		glUniform1f(invert_eff.get_uniform_location("inverse_width"), 1.0 / renderer::get_screen_width());
	 
		// Set inverse height Uniform
		glUniform1f(invert_eff.get_uniform_location("inverse_height"), 1.0 / renderer::get_screen_height());
	}

	// Bind texture from frame buffer to TU 0
	renderer::bind(frame.get_frame(), 0);

	// Render the screen quad
	renderer::render(screen_quad);

	// Set render target back to the screen
	renderer::set_render_target();

	// Bind Tex effect
	renderer::bind(mask_eff);

	// Set MVP matrix uniform
	glUniformMatrix4fv(mask_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// Bind texture from frame buffer to TU 0
	// (use temp_frame if it has been rendered before)
	if (invertColours) renderer::bind(temp_frame.get_frame(), 0);

	// Set the tex uniform, 0
	glUniform1i(mask_eff.get_uniform_location("tex"), 0);

	// Bind alpha texture to TU, 1
	renderer::bind(alpha_map, 1);

	// Set the tex uniform, 1
	glUniform1i(mask_eff.get_uniform_location("alpha_map"), 1);

	// Render the screen quad
	renderer::render(screen_quad);
  
	return true;
	
}



void main() {

	// Create application
	app application("Graphics Coursework");

	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);

	// Run application
	application.run();

}