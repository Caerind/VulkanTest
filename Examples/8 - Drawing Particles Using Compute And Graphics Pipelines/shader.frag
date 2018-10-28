#version 450

layout (location = 0) in vec2 geomTexcoord;
layout (location = 1) in vec4 geomColor;

layout (location = 0) out vec4 fragColor;

void main() 
{
	float alpha = 1.0 - dot(geomTexcoord, geomTexcoord);
	if (0.2 > alpha) 
	{
		discard;
	}
	fragColor = vec4(geomColor.rgb, alpha);
}
