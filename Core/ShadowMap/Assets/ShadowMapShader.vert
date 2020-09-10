#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightPos;
uniform bool slope_scale_bias;
uniform float offset;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    vec3 LightDir = normalize(lightPos-FragPos);
    if(slope_scale_bias==true)
    {
        FragPos=FragPos-LightDir*max(offset * (1.0 - abs(dot(Normal, LightDir))), 0.005);
    }

    
            
    gl_Position = lightSpaceMatrix * vec4(FragPos, 1.0);
}