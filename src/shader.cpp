#include "shader.h"

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>

unsigned int compileShader(const char* source, GLenum type)
{
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

		std::cout << "Shader compilation failed (Type: " + std::to_string(type) + ")\n";
		std::cout << infoLog << "\n";
	}

	return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::ifstream vertexInput(vertexPath);
	std::ifstream fragmentInput(fragmentPath);

	if (!vertexInput.is_open())
	{
		std::cout << "Could not open vertex shader \"" << vertexPath << "\"\n";
	}

	if (!fragmentInput.is_open())
	{
		std::cout << "Could not open fragment shader \"" << fragmentPath << "\"\n";
	}

	std::stringstream vertexStream;
	vertexStream << vertexInput.rdbuf();
	std::string vertexCodeStr = vertexStream.str();
	const char* vertexCode = vertexCodeStr.c_str();

	std::stringstream fragmentStream;
	fragmentStream << fragmentInput.rdbuf();
	std::string fragmentCodeStr = fragmentStream.str();
	const char* fragmentCode = fragmentCodeStr.c_str();

	unsigned int vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
	unsigned int fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	int success {};
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);

		std::cout << "Failed to link shader program\n";
		std::cout << infoLog << "\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

unsigned int Shader::getID() const
{
	return id;
}

int Shader::getLocation(const std::string& name)
{
	return glGetUniformLocation(id, name.c_str());
}
