#version 410 core

layout (location = 0) in uint2 voxel;
out uvec3 lightColor;
out uint materialAndFaces;

struct UnpackedVoxel 
{
	uvec3 position;
	bool faces[6];
	uvec3 lightColor;
	uint material;
};

uint unpackValue(uint packed, int valueBitCount, int shiftAmount) 
{
	uint64_t maxValue = uint64_t(pow(2, 8) - 1);
	uint64_t outputValue = (voxel & (maxValue << 8)) >> 8;
	return uint(outputValue);
}

UnpackedVoxel unpackVoxel() 
{
	UnpackedVoxel unpacked;
	unpacked.position = uvec3(
		unpackValue(8, 0),
		unpackValue(8, 8), 
		unpackValue(8, 16));
	return unpacked;
}

void main()
{
	UnpackedVoxel unpacked = unpackVoxel();
	vec3 voxelPos = vec3(unpacked.position) - vec3(128.0f);
	
	if (unpacked.position.y == 128)
	{
		voxelPos = vec3(0.0f);
	}

	gl_Position = vec4(voxelPos, 1.0f);

	lightColor = unpacked.lightColor;
	materialAndFaces = (int(unpacked.faces[5]) << 13) | 
	(int(unpacked.faces[4]) << 12) | 
	(int(unpacked.faces[3]) << 11) | 
	(int(unpacked.faces[2]) << 10) | 
	(int(unpacked.faces[1]) << 9) |
	(int(unpacked.faces[0]) << 8) | 
	(unpacked.material);
}