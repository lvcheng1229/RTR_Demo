#version 330 core
out vec4 FragColor;
in vec3 normal;

uniform samplerCube environmentMap;

uniform float sh_coef_r[9];
uniform float sh_coef_g[9];
uniform float sh_coef_b[9];

uniform bool useCubemap;

uniform float theta;

uniform mat4 rotateMat;
float calcSHIrradiance(float coef[9], vec3 norm)
{
    //float l00 = coef[0];
    //float l1m1 = coef[1];
    //float l10 = coef[2];
    //float l11 = coef[3];
    //float l2m2 = coef[4];
    //float l2m1 = coef[5];
    //float l20 = coef[6];
    //float l21 = coef[7];
    //float l22 = coef[8];

    //rotate:book <global illumination> qin chun lin page 678

    float costheta=cos(theta);
    float sintheta=sin(theta);
    
    float cos2theta=cos(2*theta);
    float sin2theta=sin(2*theta);

    float l00 = coef[0];

    float l1m1 = costheta*coef[1]+sintheta*coef[3];
    float l10 = coef[2];
    float l11 = -sintheta*coef[1]+costheta*coef[3];

    float l2m2 = cos2theta*coef[4]+sin2theta*coef[8];
    float l2m1 = costheta*coef[5]+sintheta*coef[7];
    float l20 = coef[6];
    float l21 = -sintheta*coef[5]+costheta*coef[7];
    float l22 = -sin2theta*coef[4]+cos2theta*coef[8];

    const float PI = 3.1415926535897;
    const float l0 = 1.0;
    const float l1 = 2.0 / 3.0;
    const float l2 = 1.0 / 4.0;
    const float c0 = 0.282095;
    const float c1 = 0.488603;
    const float c2 = 1.09255;
    const float c3 = 0.546274;
    const float c4 = 0.315392;

    vec3 normSqr = norm * norm;

    float irradiance = 
        l00 * c0 * l0 + 
        c1 * (-norm.y * l1m1 + norm.z * l10 - norm.x * l11) * l1 + 
        c2 * (-norm.x * norm.y * l2m2 - norm.y * norm.z * l2m1 - norm.x * norm.z * l21) * l2 + 
        c4 * (3.0 * normSqr.z - 1.0) * l20 * l2 +
        c3 * (normSqr.x - normSqr.y) * l22 * l2;
    return irradiance;
}
void main()
{		 
    vec3 ambient = vec3(1.0);
    if (useCubemap) 
    {
        ambient = textureCube(environmentMap, vec3(rotateMat*vec4(normal,1.0))).xyz;
    } 
    else 
    { 
        ambient = vec3(calcSHIrradiance(sh_coef_r, normal), calcSHIrradiance(sh_coef_g, normal), calcSHIrradiance(sh_coef_b, normal));
    }
    FragColor = vec4(ambient, 1.0);

}