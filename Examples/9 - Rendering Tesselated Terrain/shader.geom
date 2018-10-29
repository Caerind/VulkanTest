#version 450

layout (triangles) in;
layout (location = 0) in float teseHeight[];

layout (set = 0, binding = 0) uniform UniformBuffer {
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (triangle_strip, max_vertices = 3) out;
layout (location = 0) out vec3  geomNormal;
layout (location = 1) out float geomHeight;

void main() 
{
	vec3 v0v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 v0v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = normalize(cross(v0v1, v0v2));
  
	for (int vertex = 0; vertex < 3; ++vertex) 
	{
		gl_Position = projectionMatrix * modelViewMatrix * gl_in[vertex].gl_Position;
		geomHeight = teseHeight[vertex];
		geomNormal = normal;
		EmitVertex();
	}
  
	EndPrimitive();
}
