#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;

uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 edgeColor;
uniform vec3 objColor;

uniform float threshold;
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

if(dot(viewDir,norm)<threshold)
    result=edgeColor;

FragColor = vec4(result, 1.0)*vec4(objColor,1.0);
}