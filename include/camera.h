#pragma once


#include <stdio.h>
#include <glm/glm.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;


	float Yaw;
	float Pitch;

	const float MovementSpeed = 1.5f;
	float MouseSensitivity = 1.f;



};


void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

// Cam is camera object
// Target is position in world space that should be the focus.
// Distance is distance from camera to target
// x and y offset are amount camera will move 
void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset){
	// Modify the camera yaw using xoffset
	// // Needs to be negative as pitch and yaw is reversed in the camera. 
	// Moves camera left and right.
	in.Yaw -= xoffset * in.MovementSpeed;

	// Modify the camera pitch using yoffset.
	// Moves camera up and down.
	in.Pitch -= yoffset * in.MovementSpeed;

	if (in.Pitch > 89.f) in.Pitch = 89.f;
	if (in.Pitch < -89.f) in.Pitch = -89.f;

	// Calculate new camera position around outside of sphere
	// Convert pitch and yaw to radians when using them
	float theta = glm::radians(in.Yaw);
	float alpha = glm::radians(in.Pitch);

	// Equations from lecture ch10
	float px = cos(theta) * cos(alpha);
	float py = sin(alpha);
	float pz = sin(theta) * cos(alpha);

	in.Position = glm::vec3(px, py, pz) * distance;

	// Calculate the new Front direction vector using new position.
	// This will point at the target.

	// Subtract camera position from target position to get a vector from the cam to target location
	// Normalise that vector to get direction from camera to target location
	in.Front = glm::normalize(target - in.Position);
	

	// Calculate new Right direction vector for the camera. 
	// Points at a tangent to the sphere, along the equator.
	// Use cross product to calculate new right vector.
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));


	// Calculate new up direction vector, pointing at the tangent of the sphere.
	// Use cross product with camera right vec and camera front vec
	// Normalise to get direction from camera to pole
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
}



// For first person camera
void OrientFirstPersonCamera(SCamera& camera, float xoffset, float yoffset)
{
	// Adjust mouse sensitivity
	xoffset *= camera.MouseSensitivity;
	yoffset *= camera.MouseSensitivity;

	// Update yaw and pitch
	camera.Yaw += xoffset;
	camera.Pitch += yoffset;

	// Clamp the pitch to prevent flipping
	if (camera.Pitch > 89.0f)
		camera.Pitch = 89.0f;
	if (camera.Pitch < -89.0f)
		camera.Pitch = -89.0f;

	// Calculate new front vector
	glm::vec3 front;
	front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	front.y = sin(glm::radians(camera.Pitch));
	front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	camera.Front = glm::normalize(front);

	// Recalculate right and up vectors
	camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
	camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

void InitFirstPersonCamera(SCamera& camera, glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f))
{
	camera.Position = startPosition;
	// Increase starting height
	camera.Position.y = 2.f;
	camera.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	camera.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	// Z axis
	camera.Yaw = -90.0f; 
	camera.Pitch = 0.0f;
	camera.MouseSensitivity = 0.1f;

	// Calculate Right and Up vectors from initial Front
	camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
	camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

glm::mat4 GetViewMatrix(const SCamera& camera)
{
	return glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
}