#version 410 core

layout (location = 0) in uint material;

uniform uint MAXSIZE_X;
uniform uint MAXSIZE_Y;
uniform uint MAXSIZE_Z;

vec3 indexToPos(uint index)
{
	return vec3(
		index % MAXSIZE_X,
		index / MAXSIZE_X % MAXSIZE_Y,
		index / (MAXSIZE_X * MAXSIZE_Y)
	);
}

void main()
{
	vec3 indexPos = indexToPos(uint(gl_VertexID));
	vec3 centerOffset = vec3(float(MAXSIZE_X) * 0.5f, float(MAXSIZE_Y) * 0.5f, float(MAXSIZE_Z) * 0.5f);
	vec3 voxelPos = indexPos - centerOffset;

	gl_Position = vec4(voxelPos, 1.0f);
}