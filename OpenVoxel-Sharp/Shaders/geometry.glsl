#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in uvec3 lightColor[1];
in uint materialAndFaces[1];

uniform mat4 MVP_MATRIX;
uniform mat4 FACE_ROT_MATRICES[6];

out vec3 fragColor;

void main() 
{
    fragColor = (gl_in[0].gl_Position.xyz + vec3(75.0f)) / vec3(150.0f);

    vec3 quad[4] = vec3[4](
        vec3(-0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f, -0.5f),
        vec3(0.5f, 0.5f, 0.5f),
        vec3(0.5f, 0.5f, -0.5f)
    );

    for (int i = 0; i < 6; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            vec4 rotatedQuadPos = FACE_ROT_MATRICES[i] * vec4(quad[j], 0.0f);
            vec4 newVertex = gl_in[0].gl_Position + rotatedQuadPos;
            gl_Position = MVP_MATRIX * newVertex;
            EmitVertex();
        }
    
        EndPrimitive();
    }
}  