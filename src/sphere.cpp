#include "sphere.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <numbers>
#include <cmath>

constexpr float pi = std::numbers::pi;

void Sphere::init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void Sphere::generateIcosphere()
{
	// Source: https://en.wikipedia.org/wiki/Regular_icosahedron#Construction
	const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
	const float scale = 1.0f / std::sqrt(phi * phi + 1.0f);
	const float scaledPhi = phi * scale;

	vertices.clear();
	vertices.reserve(12 * 3);

	indices.clear();
	indices.reserve(20 * 3);

	addVertex(-scale, scaledPhi, 0);
	addVertex(scale, scaledPhi, 0);
	addVertex(-scale, -scaledPhi, 0);
	addVertex(scale, -scaledPhi, 0);
	addVertex(0, -scale, scaledPhi);
	addVertex(0, scale, scaledPhi);
	addVertex(0, -scale, -scaledPhi);
	addVertex(0, scale, -scaledPhi);
	addVertex(scaledPhi, 0, -scale);
	addVertex(scaledPhi, 0, scale);
	addVertex(-scaledPhi, 0, -scale);
	addVertex(-scaledPhi, 0, scale);

	addIndices(0, 11, 5);
	addIndices(0, 5, 1);
	addIndices(0, 1, 7);
	addIndices(0, 7, 10);
	addIndices(0, 10, 11);
	addIndices(1, 5, 9);
	addIndices(5, 11, 4);
	addIndices(11, 10, 2);
	addIndices(10, 7, 6);
	addIndices(7, 1, 8);
	addIndices(3, 9, 4);
	addIndices(3, 4, 2);
	addIndices(3, 2, 6);
	addIndices(3, 6, 8);
	addIndices(3, 8, 9);
	addIndices(4, 9, 5);
	addIndices(2, 4, 11);
	addIndices(6, 2, 10);
	addIndices(8, 6, 7);
	addIndices(9, 8, 1);

	subdivisions = 0;
	type = SphereType::IcoSphere;
}

void Sphere::generateCubesphere()
{
	// Ensure all points have a constant distance of 1 to the origin
	float position = 1.0f / std::sqrt(3.0f);

	vertices = {
		-position, -position, -position,
		 position, -position, -position,
		 position,  position, -position,
		-position,  position, -position,
		-position, -position,  position,
		 position, -position,  position,
		 position,  position,  position,
		-position,  position,  position
	};

	indices = {
		0, 1, 3, 3, 1, 2,
		1, 5, 2, 2, 5, 6,
		5, 4, 6, 6, 4, 7,
		4, 0, 7, 7, 0, 3,
		3, 2, 7, 7, 2, 6,
		4, 5, 0, 0, 5, 1
	};

	subdivisions = 0;
	type = SphereType::CubeSphere;
}

void Sphere::generateSectorsphere(unsigned int sectors, unsigned int stacks)
{
	vertices.clear();
	indices.clear();

	unsigned int numVertices = (sectors + 1) * (stacks + 1);

	for (int i = 0; i <= stacks; i++)
	{
		// Stack angles range from pi/2 to -pi/2
		const float phi = 0.5f * pi - pi * (static_cast<float>(i) / static_cast<float>(stacks));

		unsigned int stackIndex = i * (sectors + 1);
		unsigned int nextStackIndex = (i + 1) * (sectors + 1);

		for (int j = 0; j <= sectors; j++)
		{
			// Sector angles range from 0 to 2pi 
			const float theta = 2.0f * pi * (static_cast<float>(j) / static_cast<float>(sectors));

			glm::vec3 vertex {};
			vertex.x = std::cos(phi) * std::cos(theta);
			vertex.y = std::sin(phi);
			vertex.z = std::cos(phi) * std::sin(theta);

			addVertex(vertex);

			if (i != 0 && nextStackIndex < numVertices)
				addIndices(stackIndex, stackIndex + 1, nextStackIndex);

			if (i != stacks - 1 && nextStackIndex + 1 < numVertices)
				addIndices(nextStackIndex, nextStackIndex + 1, stackIndex + 1);

			stackIndex++;
			nextStackIndex++;
		}
	}

	this->sectors = sectors;
	this->stacks = stacks;

	subdivisions = 0;
	type = SphereType::SectorSphere;
}

void Sphere::subdivide(unsigned int newSubdivisions)
{
	if (newSubdivisions == subdivisions)
		return;

	if (newSubdivisions < subdivisions)
	{
		if (type == SphereType::IcoSphere)
			generateIcosphere();
		else if (type == SphereType::CubeSphere)
			generateCubesphere();
		else if (type == SphereType::SectorSphere)
			generateSectorsphere(sectors, stacks);
	}

	for (unsigned int i = subdivisions; i < newSubdivisions; i++)
	{
		std::vector<float> oldVertices = vertices;
		std::vector<unsigned int> oldIndices = indices;

		vertices.clear();
		indices.clear();

		int index = 0;

		// Loop through each triangle (every 3 indices)
		for (int j = 0; j < oldIndices.size(); j += 3)
		{
			size_t v1Pos = oldIndices[j] * 3;
			size_t v2Pos = oldIndices[j + 1] * 3;
			size_t v3Pos = oldIndices[j + 2] * 3;

			glm::vec3 v1 {oldVertices[v1Pos], oldVertices[v1Pos + 1], oldVertices[v1Pos + 2]};
			glm::vec3 v2 {oldVertices[v2Pos], oldVertices[v2Pos + 1], oldVertices[v2Pos + 2]};
			glm::vec3 v3 {oldVertices[v3Pos], oldVertices[v3Pos + 1], oldVertices[v3Pos + 2]};

			glm::vec3 mid1 = findMidpoint(v1, v2);
			glm::vec3 mid2 = findMidpoint(v2, v3);
			glm::vec3 mid3 = findMidpoint(v3, v1);

			addVertex(mid1); // 0
			addVertex(mid2); // 1
			addVertex(mid3); // 2
			addVertex(v1);   // 3
			addVertex(v2);   // 4
			addVertex(v3);   // 5

			addIndices(index, index + 2, index + 1);
			addIndices(index + 1, index + 2, index + 5);
			addIndices(index, index + 1, index + 4);
			addIndices(index, index + 2, index + 3);

			index += 6;
		}
	}

	subdivisions = newSubdivisions;
}

unsigned int Sphere::getSubdivisionLevel() const
{
	return subdivisions;
}

void Sphere::sendBufferData()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(sizeof(vertices[0]) * vertices.size()),
		vertices.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(sizeof(indices[0]) * indices.size()),
		indices.data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
}

void Sphere::render(Shader& shader, int modelLocation)
{
	glBindVertexArray(VAO);

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotationAngle, rotationAxis);
	model = glm::scale(model, glm::vec3(radius, radius, radius));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES,
		static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_INT,
		nullptr);

	glBindVertexArray(0);
}

void Sphere::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Sphere::setRotationAxis(glm::vec3 axis)
{
	rotationAxis = axis;
}

void Sphere::setRotationAngle(float angle)
{
	rotationAngle = angle;
}

void Sphere::setRadius(float radius)
{
	this->radius = radius;
}

float Sphere::getRadius() const
{
	return radius;
}

SphereType Sphere::getType() const
{
	return type;
}

size_t Sphere::getVertexCount() const
{
	return vertices.size() / 3;
}

size_t Sphere::getTriangleCount() const
{
	return indices.size() / 3;
}

unsigned int Sphere::getSectors() const
{
	return sectors;
}

unsigned int Sphere::getStacks() const
{
	return stacks;
}

glm::vec3 Sphere::findMidpoint(const glm::vec3& a, const glm::vec3& b)
{
	glm::vec3 vertex {};
	vertex.x = (a.x + b.x) * 0.5f;
	vertex.y = (a.y + b.y) * 0.5f;
	vertex.z = (a.z + b.z) * 0.5f;

	// Scale by distance from center
	float scale = 1.0f / std::sqrt (vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z);

	return vertex * scale;
}

void Sphere::addVertex(glm::vec3 vertex)
{
	vertices.push_back(vertex.x);
	vertices.push_back(vertex.y);
	vertices.push_back(vertex.z);
}

void Sphere::addVertex(float x, float y, float z)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}

void Sphere::addIndices(unsigned int a, unsigned int b, unsigned int c)
{
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
}
