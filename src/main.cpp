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

vector<directional_light> dir_lights(4);
vector<point_light> points(1);
vector<spot_light> spots(1);

int camera_switch = 1;
target_camera target_cam;   // camera_switch = 1
free_camera free_cam;       // camera_switch = 0
double cursor_x = 0.0;
double cursor_y = 0.0;

float t = 0.0f;

vec2 uv_scroll;

float zoom = 70.0f;

bool initialise() {

	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Capture initial mouse position
	glfwSetCursorPos(renderer::get_window(), cursor_x, cursor_y);

	return true;
}

bool load_content() {



	//// Meshes

	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane(2.0f));
	meshes["plane"].get_transform().scale *= 0.5f;
	meshes["plane"].get_transform().translate(vec3(0.0f, -10.0f, 0.0f));

	// Water box
	water_meshes["box"] = mesh(geometry_builder::create_box(vec3(7.0f, 7.0f, 7.0f)));
	water_meshes["box"].get_transform().translate(vec3(8.0f, 0.0f, 8.0f));

	// Moving box
	meshes["moving_box"] = mesh(geometry_builder::create_box());

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

				std::cout << mesh_name << std::endl;
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

	// Hourglass
	meshes["hourglass"] = mesh(geometry_builder::create_cylinder(1.0f, 40.0f, vec3(7.0f, 0.1f, 7.0f)));

	// Torus 1
	meshes["torus1"] = mesh(geometry_builder::create_torus(60, 20, 0.2f, 4.0f));

	// Torus 2
	meshes["torus2"] = mesh(geometry_builder::create_torus(60, 20, 0.2f, 5.0f));

	// Torus 3
	meshes["torus3"] = mesh(geometry_builder::create_torus(60, 20, 0.2f, 6.0f));
	meshes["torus3"].get_transform().position += vec3(-10.0f, 3.0f, 10.0f);

	// Create box geometry for skybox
	skybox = mesh(geometry_builder::create_box());

	// Scale box by 100
	skybox.get_transform().scale *= vec3(100);

	// Load the cubemap
	array<string, 6> filenames = { "textures/FullMoonFront2048.png", "textures/FullMoonBack2048.png", "textures/FullMoonUp2048.png",
		"textures/FullMoonDown2048.png", "textures/FullMoonRight2048.png", "textures/FullMoonLeft2048.png" };

	// Create cube_map
	cube_map = cubemap(filenames);



	//// Set materials
	{
		material mat;

		mat.set_specular(vec4(0.2f, 0.2f, 0.2f, 1.0f));
		mat.set_shininess(500.0f);
		meshes["plane"].set_material(mat);
	}
	{
		material mat;

		mat.set_specular(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		mat.set_shininess(500.0f);
		water_meshes["box"].set_material(mat);
	}
	{
		material mat;

		mat.set_specular(vec4(0.8f, 0.8f, 0.8f, 1.0f));
		mat.set_shininess(25.0f);
		meshes["torus1"].set_material(mat);
		meshes["torus2"].set_material(mat);
		meshes["torus3"].set_material(mat);
		meshes["hourglass"].set_material(mat);
	}



	//// Textures

	// Load textures
	textures["test"] = texture("textures/white.png");
	textures["floor"] = texture("textures/floor.jpg");
	textures["gold"] = texture("textures/gold.jpg");
	textures["water"] = texture("textures/water.jpg");
	textures["wall"] = texture("textures/wall.png");

	// Link textures to meshes
	textures_link["torus1"] = "gold";
	textures_link["torus2"] = "gold";
	textures_link["torus3"] = "gold";
	//textures_link["plane"] = "floor";
	textures_link["hourglass"] = "gold";
	textures_link["box"] = "water";
	
	std::cout << to_string(floor_box_count) << std::endl;
	//textures_link["floor_0"] = "wall"; // test
	/*for (int i = 0; i <= floor_box_count - 1; i++)
	{
		string mesh_name = "floor_" + to_string(i);
		textures_link[mesh_name] = "wall";
	}*/
	

	// Normal map
	normal_maps["gold"] = texture("textures/gold_norm.jpg");
	normal_maps["floor"] = texture("textures/floor_norm.jpg");
	normal_maps["test"] = texture("textures/white_norm.jpg");
	normal_maps["water"] = texture("textures/water_norm.jpg");



	//// Set lighting values

	// Directional 0 - top
	dir_lights[0].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dir_lights[0].set_light_colour(vec4(0.2f, 0.0f, 0.0f, 1.0f));
	dir_lights[0].set_direction(normalize(vec3(0.0f, 1.0f, 0.0f)));

	// Directional 1 - right
	dir_lights[1].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dir_lights[1].set_light_colour(vec4(0.0f, 0.2f, 0.0f, 1.0f));
	dir_lights[1].set_direction(normalize(vec3(0.0f, 0.0f, -1.0f)));

	// Directional 2 - left
	dir_lights[2].set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dir_lights[2].set_light_colour(vec4(0.0f, 0.0f, 0.2f, 1.0f));
	dir_lights[2].set_direction(normalize(vec3(1.0f, 0.0f, 0.0f)));

	// Directional 4 - ambient
	//dir_lights[2].set_ambient_intensity(vec4(0.0f, 0.3f, 0.0f, 1.0f)); // use this
	dir_lights[2].set_light_colour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dir_lights[2].set_direction(normalize(vec3(-1.0f, -1.0f, 1.0f)));

	// Point 0
	points[0].set_position(vec3(10.0f, 9.0f, 0.0f));
	points[0].set_light_colour(vec4(0.9f, 0.3f, 0.12f, 1.0f));
	points[0].set_range(0.0f); // Deactivated for testing

	// Spot 0
	spots[0].set_position(vec3(-2.0f, 3.0f, -10.0f));
	spots[0].set_light_colour(vec4(0.64f, 1.0f, 0.25f, 1.0f));
	spots[0].set_direction(normalize(vec3(-1.0f, 0.0f, 0.0f)));
	spots[0].set_range(0.0f); // Deactivated for testing
	spots[0].set_power(0.5f);



	// Load in shaders
	main_eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	main_eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/direction.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/point.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	main_eff.add_shader("shaders/normal_map.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/water_shader.vert", GL_VERTEX_SHADER);
	water_eff.add_shader("shaders/water_shader.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/direction.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/point.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	water_eff.add_shader("shaders/normal_map.frag", GL_FRAGMENT_SHADER);
	sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	// Build effects
	main_eff.build();
	water_eff.build();
	sky_eff.build();

	// Set camera properties
	free_cam.set_position(vec3(20.0f, 20.0f, -20.0f));
	free_cam.set_target(vec3(-1.0f, 1.0f, -1.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	free_cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
	target_cam.set_position(vec3(20.0f, 20.0f, -20.0f));
	target_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	target_cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  
	return true;

}

bool update(float delta_time) {

	// Set some camera positions
	if (glfwGetKey(renderer::get_window(), '1')) {
		free_cam.set_position(vec3(-10.0f, 10.0f, 10.0f));
	}
	if (glfwGetKey(renderer::get_window(), '2')) {
		free_cam.set_position(vec3(10.0f, 20.0f, 20.0f));
	}

	// Change cameras
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		camera_switch = 0;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		camera_switch = 1;
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
			dir += vec3(0.0f, 0.0f, 0.5f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			dir += vec3(-0.5f, 0.0f, 0.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			dir += vec3(0.5f, 0.0f, 0.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			dir += vec3(0.0f, 0.0f, -0.5f);
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

		// Use keyboard to change camera target
		// down - (50, 10, 50)
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
			target_cam.set_target(water_meshes["box"].get_transform().position);
		}
		// up - (-50, 10, 50)
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
			target_cam.set_target(meshes["hourglass"].get_transform().position);
		}

		// Update the camera
		target_cam.update(delta_time);

		break;
	}

	default:
		break;

	}

	// Rotate the torus
	meshes["hourglass"].get_transform().rotate(vec3(half_pi<float>() / 4, 0.0f, 0.0f) * delta_time);
	meshes["torus1"].get_transform().rotate(vec3(half_pi<float>() / 4, 0.0f, 0.0f) * delta_time);
	meshes["torus2"].get_transform().rotate(vec3(0.0f, 0.0f, half_pi<float>() / 3.5) * delta_time);
	meshes["torus3"].get_transform().rotate(vec3(half_pi<float>() / 3, 0.0f, 0.0f) * delta_time);

	// Move and rotate the moving box
	t += delta_time;
	meshes["moving_box"].get_transform().rotate(vec3(0.0f, sin(t) * 6, 0.0f) * delta_time);
	meshes["moving_box"].get_transform().position = vec3(0.0f, sin(t) * 3, 0.0f);
	if (sin(t) > 0.5f)
	{
		meshes["moving_box"].get_transform().translate(vec3(10.0f, 10.0f ,-10.0f));
	}
	else if (sin(t) <= 0.5f)
	{
		meshes["moving_box"].get_transform().translate(vec3(-10.0f, -10.0f, 10.0f));
	}

	uv_scroll += vec2(delta_time * 0.05, -delta_time * 0.05);

	// Set skybox position to camera position (camera in centre of skybox)
	skybox.get_transform().position = free_cam.get_position();

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

	switch (camera_switch)
	{
	case(0): // Free camera
		return free_cam.get_projection();
		break;
	case(1): // Target camera
		return target_cam.get_projection();
		break;
	default:
		return free_cam.get_projection();
		break;
	}

}

bool render() {

	//// Skybox

	// Disable depth test,depth mask,face culling
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Bind skybox effect
	renderer::bind(sky_eff);

	// Calculate MVP for the skybox
	auto M = skybox.get_transform().get_transform_matrix();
	auto V = getV();
	auto P = getP();
	auto MVP = P * V * M;

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
		auto V = getV();
		//auto P = getP();
		auto M = m.get_transform().get_transform_matrix();
		// Hierarchy
		if (e.first == "torus2")
		{
			M = meshes["torus3"].get_transform().get_transform_matrix() * M;
		}
		else if (e.first == "torus1" || e.first == "hourglass")
		{
			M = meshes["torus3"].get_transform().get_transform_matrix() * meshes["torus2"].get_transform().get_transform_matrix() * M;
		} 
		
		// ----------------------------- Othographic camera test -----------------------------
		auto P = glm::ortho(-static_cast<float>(renderer::get_screen_width()) / zoom, static_cast<float>(renderer::get_screen_width()) / zoom, -static_cast<float>(renderer::get_screen_height()) / zoom, static_cast<float>(renderer::get_screen_height()) / zoom, 2.414f, 1000.0f);
		// -----------------------------------------------------------------------------------
		auto MVP = P * V * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		// ******* NOT WORKING PROPERLY *******
		auto N = m.get_transform().get_normal_matrix();
		if (e.first == "torus1" || e.first == "torus2" || e.first == "torus3")
		{
			N = mat3(rotate(mat4(1.0), 90.0f, vec3(-1, 0, 0)) * mat4(N));
		}
		if (e.first == "torus2")
		{
			N = meshes["torus3"].get_transform().get_normal_matrix() * N;
		}
		else if (e.first == "torus1" || e.first == "hourglass")
		{
			N = meshes["torus3"].get_transform().get_normal_matrix() * meshes["torus2"].get_transform().get_normal_matrix() * N;
		}
		glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind point lights
		renderer::bind(points, "points");

		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind light
		renderer::bind(dir_lights[0], "dir_lights[0]");
		renderer::bind(dir_lights[1], "dir_lights[1]");
		renderer::bind(dir_lights[2], "dir_lights[2]");
		renderer::bind(dir_lights[3], "dir_lights[3]");

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

		// Bind Normal map
		if (textures_link.find(e.first) != textures_link.end())
		{
			renderer::bind(normal_maps[textures_link[e.first]], 1);
		}
		else
		{
			renderer::bind(normal_maps["test"], 1);
		}

		// Set normal_map uniform
		glUniform1i(main_eff.get_uniform_location("normal_map"), 1);
		

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
		auto V = getV();
		//auto P = getP();
		auto M = m.get_transform().get_transform_matrix();

		// ----------------------------- Othographic camera test -----------------------------
		auto P = glm::ortho(-static_cast<float>(renderer::get_screen_width()) / zoom, static_cast<float>(renderer::get_screen_width()) / zoom, -static_cast<float>(renderer::get_screen_height()) / zoom, static_cast<float>(renderer::get_screen_height()) / zoom, 2.414f, 1000.0f);
		// -----------------------------------------------------------------------------------
		auto MVP = P * V * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(water_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(water_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		auto N = m.get_transform().get_normal_matrix();
		glUniformMatrix3fv(water_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind point lights
		renderer::bind(points, "points");

		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind light
		renderer::bind(dir_lights[0], "dir_lights[0]");
		renderer::bind(dir_lights[1], "dir_lights[1]");
		renderer::bind(dir_lights[2], "dir_lights[2]");
		renderer::bind(dir_lights[3], "dir_lights[3]");

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
		if (textures_link.find(e.first) != textures_link.end())
		{
			renderer::bind(normal_maps[textures_link[e.first]], 1);
		}
		else
		{
			renderer::bind(normal_maps["test"], 1);
		}

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