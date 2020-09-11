#version 330 core
out vec4 FragColor;

in  vec3 FragPos;
in  vec3 Normal;
in  vec2 TexCoords;

uniform sampler2D diffuseTexture;

uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D shadowMap3;
uniform sampler2D shadowMap4;

uniform vec3 lightPos;
uniform vec3 viewPos;


uniform vec3 Front;
uniform vec4 far_d;
uniform mat4 lightSpaceMatrix[4];

float ShadowCompute()
{
    //计算该点的深度，方法为F-V向量在Front上的投影
    float d=length(dot((FragPos-viewPos),normalize(Front)));
      
    int index=3;

    if(d < far_d.x) 
        index = 0;
    else if(d < far_d.y) 
        index = 1;
    else if(d < far_d.z) 
        index = 2;
    
    vec4 FragPosLightSpace = lightSpaceMatrix[index] * vec4(FragPos, 1.0);

    vec3 projCoords = FragPosLightSpace.xyz;// / FragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth=0.0;

    //根据不同的深度采样不同的深度图
    if(index==0)
        closestDepth = texture(shadowMap1, projCoords.xy).r;
    else if(index==1)
        closestDepth = texture(shadowMap2, projCoords.xy).r;
    else if(index==2)
        closestDepth = texture(shadowMap3, projCoords.xy).r;
    else
        closestDepth = texture(shadowMap4, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 normal = normalize(Normal);

    //这个偏移存在问题，自己看着修改吧
    float bias = max((1.0 - dot(normal, lightDir)), 0.005)*0.1;

    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
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
    float shadow = ShadowCompute();                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}