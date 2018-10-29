#version 450

layout (location = 0) in vec2 vertTexcoord[];

layout (set = 0, binding = 0) uniform UniformBuffer {
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout (set = 0, binding = 1) uniform sampler2D imageSampler;

layout (vertices = 3) out;
layout (location = 0) out vec2 tescTexcoord[];

void main() 
{
	if (gl_InvocationID == 0) 
	{
		float distances[3];
		float factors[3];

		for (int i = 0; i < 3; ++i) 
		{
			float height = texture(imageSampler, vertTexcoord[i]).x;
			vec4 position = modelViewMatrix * (gl_in[i].gl_Position + vec4(0.0, height, 0.0, 0.0));
			distances[i] = dot(position, position);
		}

		factors[0] = min(distances[1], distances[2]);
		factors[1] = min(distances[2], distances[0]);
		factors[2] = min(distances[0], distances[1]);

		gl_TessLevelInner[0] = max(1.0, 20.0 - factors[0]);
		gl_TessLevelOuter[0] = max(1.0, 20.0 - factors[0]);
		gl_TessLevelOuter[1] = max(1.0, 20.0 - factors[1]);
		gl_TessLevelOuter[2] = max(1.0, 20.0 - factors[2]);
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tescTexcoord[gl_InvocationID] = vertTexcoord[gl_InvocationID];
}
