#version 330 core
layout (location = 0) out vec4 FragColor;//a chanel is z value
layout (location = 1) out vec4 NearFieldColor;
layout (location = 2) out vec4 restColor;

in vec2 texCoords;
in vec3 normal;
in vec3 FragPos;
in vec3 viewFragPos;

uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D sampleTexture;

uniform float focusDistance;
uniform float focusRange;

void main()
{	
vec3 lightDir = normalize(lightPos - FragPos);
vec3 norm = normalize(normal);
//diffuse
float diffuse = 0.9*max(dot(lightDir, norm), 0);
//ambition
float ambient = 0.3f;
//specular
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);
vec3 halfwayDir = normalize(lightDir + viewDir);  
float  spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);

vec3 result = lightcolor * vec3(ambient + diffuse + spec);
FragColor = vec4(result, 1.0)*texture(sampleTexture,texCoords);

if((-viewFragPos.z)<focusDistance)
{
    NearFieldColor=FragColor;
    restColor=vec4(0,0,0,1);
}
else
{
    NearFieldColor=vec4(0,0,0,1);
    restColor=FragColor;
}

FragColor.w=-viewFragPos.z;
}