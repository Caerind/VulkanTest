#version 450

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 vertTexcoord;
layout (location = 2) in vec3 vertNormal;
layout (location = 3) in vec3 vertTangent;
layout (location = 4) in vec3 vertBitangent;

layout (set = 0, binding = 1) uniform sampler2D imageSampler;

layout (push_constant) uniform LightParameters
{
	vec4 position;
} light;

layout (location = 0) out vec4 fragColor;

void main() 
{
	float shinniness = 60.0;

	vec3 normal = 2 * texture(imageSampler, vertTexcoord).rgb - 1.0;
	vec3 normalVector = normalize(mat3(vertTangent, vertBitangent, vertNormal) * normal);
	vec3 lightVector = normalize(light.position.xyz - vertPosition);
	
	float diffuse = max(0.0, dot(normalVector, lightVector)) * max(0.0, dot(vertNormal, lightVector));

	float specular = 0.0;
	if (diffuse > 0.0)
	{
		vec3 viewVector = normalize(vec3(0.0) - vertPosition.xyz);
		vec3 halfVector = normalize(viewVector + lightVector);
		specular = pow(dot(halfVector, normalVector), shinniness);
	}

	fragColor = vec4(diffuse + specular + 0.1);
}
