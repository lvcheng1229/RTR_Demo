#version 330 core
layout (location = 0) out vec3 gColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gDepth;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objColor;

void main()
{	
vec3 lightDir = normalize(lightPos - FragPos);
vec3 norm = normalize(Normal);
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

gColor = result*objColor;
gNormal=norm;
gDepth=vec3(0.8);
}