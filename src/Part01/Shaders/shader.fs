#version 330 core

in vec3 ourColour;

out vec4 FragColour;

void main()
{
	FragColour = vec4(ourColour, 1.0f);
}
