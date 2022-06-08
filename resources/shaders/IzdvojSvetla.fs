#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;



in vec2 TexCoords;

uniform sampler2D image;

void main()
{
vec3 result = texture(image,TexCoords).rgb ;
vec3 unit = vec3(1.0,1.0,1.0);
float dot = dot(unit,result);

    if(dot>1.0){
        BrightColor = vec4(result,1.0);
    }else{
        BrightColor = vec4(1.0,1.0,1.0,1.0);
    }

    FragColor = vec4(result,1.0);
}