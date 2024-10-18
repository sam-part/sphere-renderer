#version 330 core

out vec4 outColor;
in vec2 texCoord;
in vec3 position;

uniform bool colorEnabled;
uniform float time;

void main()
{	
	float r = 1.0;
	float g = 1.0;
	float b = 1.0;

	if(colorEnabled)
	{
		r = 0.2 + 0.6 * abs(sin(position.x + position.z + time)) + 0.1 * cos(time);
		g = 0.1 + 0.6 * abs(cos(position.x + position.y + position.z - 0.337 * time)) + 0.1 * sin(1.3217 * time);
		b = 0.2 + 0.6 * abs(cos(position.x * position.y + position.y * position.z + 0.41831 * time)) + 0.1 * sin(1.7 * time);
	}

	outColor = vec4(r, g, b, 1.0);
}