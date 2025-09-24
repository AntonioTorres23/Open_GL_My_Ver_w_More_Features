#ifndef MAM
#define MAM

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum KEYBOARD_MOVEMENT {
	W,
	S,
	A,
	D
};

float CAM_YAW = -90.0f; // default value of how left or right we are looking in our view vector, set to -90 by default to look towards negative z-axis
float CAM_PITCH = 0.0f; // default value of how up or down we are looing in our view vector, set to 0 by default for no effect at first
float CAM_SPEED = 1.5f; // global value of how fast we want the camera to move within the world environment, will be multiplied to the keybindings
float MOUSE_SENS = 0.1f; // global value of how fast we want our camera to move left, right, down, or up in terms of the view vector/when we move the mouse via call back


class Cam_AND_Mov
{
public:
	// public methods that are stored with the Cam_AND_Mov OBJ
	glm::vec3 obj_cam_pos;
	glm::vec3 obj_cam_front_view;
	glm::vec3 obj_up;
	glm::vec3 obj_cam_right;
	glm::vec3 obj_cam_world_up;
	float obj_yaw;
	float obj_pitch;
	float obj_cam_speed;
	float obj_mouse_sens;

	// PROGRAMER'S NOTE: I THINK THAT WE INITIALIZE THE FRONT VECTOR RATHER THAN TAKE IT AS AN ARGUMENT BECAUSE IT WILL ALWAYS CONTAIN THE COORDINATES (0.0f, 0.0f, -1.0f) 

	// create a constructor with glm::vec3 variables importing or setting/initializing the values of the public methods before within this constructor
	// simply just creating values or importing them for the arguments then assigning them to the prior public methods
	Cam_AND_Mov(glm::vec3 arg_pos = glm::vec3(0.0f, 0.0f, 0.0), glm::vec3 arg_up = glm::vec3(0.0f, 1.0f, 0.0f), float arg_yaw = CAM_YAW, float arg_pitch = CAM_PITCH) : obj_cam_front_view(glm::vec3(0.0f, 0.0f, -1.0f)), obj_mouse_sens(MOUSE_SENS), obj_cam_speed(CAM_SPEED)
	{
		obj_cam_pos = arg_pos;
		
		obj_cam_world_up = arg_up;
		
		obj_yaw = arg_yaw;
		
		obj_pitch = arg_pitch;

		update_cam_obj_vectors();
	}

	// overloaded constructor if scalar x, y, z values are used for the position and world up vectors

	Cam_AND_Mov(float obj_pos_x_coord, float obj_pos_y_coord, float obj_pos_z_coord, float obj_up_x_coord, float obj_up_y_coord, float obj_up_z_coord, float arg_obj_yaw, float arg_obj_pitch) : obj_cam_front_view(glm::vec3(0.0f, 0.0f, -1.0f)), obj_cam_speed(CAM_SPEED), obj_mouse_sens(MOUSE_SENS)
	{
		obj_cam_pos = glm::vec3(obj_pos_x_coord, obj_pos_y_coord, obj_pos_z_coord);

		obj_cam_world_up = glm::vec3(obj_up_x_coord, obj_up_y_coord, obj_up_z_coord);

		obj_yaw = arg_obj_yaw;

		obj_pitch = arg_obj_yaw;

		update_cam_obj_vectors();
	}

	glm::mat4 Obtain_View_Matrix()
	{
		// for view matrix that provides the position, front, and world up vectors using the methods defined before 
		return glm::lookAt(obj_cam_pos, obj_cam_pos + obj_cam_front_view, obj_up);
	}

	// movement of the camera's position, think of this as moving around forward, backward, left, right within world space or like a level in a game
	void K_MOVEMENT(KEYBOARD_MOVEMENT key, float dTime)
	{
		// the velocity or speed at which we want to move forward, backward, left, or right in world space
		float vel = obj_cam_speed * dTime;

		if (key == W)
			// within the obj_cam_pos add the product of object_cam_front_view vector times the velocity scalar value to the position vector
			// this moves the camera forward
			obj_cam_pos += obj_cam_front_view * vel;
		if (key == S)
			// same concept as prior but we are subtracting since we are moving backwards
			obj_cam_pos -= obj_cam_front_view * vel;
		if (key == D)
			// within the obj_cam_pos add the product of object_cam_right vector times the velocity scalar value to the position vector
			obj_cam_pos += obj_cam_right * vel;
		if (key == A)
			// same concept as prior but we are subtracting since we are moving left
			obj_cam_pos -= obj_cam_right * vel;

	}

	// sends our offsets to get multiplied by the mouse sensitivity and then added to the yaw and pitch methods which are then calculated in a private function
	// if our pitch is over 89 or -89, limit it by setting it to 89 or -89 
	void M_MOVEMENT(float yaw_offset, float pitch_offset, GLboolean limitPitch = true)
	{

		yaw_offset *= obj_mouse_sens; 

		pitch_offset *= obj_mouse_sens;

		obj_yaw += yaw_offset;

		obj_pitch += pitch_offset; 

		if (limitPitch)
		{
			if (obj_pitch > 89.0f)
				obj_pitch = 89.0f;
			if (obj_pitch < -89.0f)
				obj_pitch = -89.0f;
		}

		update_cam_obj_vectors();
	}

private:

	void update_cam_obj_vectors()
	{
		// vector where we calculate
		glm::vec3 calc_front_view_vector;
		// for x we use the theta (yaw) and need to get the cos of yaw and the cos of pitch since they both effect each other in 3D environments
		// the yaw and pitch also need to be in radians
		calc_front_view_vector.x = cos(glm::radians(obj_yaw)) * cos(glm::radians(obj_pitch));
		// since y is not effected by the x or z axes we only need to get the sin of the pitch
		calc_front_view_vector.y = sin(glm::radians(obj_pitch));
		// for z we need to get the sin of our yaw and the cos of pitch since the x and z axes intersect with the y axis at the same point
		calc_front_view_vector.z = sin(glm::radians(obj_yaw)) * cos(glm::radians(obj_pitch));
		// add this result to the obj_cam_front_view as well as normalize to make sure the length/magnitude is 1
		obj_cam_front_view = glm::normalize(calc_front_view_vector);
		// generate the new right vector that works with the camera's left and right position movement this is done via a cross product which generates a vector that is perpendiular to the two you mutliplied
		obj_cam_right = glm::normalize(glm::cross(obj_cam_front_view, obj_cam_world_up));
		// generate a new up vector for the lookat matrix
		obj_up = glm::normalize(glm::cross(obj_cam_right, obj_cam_front_view));

	}
	

};

#endif

