#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in uvec3 lightColor[1];
in uint materialAndFaces[1];

uniform mat4 MVP_MATRIX;

mat3 rotateXZ(float x, float z) 
{
    mat3 rotationX;
    rotationX[0][0] = 1;
    rotationX[1][1] = cos(x);
    rotationX[2][1] = -sin(x);
    rotationX[1][2] = sin(x);
    rotationX[2][2] = cos(x);

    mat3 rotationZ;
    rotationZ[0][0] = cos(z);
    rotationZ[1][0] = -sin(z);
    rotationZ[0][1] = sin(z);
    rotationZ[1][1] = cos(z);
    rotationZ[2][2] = 1;

    return rotationX * rotationZ;
}

void main() 
{
    vec3 rotations[6] = vec3[6](
        vec3(0, 0, 0),
        vec3(3.1415, 0, 0),
        vec3(0, 0, 1.5708),
        vec3(0, 0, -1.5708),
        vec3(-1.5708, 0, 0),
        vec3(1.5708, 0, 0)
    );

    vec3 quad[4] = vec3[4](
        vec3(-0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f, -0.5f),
        vec3(0.5f, 0.5f, 0.5f),
        vec3(0.5f, 0.5f, -0.5f)
    );

    for (int i = 0; i < 6; i++) 
    {
        vec3 rotationEuler = rotations[i];
        mat3 rotationMatrix = rotateXZ(rotationEuler.x, rotationEuler.z);

        for (int j = 0; j < 4; j++) 
        {
            vec3 rotatedQuadPoint = rotationMatrix * quad[j];
            vec4 newVertex = gl_in[0].gl_Position + vec4(rotatedQuadPoint, 0.0f);
            gl_Position = MVP_MATRIX * newVertex;
            EmitVertex();
        }
    
        EndPrimitive();
    }
}  