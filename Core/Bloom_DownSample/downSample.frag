#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D image;
void main()
{             
     vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel    
     vec3 result=vec3(0);
     //result += texture(image, TexCoords+vec2(0,0)*tex_offset).rgb;
     result += texture(image, TexCoords+vec2(-1,1)*tex_offset).rgb;
     result += texture(image, TexCoords+vec2(-1,-1)*tex_offset).rgb;
     result += texture(image, TexCoords+vec2(1,1)*tex_offset).rgb;
     result += texture(image, TexCoords+vec2(1,-1)*tex_offset).rgb;
     result/=4.0;
     FragColor = vec4(result, 1.0);
}