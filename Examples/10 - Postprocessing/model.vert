#version 450

layout (location = 0) in vec4 appPosition;
layout (location = 1) in vec3 appNormal;

layout (set = 0, binding = 0) uniform UniformBuffer {
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (location = 0) out vec3 vertPosition;
layout (location = 1) out vec3 vertNormal;

void main() 
{
	vertPosition = appPosition.xyz;
	vertNormal = appNormal;
  
	gl_Position = projectionMatrix * modelViewMatrix * appPosition;
}
