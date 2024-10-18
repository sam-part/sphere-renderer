#pragma once

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <string>
#include "camera.h"
#include "sphere.h"
#include "imgui/imgui-SFML.h"

class Application
{
public:
	Application();
	~Application();

	void run();

	void processInput();
	void update();
	void menu();
	void draw();

	void updateUIState();

private:
	Sphere sphere;

	sf::RenderWindow window {};
	GLenum drawMode = GL_LINE;
	bool colorful = false;

	Camera camera {};
	Shader shader {};

	bool running = false;

	const float mouseSensitivity = 0.1f;
	const float movementSpeed = 2.0f;
	const float radiusStep = 0.5f;

	const std::string windowName = "Sphere Renderer (Tab to toggle menu)";
	const int defaultWidth = 1600;
	const int defaultHeight = 900;

	const int defaultSectors = 18;
	const int defaultStacks = 18;

	float dt = 0.0f;

	bool uiOpen = false;
	sf::Vector2i mousePositionUI {defaultWidth / 2, defaultHeight / 2};

	sf::Clock clock;
};