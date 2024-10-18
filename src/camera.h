#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	void setYaw(float yaw);
	void setPitch(float pitch);

	float getYaw() const;
	float getPitch() const;

	void moveYaw(float delta);
	void movePitch(float delta);

	void setPosition(glm::vec3 position);
	void setDirection(glm::vec3 direction);
	void setWorldUp(glm::vec3 worldUp);

	void updateAspectRatio(int width, int height);
	void setFOV(float fov);

	glm::vec3 getPosition() const;

	void move(glm::vec3 delta);
	void moveRelative2D(glm::vec2 delta);

	void update();

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

private:
	void updateCameraVectors();

	float yaw = 0.0f;
	float pitch = 0.0f;
	// float roll = 0.0f;

	glm::vec3 position {0.0f, 0.0f, 0.0f};
	glm::vec3 direction {0.0f, 0.0f, 1.0f};
	glm::vec3 worldUp {0.0f, 1.0f, 0.0f};

	glm::vec3 right {};
	glm::vec3 up {};

	glm::mat4 view {};
	glm::mat4 projection {};

	float aspectRatio = 1.0f;
	float fov = 90.0f;
	float minRange = 0.1f;
	float maxRange = 100.0f;
};