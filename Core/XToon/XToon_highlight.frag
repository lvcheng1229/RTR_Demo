#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float s;
uniform sampler2D texsample;
void main () 
{
	vec3 norm=normalize(normal);
	vec3 lightDir=normalize(lightPos-FragPos);
	vec3 r=normalize(reflect(-lightDir,norm));
	
	vec3 viewDir=normalize(viewPos-FragPos);

	float u=(dot(lightDir,norm)+1.0)/2.0;
	float v=pow(abs(dot(viewDir,r)),s);

	FragColor = vec4(texture2D(texsample, vec2(u, v)).xyz,1.0); 
}