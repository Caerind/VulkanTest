#version 450

layout (location = 0) in vec4 appPosition;
layout (location = 1) in vec4 appColor;

layout (set = 0, binding = 0) uniform UniformBuffer {
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (location = 0) out vec4 vertColor;

void main() 
{
	gl_Position = modelViewMatrix * appPosition;
	vertColor = appColor;
}
