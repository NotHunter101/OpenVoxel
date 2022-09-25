#version 410 core

layout (location = 0) in uint material;

uniform uint MAXSIZE_X;
uniform uint MAXSIZE_Y;
uniform uint MAXSIZE_Z;

uvec3 indexToPos(uint index)
{
	return uvec3(
		index % MAXSIZE_X,
		uint( trunc( float(index) / float(MAXSIZE_X) ) ) % MAXSIZE_Y,
		uint( trunc( float(index) / float(MAXSIZE_X * MAXSIZE_Y) ) )
	);
}

void main()
{
	uvec3 indexPos = indexToPos(uint(gl_VertexID));
	vec3 centerOffset = vec3(float(MAXSIZE_X) * 0.5f, float(MAXSIZE_Y) * 0.5f, float(MAXSIZE_Z) * 0.5f);
	vec3 voxelPos = vec3(indexPos) - centerOffset;

	gl_Position = vec4(voxelPos, 1.0f);
}