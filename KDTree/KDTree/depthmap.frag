#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

//writes the distance to the light into the depth buffer
void main()
{
    //calculate distance to light
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // transform to 0 - 1 Range by dividing through far plane
    lightDistance = lightDistance / far_plane;
    
    // write to depth buffer
    gl_FragDepth = lightDistance;
}