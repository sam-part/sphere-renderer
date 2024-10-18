#pragma once

#include <string>
#include <GL/glew.h>

class Shader
{
public:
	Shader() = default;
	Shader(std::string vertexPath, std::string fragmentPath);

	void use();

	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);

	unsigned int getID() const;
	int getLocation(const std::string& name);

private:
	unsigned int id {};
};