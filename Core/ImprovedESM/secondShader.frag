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

uniform float c;
float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float currentDepth = projCoords.z;

    //http://www.klayge.org/2013/10/07/%E5%88%87%E6%8D%A2%E5%88%B0esm/ 
    
    float d0 = texture(shadowMap, projCoords.xy).r;
    float tempVar=0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    tempVar+=0.0625*exp(c*(texture(shadowMap, projCoords.xy+vec2(-1,1)*texelSize).r-d0));
    tempVar+=0.0625*exp(c*(texture(shadowMap, projCoords.xy+vec2(-1,-1)*texelSize).r-d0));
    tempVar+=0.0625*exp(c*(texture(shadowMap, projCoords.xy+vec2(1,1)*texelSize).r-d0));
    tempVar+=0.0625*exp(c*(texture(shadowMap, projCoords.xy+vec2(1,-1)*texelSize).r-d0));

    tempVar+=0.125*exp(c*(texture(shadowMap, projCoords.xy+vec2(0,1)*texelSize).r-d0));
    tempVar+=0.125*exp(c*(texture(shadowMap, projCoords.xy+vec2(0,-1)*texelSize).r-d0));
    tempVar+=0.125*exp(c*(texture(shadowMap, projCoords.xy+vec2(1,0)*texelSize).r-d0));
    tempVar+=0.125*exp(c*(texture(shadowMap, projCoords.xy+vec2(-1,0)*texelSize).r-d0));

    float cdz=c*d0+log(0.25+tempVar)-c*currentDepth;
    
    if(projCoords.z > 1.0||projCoords.x>1.0||projCoords.y>1.0||projCoords.x<0.0||projCoords.y<0.0||projCoords.z<0.0)
        return 1.0;

    return  clamp(exp(cdz),0,1);
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
    vec3 lighting = (ambient + (shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}