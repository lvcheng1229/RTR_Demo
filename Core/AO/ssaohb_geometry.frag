#version 330 core
layout (location = 0) out vec3 gPosition;//store depth value in w
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

float near = 0.1; 
float far  = 50.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    //gPosition = vec4(FragPos,LinearizeDepth(gl_FragCoord.z));
    gPosition=FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}