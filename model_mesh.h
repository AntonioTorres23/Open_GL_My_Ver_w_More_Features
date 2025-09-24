#ifndef M_M
#define M_M

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sdr.h"

#include <iostream>

#include <vector>
#include <string>

#define MAX_BONE_INFLUENCE_DATA 4 // sets the max number of data from bone indexes which are primarily used for animation data

// create a vertex structure that will hold the vertex data gathered from the model
struct vData_Struct {
	// vertex_position_coordinates
	glm::vec3 vertex_position_coordinates;
	// vertex_normal_coordinates
	glm::vec3 vertex_normal_coordinates;
	// vertex_texture_coordinates
	glm::vec2 vertex_texture_coordinates;
	// vertex_tangent_coordinates
	glm::vec3 vertex_tangent_coordinates;
	// vertex_bit_tangent_coordinates
	glm::vec3 vertex_bit_tangent_coordinates;
	// vertex_bone_indexes
	int vertex_bone_indexes[MAX_BONE_INFLUENCE_DATA];
	// weight of each bone which means how much pull it has in moving the vertices that it influences
	float vertex_bone_weights[MAX_BONE_INFLUENCE_DATA];
};

// create a texture structure that will hold the texture data gathered from the model
// think of uv textures and stuff like that
struct tData_Struct {
	// texture ID for OpenGL
	unsigned int texID;
	// string to help with parsing through the texture type
	std::string tex_type;
	// path to texture 
	std::string tex_path; 
};


// create a class titled MODEL_MESH

class Model_Mesh 
{
public:
	// create a public method vector (standard library version) that stores vData structs
	// be sure to specify the data type you want to store in the vector prior to the method/variable name
	std::vector<vData_Struct> model_vData;
	// create a public method vector that stores only the indices grabbed from the model data
	std::vector<unsigned int> iData;
	// create a public method vector that stores tData structs
	std::vector<tData_Struct> model_tData;
	// create a public method vertex array object
	unsigned int Vertex_Array_Object;

	// constructor to specify arguements in order to create a Model_Mesh object as well as run certain code upon the creation of the object
	Model_Mesh(std::vector<vData_Struct> arg_model_vData, std::vector<unsigned int> arg_model_iData, std::vector<tData_Struct> arg_model_tData)
	{
		// assign whatever data is gathered from the 3 arguments in the constructor to thier respective public methods stored within the class
		model_vData = arg_model_vData;
		
		iData = arg_model_iData;

		model_tData = arg_model_tData;

		// after gathering all the required data, begin setting up our VBO, EBO, and attribute pointers

		build_mesh();
		

	}

	// similar to when we draw with raw verticies in main we are getting everything set up and then drawing our model
	void Model_Draw(SDR &sdr_obj)
	{
		// create an unsigned int variable to represent the ammount of diffuse textures
		unsigned int number_of_diffuse_textures = 1;
		// create an unsigned int variable to represent the ammount of specular textures 
		unsigned int number_of_specular_textures = 1;
		// create an unsigned int variable to represent the ammount of normal textures
		unsigned int number_of_normal_textures = 1;
		// create an unsigned int variable to represent the amount of height textures
		unsigned int number_of_height_textures = 1;
		
		// parse through the size of the model_tData vector and activate the amount of textures gathered from the size
		for (unsigned int for_loop_texture_size_integer = 0; for_loop_texture_size_integer < model_tData.size(); for_loop_texture_size_integer++)
		{
			// remember that the GL_TEXTURE has a data type of GLenum which is esentially an unsigned integer so we can loop through that as well as add to GL_TEXTURE0 to increase it to GL_TEXTURE1 and so on
			glActiveTexture(GL_TEXTURE0 + for_loop_texture_size_integer);
			// create a string that holds the current number of whatever texture type you are on, think of it as the diffTex1, or specTex3
			std::string texNum;
			// create another string called texName to store the name of whatever the texture type is called, like diffTex, or specTex, or normTex
			// this was one of the variables that we have stored within this tData structure which is also a string
			std::string texName = model_tData[for_loop_texture_size_integer].tex_type;
			// if texName is equal to diffTex convert the unsigned integer number of number_of_diffuse_textures to a string that gets sent to texNum and incremented at the same time 
			if (texName == "diffTex")
				texNum = std::to_string(number_of_diffuse_textures++);
			// if texName is equal to specTex convert the unsigned integer of number_of_specular_textures to a string that gets sent to texNum and incremented at the same time
			else if (texName == "specTex")
				texNum = std::to_string(number_of_specular_textures++);
			// if texName is equal to normTex convert the unsigned integer number of number_of_normal_textures to a string that gets sent to texNum and incremented at the same time
			else if (texName == "normTex")
				texNum = std::to_string(number_of_normal_textures++);
			// if texName is equal to heightTex convert the unsigned integer number of number_of_height_textures to a string that gets sent to texNum and incremented at the same time
			else if (texName == "heightTex")
				texNum = std::to_string(number_of_height_textures++);

			// set the uniform 1 integer function of the integer for loop variable we provided and concatenate texName and texNumber to find the location of the shader type in the shader program we just found within our if-else statments
			// remember we are setting wherever the texture is located in the shader program ID and setting the for loop integer as its new value in the shaders
			glUniform1i(glGetUniformLocation(sdr_obj.shader_program_id, (texName + texNum).c_str()), for_loop_texture_size_integer);
			// then bind the current for_loop_texture integer with GL_TEXTURE_2D
			glBindTexture(GL_TEXTURE_2D, model_tData[for_loop_texture_size_integer].texID);

		}

		// draw the data we just gathered

		// bind our VAO
		glBindVertexArray(Vertex_Array_Object);
		// draw our elements since we also use indices
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(iData.size()), GL_UNSIGNED_INT, 0);
		// disable our VAO to reset for the next object
		glBindVertexArray(0);
		// set our textures back for the next model call
		glActiveTexture(GL_TEXTURE0);
	}

private:
	// private methods that create our VBO and EBO
	unsigned int Vertex_Buffer_Object, Element_Buffer_Object;

	// private function that sets up the vertex buffer object data and the vertex array object pointers, same applies for the element buffer object
	void build_mesh()
	{
		// generate the VBO, VAO, and EBO
		glGenVertexArrays(1, &Vertex_Array_Object);
		glGenBuffers(1, &Vertex_Buffer_Object);
		glGenBuffers(1, &Element_Buffer_Object);

		// bind the VAO
		glBindVertexArray(Vertex_Array_Object);
		// bind VBO to allow vertex data to be loaded
		glBindBuffer(GL_ARRAY_BUFFER, Vertex_Buffer_Object);
		// structure's memory layout is sequential (which means it stays in order at all times) for all its data within
		// passing a pointer of the model_vData fits the formatting of a glm::vec array perfectly
		glBufferData(GL_ARRAY_BUFFER, model_vData.size() * sizeof(vData_Struct), &model_vData[0], GL_STATIC_DRAW);

		// bind EBO to allow index data to be loaded
		// make sure for the buffer to use GL_ELEMENT_ARRAY_BUFFER since it's indices rather than vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Element_Buffer_Object);
		// similar structure to the VBO data we loaded prior
		// except remember to include GL_ELEMENT_ARRAY_BUFFER as your first parameter rather than GL_ARRAY_BUFFER since we are working with indices
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iData.size() * sizeof(unsigned int), &iData[0], GL_STATIC_DRAW);

		// begin adding the attribute pointers for the different kinds of vertex data that we will be using
		// the vertex data for positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)0);
		// the vertex data for normal coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_normal_coordinates));
		// the vertex data for texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_texture_coordinates));
		// the vertex data for tangent coordinates
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_tangent_coordinates));
		// the vertex data for bit tangent coordinates
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_bit_tangent_coordinates));
		// the vertex data for bone indexes
		glEnableVertexAttribArray(5);
		// use the OpenGL function glVertexAttribIPointer because we are using integers as our data
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_bone_indexes));
		// the vertex data for bone weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vData_Struct), (void*)offsetof(vData_Struct, vertex_bone_weights));

		glBindVertexArray(0);

	}
};

#endif