#version 330 core
layout (location = 0) out vec4 FragColor;


in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform sampler2D screenTexture;
uniform bool ukljuciEDGEDETECTION;
uniform bool ukljuciBLUR;
uniform bool bloom;
uniform float exposure;
uniform bool hdr;
const float offset = 1.0/300.0;
uniform bool horizontal;





vec3 FUNKCIJA(vec3 col)
{
 vec2 offsets[9] = vec2[](
            vec2(-offset,offset),
            vec2(0.0f,offset),
            vec2(offset,offset),
            vec2(-offset,0.0f),
            vec2(0.0f,0.0f),
            vec2(0.0f,offset),
            vec2(-offset,-offset),
            vec2(0.0f,-offset),
            vec2(offset,-offset)
        );

        float kernel[9] = float[](
            -1,-1,-1,
            -1,9,-1,
            -1,-1,-1
        );

        vec3 sampleTex[9];
        for(int i = 0 ; i < 9 ;++i)
        {
        sampleTex[i] = vec3(texture(screenTexture,TexCoords.st + offsets[i]));
        }





        for(int i = 0 ; i < 9 ;++i)
        {
        col+=sampleTex[i]*kernel[i];
        }



return col;
}








void main()
{
    float gamma = 2.2;
    vec3 col = texture(scene,TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur,TexCoords).rgb;

    if(bloom){
    col+=bloomColor;
    }

    if(ukljuciEDGEDETECTION){
     col = FUNKCIJA(col);
    }

    if(hdr)
    {
        col =vec3(1.0) - exp(-col*exposure);
      //  col = pow(col,vec3(1.0/gamma));
    }


    FragColor = vec4(col,1.0);
}