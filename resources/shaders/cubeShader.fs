#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;



struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};


struct Spotlight {
    vec3 position;
    vec3 lightDir;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};


struct DirectionalLight {
    vec3 direction;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};



struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;



    float shininess;
};
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform PointLight pointLight;
uniform DirectionalLight directionalLight;
uniform Spotlight spotlight;

uniform Material material;

uniform vec3 viewPosition;
// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient+diffuse+specular);;
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient+diffuse+specular);
}



vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    //SPECULAR
    //vec3 viewDir =  normalize(light.position - fragPos);




    float spec = pow(max(dot(lightDir, normal), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float temp = max(dot(normalize(lightDir),normalize(-light.lightDir)),0);
    if(temp>0.995)
    {
       return (ambient+diffuse+specular);
    }
    if(temp>0.98)
    {
       return (ambient+diffuse+specular)*(temp - 0.98 )* 66.6666666667;
    }
}




void main()
{
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result2 = CalcDirLight(directionalLight, Normal, FragPos,  viewDir);
    vec3 result = CalcPointLight(pointLight, Normal, FragPos, viewDir);
    vec3 result3 = CalcSpotLight(spotlight, Normal, FragPos, spotlight.lightDir);
    vec3 col = result + result2 + result3;
    FragColor = vec4(col, 1.0);

        vec3 unitVec = vec3(0.212,0.71,0.072);
        if(dot(unitVec,col)>1.0)
        {
        BrightColor = vec4(col, 1.0);

        }else{
        BrightColor = vec4(0.0,0.0,0.0, 1.0);

        }
}