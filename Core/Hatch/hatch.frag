#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambiWeight;
uniform float diffWeight;
uniform float specWeight;

uniform sampler2D hatch1;
uniform sampler2D hatch2;
uniform sampler2D hatch3;
uniform sampler2D hatch4;
uniform sampler2D hatch5;
uniform sampler2D hatch6;
void main()
{	
    
vec3 lightDir = normalize(lightPos - FragPos);
vec3 norm = normalize(normal);
//diffuse
float diffuse = 1.0*max(dot(lightDir, norm), 0);
//specular
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);
vec3 halfwayDir = normalize(lightDir + viewDir);  
float  spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);

float shading=(ambiWeight + diffWeight*diffuse + specWeight*spec)/(ambiWeight+specWeight+diffWeight);

vec3 c=vec3(0.0);
vec2 vUv=TexCoords;
float step = 1. / 6.;


if( shading <= step ){   
	c = mix( texture2D( hatch6, vUv ), texture2D( hatch5, vUv ), 6. * shading ).xyz;
}
if( shading > step && shading <= 2. * step ){
	c = mix( texture2D( hatch5, vUv ), texture2D( hatch4, vUv) , 6. * ( shading - step ) ).xyz;
}
if( shading > 2. * step && shading <= 3. * step ){
	c = mix( texture2D( hatch4, vUv ), texture2D( hatch3, vUv ), 6. * ( shading - 2. * step ) ).xyz;
}
if( shading > 3. * step && shading <= 4. * step ){
	c = mix( texture2D( hatch3, vUv ), texture2D( hatch2, vUv ), 6. * ( shading - 3. * step ) ).xyz;
}
if( shading > 4. * step && shading <= 5. * step ){
	c = mix( texture2D( hatch2, vUv ), texture2D( hatch1, vUv ), 6. * ( shading - 4. * step ) ).xyz;
}
if( shading > 5. * step ){
	c = mix( texture2D( hatch1, vUv ), vec4( 1. ), 6. * ( shading - 5. * step ) ).xyz;
}

FragColor=vec4(c,1.0);
}