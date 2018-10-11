#version 450

layout (location = 0) in vec4 appPosition;
layout (location = 1) in vec3 appNormal;

layout (set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (location = 0) out vec3 vertPosition;
layout (location = 1) out vec3 vertNormal;

void main() 
{
	vec4 position = modelViewMatrix * appPosition;

	vertPosition = position.xyz;
	vertNormal = mat3(modelViewMatrix) * appNormal;
	gl_Position = projectionMatrix * position;
}