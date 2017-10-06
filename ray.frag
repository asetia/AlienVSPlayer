#version 330 core
out vec4 color;

uniform vec4 desiredColor;

void main()
{    
	color = desiredColor;
}