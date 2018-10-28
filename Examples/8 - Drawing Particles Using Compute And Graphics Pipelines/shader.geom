#version 450

layout (points) in;
layout (location = 0) in vec4 vertColor[];

layout (set = 0, binding = 0) uniform UniformBuffer {
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (triangle_strip, max_vertices = 4) out;
layout (location = 0) out vec2 geomTexcoord;
layout (location = 1) out vec4 geomColor;

const float SIZE = 0.05;

void main() 
{
	vec4 position = gl_in[0].gl_Position;
  
	gl_Position = projectionMatrix * (gl_in[0].gl_Position + vec4(-SIZE, SIZE, 0.0, 0.0));
	geomTexcoord = vec2(-1.0, 1.0);
	geomColor = vertColor[0];
	EmitVertex();
  
	gl_Position = projectionMatrix * (gl_in[0].gl_Position + vec4(-SIZE, -SIZE, 0.0, 0.0));
	geomTexcoord = vec2(-1.0, -1.0);
	geomColor = vertColor[0];
	EmitVertex();
  
	gl_Position = projectionMatrix * (gl_in[0].gl_Position + vec4(SIZE, SIZE, 0.0, 0.0));
	geomTexcoord = vec2(1.0, 1.0);
	geomColor = vertColor[0];
	EmitVertex();
  
	gl_Position = projectionMatrix * (gl_in[0].gl_Position + vec4(SIZE, -SIZE, 0.0, 0.0));
	geomTexcoord = vec2(1.0, -1.0);
	geomColor = vertColor[0];
	EmitVertex();

	EndPrimitive();
}
