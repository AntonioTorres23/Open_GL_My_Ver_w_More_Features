// Preprocessor feature that if this header file doesn't exist create it. This is called an include guard
// And prevents multiple inclusion
#ifndef SHADER_CLASS
#define SHADER_CLASS


#include <glad/glad.h>
#include <glm/glm.hpp>

// All used for reading and writing your vertex and fragment shader files
#include <string>
#include <fstream>
#include <sstream>

// Used to display default output: you know what this library is used for
#include <iostream>

// create an object class called sdr
class SDR
{
// public members/functions that are accessible outside of the class
public:
	// unsigned ints are any integer that isn't a negative including zero 
	// this is the ID that ties to the shader program
	unsigned int shader_program_id;
	// constructor that reads the vertex and fragment shader and builds the shader program
	// takes the file name and path of the vertex and fragment shader files as its arguments
	SDR(const char* vertex_shader_file, const char* fragment_shader_file)
	{
		// string to store vertex source code
		std::string vertex_shader_source_code;
		// string to store fragment source code
		std::string fragment_shader_source_code;
		// ifstream which reads the vertex file
		std::ifstream vertex_source_stream;
		// ifstream which reads the fragment file
		std::ifstream fragment_source_stream;
		// enable failbits for the source streams so that they can through exceptions in try blocks
		vertex_source_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragment_source_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		// try block to attempt to read shader files
		try 
		{
			// attempt to open the vertex and fragment files
			vertex_source_stream.open(vertex_shader_file);
			fragment_source_stream.open(fragment_shader_file);
			// create a vertex and string stream to transfer the ifstream previously
			std::stringstream vertex_string_stream, fragment_string_stream;
			// read the buffers of the ifstream variables to the string stream variables using the << symbol
			vertex_string_stream << vertex_source_stream.rdbuf();
			fragment_string_stream << fragment_source_stream.rdbuf();
			// close the ifstreams
			vertex_source_stream.close();
			fragment_source_stream.close();
			// convert from string stream just to a regular string
			vertex_shader_source_code = vertex_string_stream.str();
			fragment_shader_source_code = fragment_string_stream.str();
		}
		// this is your catch statment if anything goes wrong with reading the source code from the shader files
		catch (std::ifstream::failure &error_ifstream)
		{
			std::cout << "ERROR::SHADER::COULD_NOT_READ_FILE" << std::endl;
		}
		// convert the string source code to c strings
		const char* c_vertex_source_code = vertex_shader_source_code.c_str();
		const char* c_fragment_source_code = fragment_shader_source_code.c_str();
		// create an unsigned int for the vertex shader and fragment shader
		unsigned int vertex_shader, fragment_shader;
		// create a shader with glCreateShader function and use GL_VERTEX_SHADER enum
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		// add the source code to the vertex shader
		glShaderSource(vertex_shader, 1, &c_vertex_source_code, NULL);
		// compile the vertex shader 
		glCompileShader(vertex_shader);
		// use compile_shader_check to check for any compiling errors
		compile_shader_check(vertex_shader, "VERTEX");
		// create a fragment shader with glCreateShader and use GL_FRAGMENT_SHADER enum
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		// add the source code to the fragment shader
		glShaderSource(fragment_shader, 1, &c_fragment_source_code, NULL);
		// compile the fragment shader
		glCompileShader(fragment_shader);
		// use compile_shader_check to check for any compiling errors
		compile_shader_check(fragment_shader, "FRAGMENT");
		// the program to link the two shaders together
		shader_program_id = glCreateProgram();
		// attach vertex shader to shader program
		glAttachShader(shader_program_id, vertex_shader);
		// attach fragment shader to shader program
		glAttachShader(shader_program_id, fragment_shader);
		// link the two shaders together in the shader program
		glLinkProgram(shader_program_id);
		// check for any errors while linking the two shaders in the shader program
		compile_shader_check(shader_program_id, "PROGRAM");
		// delete the vertex and fragment shaders since they are not needed anymore
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		

	}

	// create a void function that uses the shader program you just created
	void activate_shader()
	{
		// glfw function that takes in the shader program ID and uses it to run the shader program
		glUseProgram(shader_program_id);
	}

	// PROGRAMER'S NOTE: the const after the argument is used to not modify the state of the object on which it is called

	// sets whatever name of the uniform bool variable in the shaders to the value you desire within the main file
	void uniform_bool(const std::string &uniform_name, bool boolean_value) const
	{
		glUniform1i(glGetUniformLocation(shader_program_id, uniform_name.c_str()), (int)boolean_value);
	}

	// sets whatever name of the uniform int variable in the shaders to the value you desire within the main file

	void uniform_int(const std::string &uniform_name, int integer_value) const
	{
		glUniform1i(glGetUniformLocation(shader_program_id, uniform_name.c_str()), integer_value);
	}

	// sets whatever name of the uniform float variable in the shaders to the value you desire within the main file

	void uniform_float(const std::string &uniform_name, float float_value) const
	{
		glUniform1f(glGetUniformLocation(shader_program_id, uniform_name.c_str()), float_value);

	}

	// sets whatever name of the uniform vec2 variable in the shaders to the values you desire within the main file, takes in the vec2 glm variable in this instance

	void uniform_vector_2(const std::string &uniform_name, glm::vec2 vector_2_values) const
	{
		glUniform2fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, &vector_2_values[0]);
	}

	// sets whatever name of the uniform vec2 variable in the shaders to the x and y values you desire within the main file, this overloads the uniform_vector_2 function to take either a vec2 variable or two float values

	void uniform_vector_2(const std::string &uniform_name, float x_coordinate, float y_coordinate) const
	{
		glUniform2f(glGetUniformLocation(shader_program_id, uniform_name.c_str()), x_coordinate, y_coordinate);
	}

	// sets whatever name of the uniform vec3 variable in the shaders to the values you desire within the main file, takes in the vec3 glm variable in this instance

	void uniform_vector_3(const std::string &uniform_name, glm::vec3 vector_3_values) const
	{
		glUniform3fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, &vector_3_values[0]);
	}

	// sets whatever name of the uniform vec3 variable in the shaders to the x, y, and z values you desire within the main file, this overloads the uniform_vector_3 function to take either a vec3 variable or three float values

	void uniform_vector_3(const std::string  &uniform_name, float x_coordinate, float y_coordinate, float z_coordinate) const
	{
		glUniform3f(glGetUniformLocation(shader_program_id, uniform_name.c_str()), x_coordinate, y_coordinate, z_coordinate);
	}

	// sets whatever name of the uniform vec4 variable in the shaders to the values you desire within the main file, takes in the vec4 glm variable in this instance

	void uniform_vector_4(const std::string &uniform_name, glm::vec4 vector_4_values) const
	{
		glUniform4fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, &vector_4_values[0]);
	}

	// sets whatever name of the uniform vec4 variable in the shaders to the x, y, z, w value you desire within the main file, this overloads the uniform_vector_4 function to take either a vec4 variable or four float values
	
	void uniform_vector_4(const std::string &uniform_name, float x_coordinate, float y_coordinate, float z_coordinate, float w_coordinate) const 
	{
		glUniform4f(glGetUniformLocation(shader_program_id, uniform_name.c_str()), x_coordinate, y_coordinate, z_coordinate, w_coordinate);
	}

	// sets whatever name of the uniform mat2 variable in the shaders to the values you desire within the main file, takes in a mat2 variable

	void uniform_matrix_2(const std::string &uniform_name, glm::mat2 &matrix_2_values) const
	{
		// I believe there are two indexes on the matrix_2_values variable is becuase it specifies both the rows and columns that are within a matrix
		glUniformMatrix2fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, GL_FALSE, &matrix_2_values[0][0]);
	}

	// sets whatever name of the uniform mat3 variable in the shaders to the values you desire within the main file, takes in a mat3 variable
	void uniform_matrix_3(const std::string &uniform_name, glm::mat3 &matrix_3_values) const
	{
		glUniformMatrix3fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, GL_FALSE, &matrix_3_values[0][0]);
	}

	// sets whatever name of the uniform mat4 variable in the shaders to the values you desire within the main file, takes in a mat4 variable

	void uniform_matrix_4(const std::string &uniform_name, glm::mat4 &matrix_4_values) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shader_program_id, uniform_name.c_str()), 1, GL_FALSE, &matrix_4_values[0][0]);
	}

private:

	// function that checks if compile was successful
	void compile_shader_check(GLuint sdr, std::string sdr_type)
	{
		// variable that signifies compile is complete
		GLint successful_compile;
		// 1024 GLchar long c style array for storing log information
		GLchar errorlog[1024];
		// if else statments that depending on the sdr_type string argument will check for compile status of either a vertex shader, or shader program
		if (sdr_type != "PROGRAM")
		{
			// OpenGL function that checks the status of the compile and returns a GLint value for successful_compile varible
			glGetShaderiv(sdr, GL_COMPILE_STATUS, &successful_compile);
			// if glGetShaderiv doesn't return a GLint for successful_compile, get the errors and store it in the error log
			if (!successful_compile)
			{
				// gets the log infomation of the compile failure
				glGetShaderInfoLog(sdr, 1024, NULL, errorlog);
				// output the errorlog to termial (default output)
				std::cout << sdr_type << "_SHADER_ERROR:" << "\n" << errorlog << std::endl;
			}
		}
		else
		{
			// function that checks compilation status of shader program and returns a GLint value for successful_compile variable	
			glGetProgramiv(sdr, GL_LINK_STATUS, &successful_compile);
			// if glGetProgamiv doesn't return a GLint for successful_compile, get error log
			if (!successful_compile)
			{
				glGetProgramInfoLog(sdr, 1024, NULL, errorlog);
				std::cout << sdr << "Linking_ERROR:" << "\n" << errorlog << std::endl;
			}
		}

	}



};

#endif // !SHADER_CLASS