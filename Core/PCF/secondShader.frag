#version 330 core
out vec4 FragColor;

in  vec3 FragPos;
in  vec3 Normal;
in  vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float offset;

//1:3x3 grid pattern
//2:8-tap Posisson sample pattern
//3:3x3 guassian filter
//4:random 8 samples  
const int samplesNum=9;
uniform int type;
uniform vec2 samplePos[samplesNum];
uniform float weight[samplesNum];
float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(offset * (1.0 - dot(normal, lightDir)), 0.005);
    

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for(int i=0;i<samplesNum;i++)
    {
        float pcfDepth = texture(shadowMap, projCoords.xy + samplePos[i] * texelSize).r; 
        if(currentDepth - bias > pcfDepth)
        {
            shadow+=weight[i];
        }
    }  

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}