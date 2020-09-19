#version 330 core
out vec4 FragColor;

in GS_OUT {
    vec3 normal;
	vec3 FragPos;
    vec3 lineTexCoords;
} fs_in;
uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objColor;

uniform sampler1D lineTex;
void main()
{	
vec3 lightDir = normalize(lightPos - fs_in.FragPos);
vec3 norm = normalize(fs_in.normal);
//diffuse
float diffuse = 0.9*max(dot(lightDir, norm), 0);
//ambition
float ambient = 0.3f;
//specular
vec3 viewDir = normalize(viewPos - fs_in.FragPos);
vec3 reflectDir = reflect(-lightDir, norm);
vec3 halfwayDir = normalize(lightDir + viewDir);  
float  spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);

vec3 result = lightcolor * vec3(ambient + diffuse + spec);
FragColor = vec4(result, 1.0)*vec4(objColor,1.0);
float maxValue=max(max(fs_in.lineTexCoords.x,fs_in.lineTexCoords.y),fs_in.lineTexCoords.z);
float factor=texture1D(lineTex,maxValue).r;
FragColor=vec4(FragColor.xyz*factor,1.0);
}