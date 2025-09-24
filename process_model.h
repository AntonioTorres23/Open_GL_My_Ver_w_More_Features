#define P_M

#include <glad/glad.h>

#include <glm/glm.hpp> // our library

#include <glm/gtc/matrix_transform.hpp> // allows us to transform matrixes with this like scaling, translating, etc

#include "stb_image.h" // the file that loads and processes textures of multiple file types into a format that OpenGL can read

// open asset importer library that allows us to proccess a scene that takes in multiple 3D model file types: for example .obj, .sae, etc

#include <assimp/Importer.hpp>

#include <assimp/scene.h> // allows us to load scenes from the model file types

#include <assimp/postprocess.h>

#include "model_mesh.h"

#include "sdr.h"

#include <string>

#include <fstream>

#include <sstream>

#include <iostream>

#include <map>

#include <vector>

// prototype for processing textures from the directory that the model files are in
unsigned int Textures_In_Model_Directory(const char* texture_file_path, const std::string& model_files_directory, bool gamma = false);

// create a model class

class Model_OBJ
{
public:

	// create a vector method of the tData type to store the vertex data gathered from the model file
	std::vector<tData_Struct> tex_already_loaded; // a vector that stores all of the texture files that are already loaded
	// create a vector method of the Model_Mesh class type to store the meshes that we have processed
	std::vector<Model_Mesh> model_meshes;
	// create a string method that stores the directory name where the model data is stored
	std::string model_files_directory;
	// create a bool method that stores the gama correction option for textures
	bool gammaCor; 

	// the constructor of our class which takes in a path the model data filetype as well as enabling the method gammaCorrection prior to the code runnning in the bracket of our constructor, we are setting the results to false in the constructor arguments
	Model_OBJ(std::string const& path_to_3D_model_filetype, bool gam = false) : gammaCor(gam)
	{
		// object loads the model data once the Model_OBJ object is created
		Model_Load(path_to_3D_model_filetype);
	}

	// public function that draws the actual 3D model, and the meshes that where loaded using assimp; takes in a shader_obj as its argument to apply the data to the shader program
	
	void Draw_Model(SDR& sdr_obj)
	{
		// for loop that goes through every vector index that has a Model_Mesh object and draws it then moves on to the next one until there are no more 
		for (unsigned int mesh = 0; mesh < model_meshes.size(); mesh++)
			model_meshes[mesh].Model_Draw(sdr_obj);
	}

private:

	// private function that loads the 3D model with a library called assimp
	void Model_Load(std::string const& path_to_3D_model_filetype)
	{
		// create a varible/object from assimp that imports 3D models
		Assimp::Importer model_importer;
		// generate a scene which contains the root of all the data in a 3D model file; you call the function ReadFile from the model importer var/object and enter the path along with the options you want to add when the 3D model gets imported into the a scene
		const aiScene* model_scene = model_importer.ReadFile(path_to_3D_model_filetype, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check if scene doesn't get generated
		// remember we use the -> when we have a pointer and we want to call methods/functions that are within that pointer, its the equivelent to *. 
		if (!model_scene || model_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model_scene->mRootNode)
		{
			std::cout << "ERROR::OPEN_ASSET_IMPORTER_(ASSIMP)_::" << model_importer.GetErrorString();
			// return and basically stop the rest of the function here if there is an error
			return;
		}
		// get the directory name of where all the model files/textures are at and store it in that public method that we defined prior
		model_files_directory = path_to_3D_model_filetype.substr(0, path_to_3D_model_filetype.find_last_of('/'));

		// process the model's nodes that are located within the node of the model object's scene
		Parse_Node(model_scene->mRootNode, model_scene);

	}

	// process/parse through the object's nodes and retrieve the data that is within
	// this function is recursive meaning it will call itself until all of the data within is processed
	void Parse_Node(aiNode* model_node, const aiScene* model_scene)
	{
		// process all of the meshes that are located within the current node
		for (unsigned int mesh = 0; mesh < model_node->mNumMeshes; mesh++)
		{
			// the node obj only has indices to index the actual objects within the scene
			// the scene contains all the mesh data, the node is just there for organization (such as the relation between nodes)
			// so the scene has all the mesh data so we use a this pointer to grab that in addition to the node this pointer within the index of the meshes stored in an array or something similar to get the location
			aiMesh* meshes = model_scene->mMeshes[model_node->mMeshes[mesh]];
			// now push this data into the model_meshes member vector
			// we use the meshes aiMesh variable as well as the model scene from the argument from the parse node function for the Parse_Mesh function
			model_meshes.push_back(Parse_Mesh(meshes, model_scene));
			
		}

		// after all meshes are processed, processes all of the children nodes within the root node and recursively check for meshes and addiontal nodes in there until there are none left
		for (unsigned int cNode = 0; cNode < model_node->mNumChildren; cNode++)
		{
			// recursivley parse all of the children nodes and see if they have any data within until there are no child nodes left
			Parse_Node(model_node->mChildren[cNode], model_scene);
		}
	}

	// function that takes the Model_Mesh data type and goes through and processes all the data within
	Model_Mesh Parse_Mesh(aiMesh* model_mesh, const aiScene* model_scene)
	{
		// create vectors of the previous structs that we created in the Mesh class header file
		std::vector<vData_Struct> mesh_vData;
		std::vector<unsigned int> mesh_iData;
		std::vector<tData_Struct> mesh_tData;

		// go through each of the vertex data located within the mesh
		for (unsigned int vertices = 0; vertices < model_mesh->mNumVertices; vertices++)
		{

			// our place holder struct that holds all the different vertex data and pushes it to the official vertex data struct vector mesh_vData at the end
			// remember that this is only a single structure, not a vector of structures like mesh_vData 
			vData_Struct placeholder_vector_struct;
			// create a seperate 3 coordinate vector
			// placeholder vector to store the vertex data from assimp here
			glm::vec3 placeholder_vector;
			placeholder_vector.x = model_mesh->mVertices[vertices].x;
			placeholder_vector.y = model_mesh->mVertices[vertices].y;
			placeholder_vector.z = model_mesh->mVertices[vertices].z;
			
			
			placeholder_vector_struct.vertex_position_coordinates = placeholder_vector; // add our placeholder vector positional data to the glm::vec3 vertex_position_coordinates variable stored within the structure

			// now check if model_mesh has normal vertices
			if (model_mesh->HasNormals())
			{
				// overwrite your position data coordinates with the normal data coordinates using the same vector place_holder_vector
				placeholder_vector.x = model_mesh->mNormals[vertices].x;
				placeholder_vector.y = model_mesh->mNormals[vertices].y;
				placeholder_vector.z = model_mesh->mNormals[vertices].z;
				placeholder_vector_struct.vertex_normal_coordinates = placeholder_vector; // add our placeholder vector normal data to the glm::vec3 vertex_normal_coordinates variable stored within the structure 
			}
			// now check if model_mesh has texture coordinates
			if (model_mesh->mTextureCoords[0])
			{
				// create a new placeholder vector of 2 since texture coordinates only have that many vertices
				glm::vec2 placeholder_texture_vector;

				// a vertex can contain up to 8 different texture coordinates; we then make the assumption to not use models where a vertex can have multiple texture coordinates so always take the first set (index 0)
				placeholder_texture_vector.x = model_mesh->mTextureCoords[0][vertices].x;
				placeholder_texture_vector.y = model_mesh->mTextureCoords[0][vertices].y;

				placeholder_vector_struct.vertex_texture_coordinates = placeholder_texture_vector;

				if (model_mesh->HasTangentsAndBitangents())
				{

					// use the original place holder vector for the tangents
					placeholder_vector.x = model_mesh->mTangents[vertices].x;
					placeholder_vector.y = model_mesh->mTangents[vertices].y;
					placeholder_vector.z = model_mesh->mTangents[vertices].z;


					placeholder_vector_struct.vertex_tangent_coordinates = placeholder_vector;

					// use the original place holder vector for the bit tangents as well
					placeholder_vector.x = model_mesh->mBitangents[vertices].x;
					placeholder_vector.y = model_mesh->mBitangents[vertices].y;
					placeholder_vector.z = model_mesh->mBitangents[vertices].z;

					placeholder_vector_struct.vertex_bit_tangent_coordinates = placeholder_vector;
				}

			}
			else
				// if there are no texture coordinates set the texture coordinates to a vector of 2 with location (0.0, 0.0)
				placeholder_vector_struct.vertex_texture_coordinates = glm::vec2(0.0f, 0.0f);

			// push the placeholder_vector_struct to the mesh_vData vector that holds vData_structs
			mesh_vData.push_back(placeholder_vector_struct);
		}
		// parse through each of the mesh's faces (a face is a mesh's triangle) and get the corresponding vertex indices
		for (unsigned int face = 0; face < model_mesh->mNumFaces; face++)
		{
			// create a variable with the aiFace data type that stores the amount of faces within the model that is loaded
			aiFace model_face = model_mesh->mFaces[face];
			// nested for loop that goes through the each face, grabs the indices, and puts them into 
			for (unsigned int indices = 0; indices < model_face.mNumIndices; indices++)
				// push into the vector mesh_iData that stores unsigned ints for all the indices in each of the model's faces
				mesh_iData.push_back(model_face.mIndices[indices]);
		}

		// process the materials within the models directory which are the textures 

		// create a pointer variable of the aiMaterial data type and assign it to the data mMaterials which is found in the scene
		aiMaterial* model_material = model_scene->mMaterials[model_mesh->mMaterialIndex];
		// we create a naming convention for each different textuer type within the shaders

		// check the mesh class to get more info on our naming conventions

		// diffuse: diffTex

		// specular: specTex

		// normal: normTex

		// height: heightTex

		// create a vector with the tData_Struct type that will store our diffuseMaps
		// we run the function Load_Textures_From_Materials and store the return placeholder_texture_vector in the diffMaps vector
		// for the args we use the model_material pointer, aiTextureTypeDiffuse enumeration type, as well as a string with our naming convention for our shaders 
		std::vector<tData_Struct> diffMaps = Load_Textures_From_Materials(model_material, aiTextureType_DIFFUSE, "diffTex");
		// within the mesh_tData vector, insert the diffMaps vector data with the C++ built in function insert
		// we will start with ending the inital data within textures, then, we will begin adding the diffMaps vector into the mesh_tData vector, then end the insertion of the diffMaps vector
		mesh_tData.insert(mesh_tData.end(), diffMaps.begin(), diffMaps.end());
		
		// create a vector for the specular maps
		std::vector<tData_Struct> specMaps = Load_Textures_From_Materials(model_material, aiTextureType_SPECULAR, "specTex");
		// add the specMaps to the mesh_tData vector using the built-in C++ function insert
		mesh_tData.insert(mesh_tData.end(), specMaps.begin(), specMaps.end());

		// create a vector for the normal maps
		std::vector<tData_Struct> normMaps = Load_Textures_From_Materials(model_material, aiTextureType_NORMALS, "normTex");
		// add the normMaps to the mesh_tData vector usinggg the built-in C++ function insert
		mesh_tData.insert(mesh_tData.end(), normMaps.begin(), normMaps.end());

		// create a vector for the height maps
		std::vector<tData_Struct> heightMaps = Load_Textures_From_Materials(model_material, aiTextureType_HEIGHT, "heightTex");
		// add the heightMaps to the to the mesh_tData vector using built-in C++ function insert
		mesh_tData.insert(mesh_tData.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object with the constructor values being the data we've gathered from this Parse_Mesh function
		return Model_Mesh(mesh_vData, mesh_iData, mesh_tData);

	}

	// Load_Textures_From_Materials is a private function that checks all material textures of a type (i.e. diffuse, specular, normal) and loads them if they already haven't been loaded yet
	// all of the information is then returned as a tData_Struct
	// takes a std::vector of our tData_Struct
	// takes in 3 arguments: an pointer with the the aiMaterial datatype, an aiTexture type, and a string that specifies the type with our previous naming convention
	std::vector<tData_Struct> Load_Textures_From_Materials(aiMaterial* model_material, aiTextureType tex_type, std::string tex_type_name)
	{
		// create a placeholder vector holding the tdata_Struct type
		std::vector<tData_Struct> placeholder_texture_vector;
		// create a for loop that will parse through how many textures are in the specific texture type specified within the tex_type argument
		for (unsigned int texture = 0; texture < model_material->GetTextureCount(tex_type); texture++)
		{
			// create a variable with the aiString data type
			aiString assimp_string;

			// parse through the model_material and use the built in function GetTexture within the model_material object
			// provide the GetTexture method function with the texture type, our loop argument (unsigned int texture = 0), as well as the address of the assimp string we just created previously
			model_material->GetTexture(tex_type, texture, &assimp_string);
			// this is where we check if the texture was loaded prior
			// create a boolean variable that is set to false
			bool skip_tex = false;
			// nested for loop that parses tex_already_loaded vector with assimp_string and compares if the paths match
			for (unsigned int already_loaded_texture = 0; already_loaded_texture < tex_already_loaded.size(); already_loaded_texture++)
			{
				// use the standard library builtin function strcmp to comprare the 2 strings to see if the paths match
				// I believe the == 0 means if the strcmp is true since 0 is true and 1 is false for booleans
				if (std::strcmp(tex_already_loaded[already_loaded_texture].tex_path.data(), assimp_string.C_Str()) == 0)
				{
					// push the already loaded texture into the placeholder_texture_vector
					placeholder_texture_vector.push_back(tex_already_loaded[already_loaded_texture]);
					// assign skip_tex value to true and move onto the next texture by breaking out of this nested for loop
					skip_tex = true;
					// break out of the nested for loop
					break;
				}
			}
				// if the tex_skip is still false set up the texture with the data provided from assimp
				if (!skip_tex)
				{
					// create a tData_struct
					tData_Struct material_tData;
					// generate an OpenGL texture object using the Textures_In_Model_Directory function
					// we use the assimp string as our path as well as the public model_files_directory method we are using for this object
					material_tData.texID = Textures_In_Model_Directory(assimp_string.C_Str(), this->model_files_directory);
					// we now assign the naming convention to the texture type
					material_tData.tex_type = tex_type_name;
					// we now provide the full path to the texture via assimp_string
					material_tData.tex_path = assimp_string.C_Str();
					// now we take the newly added data into our material_tData struct and push that struct into the placeholder_texture_vector
					placeholder_texture_vector.push_back(material_tData);
					// push back the material_tdata into the tex_already_loaded vector to store so we don't load duplicate textures
					tex_already_loaded.push_back(material_tData);
				}
			

		}
		// return that placeholder_texture_vector
		return placeholder_texture_vector;
	}

};

// Textures_In_Model_Directory is a function that takes the textures that are within the model's directory and loads them into a OpenGL format with stb_image
// It also sets the repeat and filtering options of the textures as well

unsigned int Textures_In_Model_Directory(const char* texture_file_path, const std::string& model_files_directory, bool gamma)
{
	
	// convert texture_file_path into a string
	std::string name_of_texture_file = std::string(texture_file_path);

	// concat the directory that the texture files are located in along with a / and then the name of the texture file
	name_of_texture_file = model_files_directory + "/" + name_of_texture_file;

	// create an unsigned int for a texture ID
	unsigned int texID;
	// generate a texture object using glGenTextures, we only want to create one object and provide the address of our texture ID
	glGenTextures(1, &texID);
	// provide the width, height, and ammount of RGB components
	int w, h, amount_of_RGB_components;

	// load texture with name_of_texture_file converted to a c-string as well as the addresses of the width, height and amount of RGB components
	unsigned char* tex_data = stbi_load(name_of_texture_file.c_str(), &w, &h, &amount_of_RGB_components, 0); 
	
	// if there is texture data loaded from stbi_load, run this block of code
	if (tex_data)
	{
		// Create a GLenum variable that takes the amount of color components that where gathered prior from the tex_data pointer and assign it to an enumeration that matches the color components gathered
		GLenum tex_col_format;
		if (amount_of_RGB_components == 1)
			tex_col_format = GL_RED;
		if (amount_of_RGB_components == 3)
			tex_col_format = GL_RGB;
		if (amount_of_RGB_components == 4)
			tex_col_format = GL_RGBA;

		// bind the texture object to the GL_TEXTURE_2D type enumeration
		glBindTexture(GL_TEXTURE_2D, texID);
		// enter the configuration you want within the glfw function glTexImaage 2D
		glTexImage2D(GL_TEXTURE_2D, 0, tex_col_format, w, h, 0, tex_col_format, GL_UNSIGNED_BYTE, tex_data);
		// generate a mipmap with this glfw function
		glGenerateMipmap(GL_TEXTURE_2D);
		// add our wrapping and filtering options on the current texture object

		// texture wrapping options for the s coordinates
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S is our X for texture coordinates
		// texture wrapping options for the t coordinates
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T is our Y for texture coordinates
		// set filtering options for minimum (for textures on objects that are further away)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // for the min texture filter, use the mipmap that was generated as well as the linear filtering method
		// set filtering options for magnification (for textures on objects that are closer)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // for the mag texture filter, use only the linear filtering method, no mipmap
		
		// free the previous image data with this stb image function for the next texture object
		stbi_image_free(tex_data);

	}
	// if you cannot load texture data from path, send ERROR MESSAGE to default output and free the texture data with stbi_image_free
	else
	{
		std::cout << "ERROR::CANNOT_LOAD_TEXTURE_AT_PATH: " << texture_file_path << std::endl;

		stbi_image_free(tex_data);
	}
	
	return texID;
}
