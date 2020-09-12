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

uniform float pi;
float ck(float k)
{
    return pi*(2.0*k-1.0);
}
float sig(int m,float d,float z)
{
    float res=0.5;
    for(int i=1;i<=m;i++)
    {
        res+=2.0/ck(i)*sin(ck(i)*(d-z+0.032));
    }
    return res;
}
float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float d = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;      

    float shadow=0.0;
    if(projCoords.z > 1.0||projCoords.x>1.0||projCoords.y>1.0||projCoords.x<0.0||projCoords.y<0.0||projCoords.z<0.0)
        return 0.7;

    //卷积阴影存在一些问题
    //比如在d-z=0两侧的波动较大
    //并且当d-z=0时，函数取值为0.5
    //所以我们偏移了0.032
    shadow=sig(4,d,currentDepth);
    shadow+=0.1;
    shadow*=0.9;
        
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
    vec3 lighting = (ambient + (shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}