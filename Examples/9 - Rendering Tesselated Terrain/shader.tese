#version 450

layout (triangles, fractional_even_spacing, cw) in;
layout (location = 0) in vec2 tescTexcoord[];

layout (set = 0, binding = 1) uniform sampler2D heightMap;

layout (location = 0) out float teseHeight;

void main() 
{
	vec4 position = gl_in[0].gl_Position * gl_TessCoord.x +
					gl_in[1].gl_Position * gl_TessCoord.y +
					gl_in[2].gl_Position * gl_TessCoord.z;

	vec2 texcoord = tescTexcoord[0] * gl_TessCoord.x +
					tescTexcoord[1] * gl_TessCoord.y +
					tescTexcoord[2] * gl_TessCoord.z;

	float height = texture(heightMap, texcoord).x;
	position.y += height;

	gl_Position = position;
	teseHeight = height;
}
