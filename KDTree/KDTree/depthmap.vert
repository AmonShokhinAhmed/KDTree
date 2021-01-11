#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;


//simply pass position with model transformation forward to fragment shader
void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}