#version 450

layout (location = 0) in vec4 appPosition;

void main() 
{
	gl_Position = appPosition;
}
