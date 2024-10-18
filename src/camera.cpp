#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera()
{
	updateCameraVectors();
}

void Camera::setYaw(float yaw)
{
	this->yaw = yaw;
}

void Camera::setPitch(float pitch)
{
	this->pitch = pitch;
}

float Camera::getYaw() const
{
	return yaw;
}

float Camera::getPitch() const
{
	return pitch;
}

void Camera::moveYaw(float delta)
{
	yaw += delta;
	yaw = std::fmod(yaw, 360.0f);
}

void Camera::movePitch(float delta)
{
	pitch += delta;
	pitch = std::clamp(pitch, -89.9f, 89.9f);
}

void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Camera::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}

void Camera::setWorldUp(glm::vec3 worldUp)
{
	this->worldUp = worldUp;
	updateCameraVectors();
}

void Camera::updateAspectRatio(int width, int height)
{
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Camera::setFOV(float fov)
{
	this->fov = fov;
}

glm::vec3 Camera::getPosition() const
{
	return position;
}

void Camera::move(glm::vec3 delta)
{
	position += delta;
}

void Camera::moveRelative2D(glm::vec2 delta)
{
	glm::vec3 moveVector {0.0f, 0.0f, 0.0f};

	moveVector += delta.x * glm::normalize(glm::cross(direction, up));
	moveVector += delta.y * direction;

	move(moveVector);
}

void Camera::update()
{
	direction.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
	direction.y = static_cast<float>(sin(glm::radians(pitch)));
	direction.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	direction = glm::normalize(direction);

	right = glm::normalize(glm::cross(direction, worldUp));
	up = glm::normalize(glm::cross(right, direction));

	view = glm::lookAt(position, position + direction, up);
	projection = glm::perspective(glm::radians(fov), aspectRatio, minRange, maxRange);
}

glm::mat4 Camera::getViewMatrix() const
{
	return view;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return projection;
}

void Camera::updateCameraVectors()
{
	// Update direction vector
	direction = glm::vec3 {};
	direction.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
	direction.y = static_cast<float>(sin(glm::radians(pitch)));
	direction.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	direction = glm::normalize(direction);

	// Recalculate right and up vectors
	right = glm::normalize(glm::cross(direction, worldUp));
	up = glm::normalize(glm::cross(right, direction));
}