#include "application.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <iostream>

Application::Application()
{
	// Create SFML window context
    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.minorVersion = 3;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    //settings.attributeFlags |= sf::ContextSettings::Core;

    window.create(sf::VideoMode(defaultWidth, defaultHeight), windowName.c_str(), sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
 

    if (!window.isOpen())
    {
        throw std::runtime_error("Could not open SFML window");
    }

    window.setActive(true);
    
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2i windowCenter(windowSize.x / 2, windowSize.y / 2);
    sf::Mouse::setPosition(windowCenter, window);

    // Initialize ImGui
    ImGui::SFML::Init(window);

    // Initiallize GLEW
    GLenum glewErr = glewInit();

    if (glewErr != GLEW_OK)
    {
        std::string errString = reinterpret_cast<const char*>(glewGetErrorString(glewErr));
        throw std::runtime_error("Could not initialize GLEW: " + errString);
    }

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, drawMode);

    shader = Shader("shaders/basic.vs", "shaders/basic.fs");

    sphere.init();
    sphere.generateIcosphere();
    sphere.sendBufferData();

    camera.setPosition({-2.0f, 0.0f, 0.0f});
    camera.updateAspectRatio(defaultWidth, defaultHeight);
}

Application::~Application()
{
    ImGui::SFML::Shutdown();
}

void Application::run()
{
    running = true;

    float lastFrameTime = clock.getElapsedTime().asSeconds();

    while (running && window.isOpen())
    {
        // Update frametime delta
        float currentTime = clock.getElapsedTime().asSeconds();
        dt = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        processInput();
        update();
        menu();
        draw();
    }
}

void Application::processInput()
{
    // Update mouse input
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2i windowCenter(windowSize.x / 2, windowSize.y / 2);

    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2i mouseOffset = mousePosition - windowCenter;

    // Only use/set mouse input if UI is not open
    if (!uiOpen)
    {
        sf::Mouse::setPosition(windowCenter, window);

        // Update camera with mouse input
        camera.moveYaw(mouseOffset.x * mouseSensitivity);
        camera.movePitch(-mouseOffset.y * mouseSensitivity);
    }

    // Handle window events
    sf::Event event;
    while (window.pollEvent(event))
    {
        if(uiOpen)
            ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::Closed)
        {
            // End the program
            running = false;
        }
        else if (event.type == sf::Event::Resized)
        {
            // Adjust the viewport when the window is resized
            glViewport(0, 0, event.size.width, event.size.height);
            camera.updateAspectRatio(event.size.width, event.size.height);
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            int key = event.key.code;

            if (key == sf::Keyboard::Escape)
            {
                running = false;
            }
            else if (key == sf::Keyboard::Tab)
            {
                // Enable or disable UI overlay

                uiOpen = !uiOpen;
                updateUIState();
            }
            else if(key == sf::Keyboard::Up)
            {
                int subdivisions = sphere.getSubdivisionLevel();
                sphere.subdivide(subdivisions + 1);
                sphere.sendBufferData();
            }
            else if(key == sf::Keyboard::Down)
            {
                int subdivisions = sphere.getSubdivisionLevel();

                if (subdivisions > 0)
                {
                    sphere.subdivide(subdivisions - 1);
                    sphere.sendBufferData();
                }
            }
            else if (key == sf::Keyboard::Right)
            {
                sphere.setRadius(sphere.getRadius() + radiusStep);
            }
            else if (key == sf::Keyboard::Left)
            {
                float radius = sphere.getRadius();

                if(radius > radiusStep + 0.1f)
                    sphere.setRadius(sphere.getRadius() - radiusStep);
            }
        }
    }
}

void Application::update()
{
    // Move camera
    glm::vec2 moveVector {0.0f, 0.0f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moveVector.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moveVector.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moveVector.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moveVector.x += 1;

    moveVector = moveVector * movementSpeed * dt;
    camera.moveRelative2D(moveVector);
    camera.update();
}

void Application::menu()
{
    if (!uiOpen)
        return;

    ImGui::SFML::Update(window, sf::seconds(dt));

    bool open = true;
    ImGui::Begin("Sphere Renderer", &open);

    // Check if ImGui window was closed
    if (!open)
    {
        uiOpen = false;
        updateUIState();

        ImGui::End();
        ImGui::EndFrame();

        return;
    }

    SphereType type = sphere.getType();

    if (ImGui::TreeNodeEx("Sphere Type", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Selectable("IcoSphere", type == SphereType::IcoSphere))
        {
            type = SphereType::IcoSphere;
            sphere.generateIcosphere();
            sphere.sendBufferData();
        }

        if (ImGui::Selectable("CubeSphere", type == SphereType::CubeSphere))
        {
            type = SphereType::CubeSphere;
            sphere.generateCubesphere();
            sphere.sendBufferData();
        }

        if (ImGui::Selectable("SectorSphere", type == SphereType::SectorSphere))
        {
            type = SphereType::SectorSphere;
            sphere.generateSectorsphere(defaultSectors, defaultStacks);
            sphere.sendBufferData();
        }

        ImGui::TreePop();
    }

    ImGui::NewLine();
    ImGui::PushItemWidth(100);

    int subdivisions = sphere.getSubdivisionLevel();
    if (ImGui::InputInt("Subdivisions", &subdivisions, 1, 1))
    {
        subdivisions = std::max(subdivisions, 0);

        sphere.subdivide(subdivisions);
        sphere.sendBufferData();
    }

    float radius = sphere.getRadius();
    if (ImGui::InputFloat("Radius", &radius, radiusStep, 0.5f))
    {
        radius = std::max(radius, radiusStep);
        sphere.setRadius(radius);
    }

    if(type == SphereType::SectorSphere)
    { 
        ImGui::NewLine();

        int sectors = sphere.getSectors();
        int stacks = sphere.getStacks();

        if (ImGui::InputInt("Sectors", &sectors, 1, 1) ||
            ImGui::InputInt("Stacks", &stacks, 1, 1))
        {
            sphere.generateSectorsphere(sectors, stacks);
            sphere.sendBufferData();
        }
    }

    ImGui::NewLine();

    if (ImGui::TreeNodeEx("Draw Mode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Selectable("Wireframe", drawMode == GL_LINE))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawMode = GL_LINE;
        }

        if (ImGui::Selectable("Point", drawMode == GL_POINT))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            drawMode = GL_POINT;
        }

        if (ImGui::Selectable("Solid", drawMode == GL_FILL))
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            drawMode = GL_FILL;
        }

        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (drawMode == GL_POINT)
    {
        float pointSize {};
        glGetFloatv(GL_POINT_SIZE, &pointSize);

        if (ImGui::InputFloat("Point Size", &pointSize, 0.5f, 0.5f))
        {
            pointSize = std::max(pointSize, 0.5f);
            glPointSize(pointSize);
        }
    }

    ImGui::Checkbox("Colorful Mode", &colorful);
    ImGui::NewLine();

    size_t numVertices = sphere.getVertexCount();
    float vertexMemoryMB = numVertices * 3 * sizeof(float) / 1000.0f / 1000.0f;
    
    size_t numTriangles = sphere.getTriangleCount();
    float triangleMemoryMB = numTriangles * 3 * sizeof(unsigned int) / 1000.0f / 1000.0f;

    ImGui::Text("Vertices: %d (%.4f MB)", numVertices, vertexMemoryMB);
    ImGui::Text("Triangles: %d (%.4f MB)", numTriangles, triangleMemoryMB);

    ImGui::PopItemWidth();
    ImGui::End();
}

void Application::draw()
{
    // Clear the buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update view and projection matrices
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    shader.use();

    int viewLocation = shader.getLocation("view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    int projectionLocation = shader.getLocation("projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    shader.setFloat("time", clock.getElapsedTime().asSeconds());
    shader.setBool("colorEnabled", colorful);

    int modelLocation = shader.getLocation("model");
    sphere.render(shader, modelLocation);

    if (uiOpen)
    {
        window.pushGLStates();
        window.resetGLStates();

        ImGui::SFML::Render(window);

        window.popGLStates();
    }

    window.display();
}

void Application::updateUIState()
{
    window.setMouseCursorVisible(uiOpen);

    if (uiOpen)
    {
        // Open UI
        sf::Mouse::setPosition(mousePositionUI, window);
    }
    else
    {
        // Close UI
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2i windowCenter(windowSize.x / 2, windowSize.y / 2);

        mousePositionUI = sf::Mouse::getPosition(window);
        sf::Mouse::setPosition(windowCenter, window);
    }
}