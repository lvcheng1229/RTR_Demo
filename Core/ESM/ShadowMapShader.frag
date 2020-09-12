#version 330 core

in vec4 v_position;
out vec2 outColor;

uniform float c;
void main()
{             
    float depth = v_position.z / v_position.w;
    depth = depth * 0.5 + 0.5;// TO NDC [0, 1]
    outColor = vec2(exp(c*depth),0);
}