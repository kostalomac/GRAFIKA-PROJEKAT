#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform vec3 ColorOfCube;


void main()
{


    BrightColor = vec4(ColorOfCube*9, 1.0);
    FragColor = vec4(ColorOfCube,1.0);

}