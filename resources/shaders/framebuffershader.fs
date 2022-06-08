#version 330 core

in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform sampler2D screenTexture;
uniform bool ukljuciEDGEDETECTION;
uniform bool ukljuciBLUR;
uniform float exposure = 1;
uniform bool hdr;
const float offset = 1.0/300.0;
uniform bool horizontal;

vec3 BLUR()
{


    float weight[5] = float[](0.22,0.19,0.12,0.05,0.016);

    vec2 tex_offset = 1.0/ textureSize(screenTexture,0);
    vec3 result = texture(screenTexture,TexCoords).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(screenTexture,TexCoords + vec2(tex_offset.x * i,0.0)).rgb * weight[i];
            result += texture(screenTexture,TexCoords - vec2(tex_offset.x * i,0.0)).rgb * weight[i];
        }

    }else{
        for(int i = 1; i < 5; ++i)
        {
            result += texture(screenTexture,TexCoords + vec2(0.0,tex_offset.y * i)).rgb * weight[i];
            result += texture(screenTexture,TexCoords - vec2(0.0,tex_offset.y * i)).rgb * weight[i];
        }
    }
    return result;
}


vec3 FUNKCIJA()
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



        vec3 col = vec3(0.0);


        for(int i = 0 ; i < 9 ;++i)
        {
        col+=sampleTex[i]*kernel[i];
        }



return col;
}



void main()
{



    const float gamma = 2.2f;
    vec3 col = texture(screenTexture,TexCoords).rgb;

    if(ukljuciEDGEDETECTION==true)
    {
        FragColor = vec4(FUNKCIJA(), 1.0);
        return;
    }


    if(hdr)
    {
        vec3 result =vec3(1.0) -exp(-col*exposure);
        result = pow(result,vec3(1.0/gamma));
        FragColor = vec4(result, 1.0);
        return;
    }

    if(ukljuciBLUR)
    {
        FragColor = vec4(BLUR(), 1.0);
        return;
    }

    FragColor = vec4(col, 1.0);



}

