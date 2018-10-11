#version 450

layout (location = 0) in vec4 appPosition;
layout (location = 1) in vec3 appNormal;
layout (location = 2) in vec2 appTexcoord;
layout (location = 3) in vec3 appTangent;
layout (location = 4) in vec3 appBitangent;

layout (set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (location = 0) out vec3 vertPosition;
layout (location = 1) out vec2 vertTexcoord;
layout (location = 2) out vec3 vertNormal;
layout (location = 3) out vec3 vertTangent;
layout (location = 4) out vec3 vertBitangent;

void main() 
{
	vec4 position = modelViewMatrix * appPosition;
	gl_Position = projectionMatrix * position;
	mat3 MVMatrix = mat3(modelViewMatrix);

	vertPosition = position.xyz;
	vertTexcoord = appTexcoord;
	vertNormal = MVMatrix * appNormal;
	vertTangent = MVMatrix * appTangent;
	vertBitangent = MVMatrix * appBitangent;
}