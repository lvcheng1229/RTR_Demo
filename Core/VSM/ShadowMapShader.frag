#version 330 core

in vec4 v_position;
out vec2 outColor;
uniform bool acne;
void main()
{             
    float depth = v_position.z / v_position.w;
    depth = depth * 0.5 + 0.5;// TO NDC [0, 1]

    float moment1 = depth; // 一阶矩
    float moment2 = depth * depth; // 二阶矩

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    
    if(acne)
        moment2 += 0.25 * (dx * dx + dy * dy); // 解决acne问题，可看GPU GEMS3详细了解
        
    outColor = vec2(moment1, moment2);
}