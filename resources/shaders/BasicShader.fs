#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;


void main()
{
vec3 temp = vec3(texture(texture1, TexCoords).rgb);

if ( temp.g < 0.2 &&  temp.r < 0.2 && temp.b < 0.2 ){
 discard;
}


FragColor = vec4(vec3(texture(texture1, TexCoords).rgb),1.0);
}