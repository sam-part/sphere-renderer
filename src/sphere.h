#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "shader.h"

enum class SphereType
{
    IcoSphere,
    CubeSphere,
    SectorSphere
};

class Sphere
{
public:
    Sphere() = default;

    void init();

    void generateIcosphere();
    void generateCubesphere();
    void generateSectorsphere(unsigned int sectors, unsigned int stacks);

    void subdivide(unsigned int subdivisions);
    unsigned int getSubdivisionLevel() const;

    // Sends data to GPU
    void sendBufferData();

    void render(Shader& shader, int modelLocation);

    void setPosition(glm::vec3 position);
    void setRotationAxis(glm::vec3 axis);
    void setRotationAngle(float angle);

    void setRadius(float radius);
    float getRadius() const;

    SphereType getType() const;

    size_t getVertexCount() const;
    size_t getTriangleCount() const;

    unsigned int getSectors() const;
    unsigned int getStacks() const;

private:
    // Find the midpoint betwen two vertices with constant distsancce from center
    static glm::vec3 findMidpoint(const glm::vec3& a, const glm::vec3& b);

    void addVertex(glm::vec3 vertex);
    void addVertex(float x, float y, float z);
    void addIndices(unsigned int a, unsigned int b, unsigned int c);

    float radius = 1.0f;
    unsigned int subdivisions = 0;

    SphereType type = SphereType::IcoSphere;
    unsigned int sectors {};
    unsigned int stacks {};

    std::vector<float> vertices {};
    std::vector<unsigned int> indices {};

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 rotationAxis {1.0f, 0.0f, 0.0f};
    float rotationAngle {0.0f};
};