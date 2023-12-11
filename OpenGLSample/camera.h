#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

//Constraints
const float MAX_PITCH = 89.0f;                           //Max Look Up
const float MIN_PITCH = -89.0f;                          //Max Look Down

const float MIN_FOV = 1.0f;                              //Minimum FOV           
const float MAX_FOV = 175.0f;                            //Maximum FOV

const float MIN_MOVE_SPEED = 0.1f;                       //Minimum move speed
const float MAX_MOVE_SPEED = 100.0f;                     //Maximum move speed

// Default camera values
const float DEFAULT_YAW = -90.0f;                        //Default YAW (points to -z)
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_FOV = 100.0f;                        //Default FOV value
const float DEFAULT_MOVE_SPEED = 3.0f;                   //Default Movement Speed
const float DEFAULT_SENSITIVITY = 0.15f;                 //Default Mouse sensitivity

const float DEFAULT_SCROLL_MOD = 5.0f;                   //Adjusts the scroll speed


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float CurrentFOV;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(DEFAULT_MOVE_SPEED), MouseSensitivity(DEFAULT_SENSITIVITY), CurrentFOV(DEFAULT_FOV)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(DEFAULT_MOVE_SPEED), MouseSensitivity(DEFAULT_SENSITIVITY), CurrentFOV(DEFAULT_FOV)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position += Up * velocity;
		if (direction == DOWN)
			Position -= Up * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > MAX_PITCH)
				Pitch = MAX_PITCH;
			if (Pitch < MIN_PITCH)
				Pitch = MIN_PITCH;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset, bool modifierEnable)
	{
		float increase = 0.0;
		float modifier = 1.0;

		//If modifier enabled, then adjust it
		modifierEnable ? modifier = DEFAULT_SCROLL_MOD : modifier = 1.0f;

		/*CurrentFOV -= (float)yoffset * DEFAULT_SCROLL_MOD;
		if (CurrentFOV < MIN_FOV)
			CurrentFOV = MIN_FOV;
		if (CurrentFOV > MAX_FOV)
			CurrentFOV = MAX_FOV;*/

		increase = ((float)yoffset / 10) * modifier; //offset results in +- 1, so divide by 10 to get .1 increments
		MovementSpeed += increase;

		yoffset > 0 ? std::cout << "SPEED INCREASED" << std::endl : std::cout << "SPEED INCREASED" << std::endl;

		if (MovementSpeed < MIN_MOVE_SPEED)
			MovementSpeed = MIN_MOVE_SPEED;
		if (MovementSpeed > MAX_MOVE_SPEED)
			MovementSpeed = MAX_MOVE_SPEED;

		std::cout << "CURRENT SPEED: " << MovementSpeed << std::endl;
	}

	//Resets the FOV to the default
	void ResetFOV() {
		CurrentFOV = DEFAULT_FOV;
	}

	//Resets the Move Speed to the default
	void ResetMoveSpeed() {
		MovementSpeed = DEFAULT_MOVE_SPEED;
	}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif