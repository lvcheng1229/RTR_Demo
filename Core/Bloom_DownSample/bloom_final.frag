#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D downSample0;
uniform sampler2D downSample1;
uniform sampler2D downSample2;
uniform sampler2D downSample3;

uniform bool bloom;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor=vec3(0);      
    bloomColor += texture(downSample0, TexCoords).rgb;
    bloomColor += texture(downSample1, TexCoords).rgb;
    bloomColor += texture(downSample2, TexCoords).rgb;
    bloomColor += texture(downSample3, TexCoords).rgb;
    bloomColor /= 4.0;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}