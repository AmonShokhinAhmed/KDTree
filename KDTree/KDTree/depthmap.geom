#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
//foreach cubemap face
    for(int face = 0; face < 6; ++face)
    {
        //set current layer so we render to correct face
        gl_Layer = face;
        //for each vertex in current tri
        for(int i = 0; i < 3; ++i) 
        {
            //set frag position
            FragPos = gl_in[i].gl_Position;
            //transform position by current transform matrix for face
            gl_Position = shadowMatrices[face] * FragPos;
            //pass out vertex
            EmitVertex();
        }    
        //finish tri for current face
        EndPrimitive();
    }
} 