#version 450

layout(location = 0) in vec4 appPosition;
layout(location = 1) in vec3 appNormal;

layout(set = 0, binding = 0) uniform UniformBuffer
{
	mat4 ModelViewMatrix;
	mat4 ProjectionMatrix;
};

layout(location = 0) out float vertexColor;

void main()
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * appPosition;
	vec3 normal = mat3(ModelViewMatrix) * appNormal;
	vertexColor = max(0.0, dot(normal, vec3(0.58, 0.58, 0.58))) + 0.1;
}
