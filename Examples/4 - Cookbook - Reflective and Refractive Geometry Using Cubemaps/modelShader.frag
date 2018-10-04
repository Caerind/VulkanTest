#version 450

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;

layout (set = 0, binding = 1) uniform samplerCube cubemap;

layout (push_constant) uniform LightParameters
{
	vec4 position;
} camera;

layout (location = 0) out vec4 fragColor;

void main() 
{
	vec3 viewVector = vertPosition - camera.position.xyz;
	float angle = smoothstep(0.3, 0.7, dot(normalize(-viewVector), vertNormal));
	
	vec3 reflectVector = reflect(viewVector, vertNormal);
	vec4 reflectColor = texture(cubemap, reflectVector);

	vec3 refractVector = refract(viewVector, vertNormal, 0.3);
	vec4 refractColor = texture(cubemap, refractVector);

	fragColor = mix(reflectColor, refractColor, angle);
}
