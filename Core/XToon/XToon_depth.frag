#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform sampler2D texsample;
uniform float zmin;
uniform float zmax;
void main () 
{
	vec3 norm=normalize(normal);
	vec3 lightDir=normalize(lightPos-FragPos);


	float u=(dot(lightDir,norm)+1.0)/2.0;

	float zed =  -FragPos.z;
	float v=1.0 - log(zed / zmin) / log(zmax / zmin);
	
	FragColor = vec4(texture2D(texsample, vec2(u, v)).xyz,1.0); 
}