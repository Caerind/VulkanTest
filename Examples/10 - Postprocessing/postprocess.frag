#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputAttachment;

layout (push_constant) uniform TimeParameters {
	float time;
} pushConstant;

layout (location = 0) out vec4 fragColor;

void main() 
{
	vec4 color = subpassLoad(inputAttachment);
	float grey = dot(color.rgb, vec3(0.2, 0.7, 0.1));
	float interpolator = clamp(sin(pushConstant.time) + 0.5, 0.0, 1.0);
	fragColor = mix(grey * vec4(1.5, 1.0, 0.5, 1.0), color, interpolator);
}
