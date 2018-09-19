#version 450

layout(location = 0) in float vertexColor;

layout(location = 0) out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(vertexColor);
}