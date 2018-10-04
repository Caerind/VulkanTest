#version 450

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (push_constant) uniform LightParameters
{
	vec4 position;
} light;

layout (location = 0) out vec4 fragColor;

void main() 
{
	float shinniness = 60.0;

	vec3 normalVector = normalize(vertNormal);
	vec3 lightVector = normalize(light.position.xyz - vertPosition);

	float diffuse = max(0.0, dot(normalVector, lightVector));
	float specular = 0.0;

	if (diffuse > 0.0)
	{
		vec3 viewVector = normalize(vec3(0.0) - vertPosition.xyz);
		vec3 halfVector = normalize(viewVector + lightVector);
		specular = pow(dot(halfVector, normalVector), shinniness);
	}

	fragColor = vec4(diffuse + specular + 0.1);
}
