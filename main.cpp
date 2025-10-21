#include <glad/glad.h> // Always include glad before glfw. If not you'll get a weird syntax error spit at you
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // For vectors and other 3D math
#include <glm/gtc/matrix_transform.hpp> // so that we can transform matrices
#include <glm/gtc/type_ptr.hpp> // needed in addition for 3D math

#include "sdr.h" // Shader class
#include "movement_and_mouse.h" // mouse and keyboard functionality class
#include "stb_image.h" // allows us to load images of multiple different file types

#include "process_model.h" // only include the process_model header since the process mesh header is to support the model class

#include <iostream>

// a more in depth container for us to store data types, don't get it confused with the glm vectors
#include <vector>


//prototypes which assist in less compile time as well as clean up the global space of our C++ file
void keyboard_functionality(GLFWwindow *win);
void mouse_functionality(GLFWwindow* win, double raw_mouse_yaw, double raw_mouse_pitch);
void window_resize_callback(GLFWwindow *win, int w, int h);
unsigned int tex_load(const char *tex_path);
unsigned int skybox_load(std::vector<std::string> skybox_faces_arg);


// screen dimensions stored as global integer variables
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 1000;

// camera object with vector constructor used
Cam_AND_Mov cam_and_mov_obj(glm::vec3(0.0f, 0.0f, 3.0f));

// our boolean variable for the mouse callback when the window is created for the first time
bool inital_mouse_win = true;

// where the last yaw position that was grabbed from the callback function is stored
float last_mouse_yaw_position = SCREEN_WIDTH / 2.0f; 
// where the last pitch position that was grabbed from the callback function is stored
float last_mouse_pitch_position = SCREEN_HEIGHT / 2.0f; 

// our delta time variable
float dTime = 0.0f; 
// our last frame variable
float lFrame = 0.0f; 

// your main function within C++ 
int main()

{

	// initialize GLFW
	glfwInit();
	// Set window hint to specify what version of GLFW/OpenGL you are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Set window hint to tell GLFW to use multisampling frame buffer of 4 subsamples per pixel
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Tells what package or "profile" of OpenGL to use
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	// Creating the window of our OpenGL application using GLFW
	GLFWwindow* win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Engine", NULL, NULL);
	// if window is not created, kill GLFW and the OpenGL application
	if (win == NULL)
	{
		std::cout << "Cannot Create GLFW Window" << std::endl;
		// This function terminates/kills glfw
		glfwTerminate();
		// stop application and return error basically
		return -1;
	}

	// Makes the context of the current window specified on the calling thread. 
	// I think this ties to the callback functions so it knows that this window in paticular is the one to preform those functions on 
	glfwMakeContextCurrent(win);
	// Whenever the window is resized, this function will be called and update the size of the window with the new width and height dimensions 
	glfwSetFramebufferSizeCallback(win, window_resize_callback);
	glfwSetCursorPosCallback(win, mouse_functionality);

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// glad is what loads all of our OpenGL function parameters 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Cannot initalize glad" << std::endl;
		// End appication and return error
		return -1;
	}

	// enable depth testing in it tests what coorinates are in front of other within the z axis
	glEnable(GL_DEPTH_TEST);

	// enable Multi Sample Anti-Aliasing in OpenGL
	glEnable(GL_MULTISAMPLE);

	// enable OpenGL's built-in gamma correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	// load model after enabling GL_DEPTH_TEST
	//Model_OBJ model("Models/N64 Logo/n64_logo.obj");
	Model_OBJ model("Models/N64 Logo/n64_logo.obj");

	// set up the shader proggram that we will use for our model
	SDR model_shader("blinn-phong.vert", "blinn-phong.frag");

	// object titled shader that is apart of the SDR class, takes in two arguments which are the vertex shader file path and the fragment shader file path
	SDR shader_for_cube("lighting_test.vert", "lighting_test.frag");

	// set up the shader program that we wil use for our skybox
	SDR skybox_shader("skybox.vert", "skybox.frag");

	// our vertices to draw with OpenGL
	float vertex_data[] = {

		// front face 1st triangle
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // lower left corner of texture image
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // lower right corner of texture image
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right corner of texture image

		 // front face 2nd triangle
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right corner of texture image
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // lower left corner of texture image

		// back face 1st triangle
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right corner of texture image
		 // back face 2nd triangle
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top right corner of texture image
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top left corner of texture image
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image

		// left face 1st triangle
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right corner of texture image
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		// left face second triangle
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image

		// right face 1st triangle
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right corner of texture image
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		 // right face 2nd triangle
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image 
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image

		 // bottom face 1st triangle
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top right corner of texture image
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // lower right corner of texture image
		 // bottom face 2nd triangle
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image

		// top face 1st triangle
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // top right corner of texture image
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		 // top face 2nd triangle
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // lower right corner of texture image
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, // lower left corner of texture image
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top left corner of texture image


	};

	// all local coordinates are at position 1.0 or -1.0 which are the furthest coordiantes you can have within local space
	float verticies_for_skybox[] =
	{	
		// BACK FACE SQUARE
		// back face 1st triangle
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		// back face 2nd triangle
		1.0f, -1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f, 
		-1.0f, 1.0f, -1.0f,

		// LEFT FACE SQUARE
		// left face 1st triangle
		-1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		// left face 2nd triangle
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f, 

		// RIGHT FACE SQUARE
		// right face 1st triangle
		1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 
		// right face 2nd triangle
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 
		1.0f, -1.0f, -1.0f,

		// FRONT FACE SQUARE
		// front face 1st triangle
		-1.0f, -1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 
		// front face 2nd triangle
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f,

		// TOP FACE SQUARE
		// top face 1st triangle
		-1.0f, 1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f, 
		// top face 2nd triangle
		1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		// BOTTOM FACE SQUARE
		// bottom face 1st triangle
		-1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		// bottom face 2nd triangle
		1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
		
	};

	std::vector<glm::vec3> object_model_transormation_world_positions
	{
		glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(2.0, 0.0, 0.0),
		glm::vec3(-2.0, 0.0, 0.0)

	};


	std::vector<glm::vec3> light_pos
	{
		glm::vec3(3.0, 3.0, 3.0),
		glm::vec3(-2.0, -1.0, -1.0),
		glm::vec3(4.0, -3.0, 2)
	};

	std::vector<std::string> sky_box_textures
	{
		"retro_skyboxes_pack/Classic/vz_classic_right.png", 
		"retro_skyboxes_pack/Classic/vz_classic_left.png",
		"retro_skyboxes_pack/Classic/vz_classic_up.png",
		"retro_skyboxes_pack/Classic/vz_classic_down.png",
		"retro_skyboxes_pack/Classic/vz_classic_front.png",
		"retro_skyboxes_pack/Classic/vz_classic_back.png"
	};


	// Creating a vertex buffer object and a vertex array object 
	unsigned int VERTEX_BUFFER_OBJECT, VERTEX_ARRAY_OBJECT;

	// generate 1 vertex array object with glGenVertexArrays
	glGenVertexArrays(1, &VERTEX_ARRAY_OBJECT);
	// generate 1 vertex buffer object with glGenBuffers
	glGenBuffers(1, &VERTEX_BUFFER_OBJECT);
	// bind the vertex array object with glBindVertexArray
	glBindVertexArray(VERTEX_ARRAY_OBJECT);
	// bind the vertex buffer object to a gl array buffer using glBindBuffer
	glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_OBJECT);
	// tell OpenGL what type of data to take in, the array that the vertex data is stored, the size of that array, and how it should draw that data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	// sets the position attribute within the vertex data, like which attribute to start on, how many verticies to expect, what data type to use, whether they want the data to be in unsigned ints form, the size of the each vertex attribute, and the offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// enable that vertex attribute to point 0
	glEnableVertexAttribArray(0);
	// sets the position attribute within the vertex data, in this case, its on position attribute 1 and takes 2 vertices, and has an offset of 3
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	// enable that vertex attribute to point 1
	glEnableVertexAttribArray(1);

	// Create a vertex buffer object and a vertex array object for the sky box
	unsigned int SKYBOX_VBO, SKYBOX_VAO;
	// generate 1 vertex array object with glGenVertexArrays for the sky box
	glGenVertexArrays(1, &SKYBOX_VAO);
	// generate 1 vertex buffer object with glGenBuffers for the skybox
	glGenBuffers(1, &SKYBOX_VBO);
	// bind the vertex array object with glBindVertexArray for the skybox
	glBindVertexArray(SKYBOX_VAO);
	// bind the vertex buffer object to a gl array buffer using glBindBuffer for the skybox
	glBindBuffer(GL_ARRAY_BUFFER, SKYBOX_VBO);
	// tell OpenGL what type of data to take in, the array that the vertex data is stored, the size of that array, and how it should draw that data
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies_for_skybox), verticies_for_skybox, GL_STATIC_DRAW);
	// sets the position attribute within the vertex data, like which attribute to start on, how many vertices to expect, what data type to use, whether the data to be in unsigned ints form, the size of each vertex attribute, and the offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// enable that vertex attribute to point 0
	glEnableVertexAttribArray(0);

	//unsigned int tex_to_shader = tex_load("water.png");

	//unsigned int layer_2_tex_to_shader = tex_load("checkerboard_pattern.jpg");

	// activate the shader and use the shader program
	//shader_for_cube.activate_shader();

	//model_shader.activate_shader();


	// bug where even though we have the texture IDs activated based on the texID, we still have to activate the default GLenum texture unit and bind it to the corresponding texture
	// its an odd issue that is common with OpenGL
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex_to_shader);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, layer_2_tex_to_shader);

	//shader.uniform_int("tex", 0);

	//shader.uniform_int("layer_tex", 1);

	unsigned int skybox_texID = skybox_load(sky_box_textures);

	std::cout << skybox_texID << std::endl; 

	skybox_shader.activate_shader();

	skybox_shader.uniform_int("skybox_texture", 0);

	// Our loop where we render every frame to the window
	// If window is closed is set to true or becomes apparent, this while loop will be exited


	while (!glfwWindowShouldClose(win))
	{

		// current frame variable
		float cFrame = static_cast<float>(glfwGetTime());
		// calculate deltatime by subtracting current frame from the last frame
		dTime = cFrame - lFrame;
		// update the current frame as the last frame 
		lFrame = cFrame;
		// IO Keyboard features like button presses 
		keyboard_functionality(win);
		// rendering proccess
		// clears the color buffer and sets it to the RGBA in glClearColor
		glClearColor(0.3f, 0.3f, 0.9f, 1.0f);
		// This actually clears the buffer of the entire framebuffer as well as the depth buffer each frame in this while loop which is are renderer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// this is our view_matrix this is what allows us to see our objects in a 3D space

		glm::mat4 view_matrix = cam_and_mov_obj.Obtain_View_Matrix();


		// moving the camera from the origin position back to give more space to see the objects
		//view_matrix = glm::translate(view_matrix, glm::vec3(0.0, 0.0, -1.0));

		// using the lookAt GLFW function to move view matrix around in real time
		// 1st argument: camera/view's position in world space
		// 2nd argument: camera/view's position in terms of where it's looking AKA it's "eye"
		// 3rd argument: a 3-coordinate vector pointing up in world space (0.0, 1.0, 0.0) 
		//view_matrix = glm::lookAt(glm::vec3(glm::sin((float)glfwGetTime() * 2), 0.0, glm::cos((float)glfwGetTime() * 2)), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

		// this is our projection matrix which gives our object depth of field by changing the camera  
		glm::mat4 perspective_matrix = glm::mat4(1.0f);
		perspective_matrix = glm::perspective(glm::radians(90.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1F, 100.0F);

		// moves the object back and forth in real time while staying within the range of 0.0-1.0
		float sin_translate = glm::sin((float)glfwGetTime());
			
		// send our 4x4 view matrix to the vertex shader
		//shader_for_cube.uniform_matrix_4("view_matrix", view_matrix);
		// send our 4x4 projection matrix to the vertex shader
		//shader_for_cube.uniform_matrix_4("perspective_matrix", perspective_matrix);

	

		// bind the vertex array you created earlier
		//glBindVertexArray(VERTEX_ARRAY_OBJECT);

		
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		model_shader.activate_shader();

		//glm::vec3 color_of_light = glm::vec3(sin_translate, sin_translate, 1.0f);
		glm::vec3 color_of_light = glm::vec3(1.0f, 1.0f, 1.0f);

		//model_shader.uniform_vector_3("color_of_light", color_of_light);

		model_shader.uniform_matrix_4("view_matrix", view_matrix);

		model_shader.uniform_matrix_4("perspective_matrix", perspective_matrix);

		//model_shader.uniform_vector_3("light_pos", light_pos[0]);

		model_shader.uniform_vector_3("pos_of_camera", cam_and_mov_obj.obj_cam_pos);

		model_shader.uniform_vector_3("direction_lighting_var.light_direction", -0.2f, -1.0f, -0.3f);

		model_shader.uniform_vector_3("direction_lighting_var.ambient_color", 0.1f, 0.1f, 0.1f);
		model_shader.uniform_vector_3("direction_lighting_var.diffuse_color", 0.4f, 0.4f, 0.4f);
		model_shader.uniform_vector_3("direction_lighting_var.specular_color", 1.0f, 1.0f, 1.0f);


		model_shader.uniform_vector_3("light_pos_var[0].world_space_position", light_pos[0]);

		model_shader.uniform_vector_3("pos_light_var[0].ambient_color", 0.1f, 0.1f, 0.1f);
		model_shader.uniform_vector_3("pos_light_var[0].diffuse_color", 0.4f, 0.4f, 0.4f);
		model_shader.uniform_vector_3("pos_light_var[0].specular_color", 1.0f, 1.0f, 1.0f);

		model_shader.uniform_float("pos_light_var[0].constant", 1.0f);
		model_shader.uniform_float("pos_light_var[0].lin", 0.09f);
		model_shader.uniform_float("pos_light_var[0].quad", 0.032f);


		model_shader.uniform_vector_3("pos_light_var[1].world_space_position", light_pos[1]);


		model_shader.uniform_vector_3("pos_light_var[1].ambient_color", 0.1f, 0.1f, 0.1f);
		model_shader.uniform_vector_3("pos_light_var[1].diffuse_color", 0.4f, 0.4f, 0.4f);
		model_shader.uniform_vector_3("pos_light_var[1].specular_color", 1.0f, 1.0f, 1.0f);

		model_shader.uniform_float("pos_light_var[1].constant", 1.0f);
		model_shader.uniform_float("pos_light_var[1].lin", 0.09f);
		model_shader.uniform_float("pos_light_var[1].quad", 0.032f);

		model_shader.uniform_vector_3("pos_light_var[2].world_space_position", light_pos[2]);

		model_shader.uniform_vector_3("pos_light_var[2].ambient_color", 0.1f, 0.1f, 0.1f);
		model_shader.uniform_vector_3("pos_light_var[2].diffuse_color", 0.4f, 0.4f, 0.4f);
		model_shader.uniform_vector_3("pos_light_var[2].specular_color", 1.0f, 1.0f, 1.0f);

		model_shader.uniform_float("pos_light_var[2].constant", 1.0f);
		model_shader.uniform_float("pos_light_var[2].lin", 0.09f);
		model_shader.uniform_float("pos_light_var[2].quad", 0.032f);

		// SOURCE CODE FOR DRAWING MULTIPLE MODELS USING A FOR LOOP AND TRANSLATION MATRIX

		/*

		for (int i = 0; i < object_model_transormation_world_positions.size(); i++)
		{


			// create transformation matrix for scaling, rotating, moving, etc.
			glm::mat4 transformation_matrix = glm::mat4(1.0f); // sets matrix that has a diagonal matrix all of 1s. Good for transforming this hence the name transformation matrix

			// PROGRAMER'S NOTE: remember that you have to do all your transformations backwards due to them being multiplied by the vertices first, so first we are actually scaling the object down then rotating it over time
			// TYPICALLY THE TRANSFORMATION MATRIX (AKA MODEL MATRIX) SHOULD GO LIKE THIS, TRANSFORMATIONS, SCALING, ROTATIONS 

			// rotate on one axis over time via glfwGetTime as its radians
			transformation_matrix = glm::rotate(transformation_matrix, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));

			// scale this object down by 0.5 across all 3 coordinates
			transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.5));

			// transformation matrix is translated over time to move between values 0.0-1.0 on whatever axis we specified

			transformation_matrix = glm::translate(transformation_matrix, object_model_transormation_world_positions[i]);

			// send our 4x4 matrix which is our transformation matrix to the uniform 4x4 matrix titled trasformation_matrix in the vertex shader
			//shader.uniform_matrix_4("transformation_matrix", transformation_matrix);

			glm::mat3 transformation_matrix_for_normal_coordinates = glm::mat3(1.0f);

			transformation_matrix_for_normal_coordinates = glm::transpose(glm::inverse(transformation_matrix));

			model_shader.uniform_matrix_3("transformation_matrix_for_normal_coordinates", transformation_matrix_for_normal_coordinates);


			model_shader.uniform_matrix_4("transformation_matrix", transformation_matrix);

			// draw the vertices in the vertex array object
			//glDrawArrays(GL_TRIANGLES, 0, 36);

			model.Draw_Model(model_shader);

		}
		*/

		glm::mat4 transformation_matrix = glm::mat4(1.0f);

		transformation_matrix = glm::rotate(transformation_matrix, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));

		transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.5));

		glm::mat3 transformation_matrix_for_normal_coordinates = glm::mat3(1.0f);

		transformation_matrix_for_normal_coordinates = glm::transpose(glm::inverse(transformation_matrix));

		model_shader.uniform_matrix_3("transformation_matrix_for_normal_coordinates", transformation_matrix_for_normal_coordinates);

		model_shader.uniform_matrix_4("transformation_matrix", transformation_matrix);

		model.Draw_Model(model_shader);

		shader_for_cube.activate_shader();

		shader_for_cube.uniform_vector_3("color_of_light", color_of_light);

		glBindVertexArray(VERTEX_ARRAY_OBJECT);

		//glm::mat4 transformation_matrix = glm::mat4(1.0f);

		transformation_matrix = glm::mat4(1.0f);

		//transformation_matrix = glm::rotate(transformation_matrix, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

		//transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.5, 0.5, 0.5));

		shader_for_cube.uniform_matrix_4("view_matrix", view_matrix);

		shader_for_cube.uniform_matrix_4("perspective_matrix", perspective_matrix);

		for (unsigned int pos = 0; pos < light_pos.size(); pos++)
		{
			transformation_matrix = glm::translate(transformation_matrix, light_pos[pos]);

			shader_for_cube.uniform_matrix_4("transformation_matrix", transformation_matrix);

			glDrawArrays(GL_TRIANGLES, 0, 36);
	
		}
		
		glBindVertexArray(0);

		// enable depth function so that it passes vertices that are equal to depth buffer's content
		glDepthFunc(GL_LEQUAL);

		// activate our skybox shader
		skybox_shader.activate_shader();

		// transforming this 4x4 matrix to a 3x3 with no values in the 4th column to prevent w coordinate from making translations
		view_matrix = glm::mat4(glm::mat3(cam_and_mov_obj.Obtain_View_Matrix()));
		
		skybox_shader.uniform_matrix_4("view_matrix", view_matrix);

		skybox_shader.uniform_matrix_4("perspective_matrix", perspective_matrix);

		glBindVertexArray(SKYBOX_VAO);

		// activate skybox texture
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texID);

		// draw skybox
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// set depth func back to original state which is GL_LESS
		glDepthFunc(GL_LESS);

		// Swaps the front and back buffers of the specified window's double-buffer
		glfwSwapBuffers(win);
		// This function processes those events that have been recieved and then returns immediately. Proccessing events will cause the window and input callbacks associated with those events to be called
		glfwPollEvents();
	}

	// After the key is pressed or the window is closed we terminate GLFW and all of its resources
	glfwTerminate();
	// Exit the program it is now finished
	return 0;
}


// if window is changed by a user or other factor, this callback function is run
// I assume it takes the current dimensions of the newly resized window and applies it. 
// A callback function is a special function that ties to the window with a glfw function and if any changes or parameters within the window are met, the function will execute
void window_resize_callback(GLFWwindow *win, int w, int h)
{
	// make sure that the viewport matches the new window dimensions
	// note that width and height will be much larger than specified on retina displays
	// arguments go in this func x pos, y pos, width of window, height of window
	glViewport(0, 0, w, h);
}

// if key is pressed in window, it does whatever logic is defined in the function
void keyboard_functionality(GLFWwindow *win)
{
	// if escape key is pressed, the OpenGL window should be closed
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	// if w key is pressed, move the camera position forward
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		cam_and_mov_obj.K_MOVEMENT(W, dTime);
	// if s key is pressed, move the camera position backward
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		cam_and_mov_obj.K_MOVEMENT(S, dTime);
	// if d key is pressed, move the camera position to the right
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
		cam_and_mov_obj.K_MOVEMENT(D, dTime);
	// if a key is pressed, move the camera position to the left
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
		cam_and_mov_obj.K_MOVEMENT(A, dTime);
}

void mouse_functionality(GLFWwindow* win, double raw_mouse_yaw, double raw_mouse_pitch)
{
	// convert raw mouse yaw from callback function to float instead of double
	float flt_raw_mouse_yaw = static_cast<float>(raw_mouse_yaw);
	// conver raw mouse pitch from callback function to float instead of double
	float flt_raw_mouse_pitch = static_cast<float>(raw_mouse_pitch);

	// if this is the first time this window has been open set the last_mouse_yaw_position equal to the flt_raw_mouse_yaw and last_mouse_pitch_position equal to flt_raw_mouse_pitch to prevent that large jerk from the inital window
	if (inital_mouse_win)
	{
		last_mouse_yaw_position = flt_raw_mouse_yaw;
		last_mouse_pitch_position = flt_raw_mouse_pitch;
		// set initial_mouse_win to false to allow us to caclulate the difference between last_mouse_position and flt_raw_mouse so that the result isn't always 0
		inital_mouse_win = false;
	}

	// subtracts the difference of the yaw position last stored and the current yaw position that was called. 
	float mouse_yaw_offset = last_mouse_yaw_position - flt_raw_mouse_yaw;
	// subtracts the difference of the pitch position last stored and the current pitch position that was called.
	float mouse_pitch_offset = flt_raw_mouse_pitch - last_mouse_pitch_position;
	// set the yaw position that was just called as the last yaw position
	// this gets us set up for the next time this function is called
	last_mouse_yaw_position = flt_raw_mouse_yaw;
	// sets the pitch position that was just called as the last pitch position
	// this gets us set up for the next time this function is called 
	last_mouse_pitch_position = flt_raw_mouse_pitch;

	// sends the offsets of the yaw and pitch that where recently calculated and sends them to the camera object method function that processes them  
	cam_and_mov_obj.M_MOVEMENT(mouse_yaw_offset, mouse_pitch_offset);

	
}


// function that loads file path of texture and returns a texture object compatible with OpenGL
unsigned int tex_load(char const *tex_path)
{
	// provide an unsigned int variable that will store the texture id
	unsigned int texID;
	// use the address of that unsigned int previously and generate a texture object with the glfw built-in function
	glGenTextures(1, &texID);
	// now we provide some integer variables that will be used for the dimensions of the texture provided as well as the number of components like RGB or RGBA
	int w, h, amount_of_RGB_components;
	// a pointer variable that uses the built-in stb image function to load the image into binary data that OpenGL can use to generate a texture
	// this uses the path that we have within the argument as well as the address of the 3 prior variables we defined
	unsigned char* tex_data = stbi_load(tex_path, &w, &h, &amount_of_RGB_components, 0);
	// if statement that if tex_data exists or is true do the code within the brackets
	if (tex_data)
	{
		// create a GLenum variable that takes the amount of color components that where gathered prior from the tex_data pointer and assign it to a enmueration that matches the color components gathered
		GLenum tex_col_format;
		if (amount_of_RGB_components == 1)
			tex_col_format = GL_RED;
		if (amount_of_RGB_components == 3)
			tex_col_format = GL_RGB;
		if (amount_of_RGB_components == 4)
			tex_col_format = GL_RGBA;


		// bind the texture object to the GL_TEXTURE_2D type enumeration
		glBindTexture(GL_TEXTURE_2D, texID);
		// enter the configuration you want within the glfw function glTexImage2D
		glTexImage2D(GL_TEXTURE_2D, 0, tex_col_format, w, h, 0, tex_col_format, GL_UNSIGNED_BYTE, tex_data);
		// generate a mipmap with this glfw function
		glGenerateMipmap(GL_TEXTURE_2D);
		// add our wrapping and filtering options on the current texture object
		
		// texture wrapping options for the s coordniates
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S is our X for texture coordinates
		// texture wrapping options for the t coordinates
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T is our Y for texture coordinates
		// set filtering options minimum (for textures on objects that are further away)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // for the min texture filter, use the mipmap that was generated as well as the linear filtering method
		// set filtering options magnification (for textures on objects that are closer)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // for the mag texture filter, use only the linear filtering method

		// free the previous image data with this stb image function for the next texture object
		stbi_image_free(tex_data);
	}
	// if you cannot load texture data from path, send ERROR MESSAGE to default output and free the texture data with stbi_image_free
	else
	{
		std::cout << "ERROR::CANNOT_LOAD_TEXTURE_AT_PATH: " << tex_path << std::endl;
		stbi_image_free(tex_data);
	}

	return texID;
}

// function that loads vector of strings that contain the file paths to each face of the skybox (cubemap)
// a skybox/cubemap only uses the positional vertex data and does not contain any texture coordinates
unsigned int skybox_load(std::vector<std::string> skybox_faces_arg)
{
	
	
	
	// LOGIC IS SIMILAR TO THE FUNCTION ABOVE THAT LOADS ONE TEXTURE HOWEVER WE ARE GOING TO LOOP THROUGH EACH TEXTURE STORED WITHIN THE SKYBOX VECTOR ARGUMENT PROVIDED
	
	// provide an unsigned int variable that will store the texture id
	unsigned int texID;
	// use the address of that unsigned int previously and generate a texture object with the glfw built-in function
	glGenTextures(1, &texID);
	// bind current texture ID to a GL_TEXTURE_CUBE_MAP texture type
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	// now we provide some integer variables that will be used for the dimensions of the texture provided as well as the number of components like RGB or RGBA
	int w, h, amount_of_RGB_components;
	// create a for loop that goes through each path of the textures that are stored in skybox_faces_arg, and load the texture/add texture parameters
	for (unsigned int face = 0; face < skybox_faces_arg.size(); face++)
	{
		// a pointer variable that uses the built-in stbi_load function to load the image into binary data that OpenGL can use to generate a texture
		// in this function we index through the skybox_face_arg string vector using the for loop face argument to go through each texture path stored in the C++ string vector
		// also note how we have to convert our string to a c string since that is what the stbi_load function expects
		unsigned char *tex_data = stbi_load(skybox_faces_arg[face].c_str(), &w, &h, &amount_of_RGB_components, 0);

		// if statment that if tex_data exists or is true do the code within the brackets
		if (tex_data)
		{
			
			// create a GLenum variable that takes the amount of color components that where gathered prior from the tex_data pointer and assign it to a enmueration that matches the color components gathered
			GLenum tex_col_format;
			if (amount_of_RGB_components == 1)
				tex_col_format = GL_RED;
			if (amount_of_RGB_components == 3)
				tex_col_format = GL_RGB;
			if (amount_of_RGB_components == 4)
				tex_col_format = GL_RGBA;
			
			
			// since GL_TEXTURE_CUBE_MAP int values are linearly incremented, we can loop over them using our face argument
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, tex_col_format, w, h, 0, tex_col_format, GL_UNSIGNED_BYTE, tex_data);
			stbi_image_free(tex_data);
		}
		else
		{
			std::cout << "Cannot Load Skybox Texture: " << skybox_faces_arg[face] << std::endl;
			stbi_image_free(tex_data);
		}
	}
	// configure texture parameters
	// set filtering options for minimum (for textures on objects that are far away)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // we are only using the linear filtering method, no mipmap
	// set filtering options for magnification (for textures on objects that are close)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // we are only using the linear filtering method, no mipmap
	// texture wrapping options for the s coordinates; we use clamp to edge to ensure that the texture covers the entire skybox face
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // S is our X coordinate for textures
	// texture wrapping options for the t coordinates
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // T is our Y coordinates for textures
	// texture wrapping options for the r coordinates
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // R is our Z coordinates for textures

	// return texID
	return texID;
	
}