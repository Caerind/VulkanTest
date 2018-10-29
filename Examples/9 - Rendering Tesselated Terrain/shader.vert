#version 450

layout (location = 0) in vec4 appPosition;
layout (location = 1) in vec2 appTexcoord;

layout (location = 0) out vec2 vertTexcoord;

void main() 
{
	gl_Position = appPosition;
	vertTexcoord = appTexcoord;
}
