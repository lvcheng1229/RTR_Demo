#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 FragPos;

in vec3 tangent;
in vec3 bitangent;

uniform vec3 lightcolor;
uniform vec3 lightPos;
uniform vec3 viewPos;


uniform bool showDiffuse;
uniform float alpha;
uniform float alphay;
uniform int type;
const float PI=3.1415926535;
//notice that in this demo,Vis=G/(4*NoL*Nov),so G=vis*(4*NoL*Nov)
//type1:Implicit
//type2:Neumann[1999]
//type3:Kelemen[2001]
//type4:Schlick GGX[1994][important]
//type5:Smith used in Disney[2007]

//type 6 7 8 9 10 are based on Paper "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"

//type6:SmithJointApprox used in UE
//type7:SmithJointCorrelated height_correlated_Smith_G2 used in Google Filament and Frostbite
//type8:GGX-Smith Correlated Joint Approximate used in Unity HDRP
//type9:Respawn Entertainment
//type10:mithJointAniso

float Vis_Implicit()
{
	return 0.25;
}

// [Neumann et al. 1999, "Compact metallic reflectance models"]
float Vis_Neumann( float NoV, float NoL )
{
	return 1 / ( 4 * max( NoL, NoV ) );
}

// [Kelemen 2001, "A microfacet based coupled specular-matte brdf model with importance sampling"]
float Vis_Kelemen( float VoH )
{
	// constant to prevent NaN
	return 1.0/( 4 * VoH * VoH + 1e-5);
}
// Tuned to match behavior of G_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float Vis_Schlick_GGX(float a, float NoV, float NoL)
{
    float k = (a+1.0)*(a+1.0)/8.0;
    float G_SchlickV = NoV * (1 - k) + k;
    float G_SchlickL = NoL * (1 - k) + k;
    return 0.25 / (G_SchlickV*G_SchlickL);
}
// Smith term for GGX
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
// [Walter 2007]
float Vis_Smith( float a, float NoV, float NoL )
{
    float a2=(0.5+a/2.0)*(0.5+a/2.0);
	float G_SmithV = NoV + sqrt( NoV * (NoV - NoV * a2) + a2 );
	float G_SmithL = NoL + sqrt( NoL * (NoL - NoL * a2) + a2 );
	return 1.0 / ( G_SmithV * G_SmithL );
}
//UE
// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox( float a, float NoV, float NoL )
{
    a=a*a;//a=roughness*roughness and in this demo alpha = roughness
	float G_SmithV = NoL * ( NoV * ( 1 - a ) + a );
	float G_SmithL = NoV * ( NoL * ( 1 - a ) + a );
	return 0.5 / ( G_SmithV + G_SmithL );
}
//Google Filament
//Frostbite [Lagarde 2014 ] height-correlated Smith G2
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointCorrelated(float a, float NoV, float NoL) 
{
    float a2=a*a;
	float G_SmithV = NoL * sqrt(NoV * (NoV - NoV * a2) + a2);
	float G_SmithL = NoV * sqrt(NoL * (NoL - NoL * a2) + a2);
	return 0.5 / (G_SmithV + G_SmithL);
}
//GGX-Smith Correlated Joint Approximate used in Unity HDRP
float Vis_SmithJointGGX_Unity(float a, float NoV, float NoL) 
{
    float a2=a*a;
	float G_SmithV = NoL * (NoV * (1 - a) + a);
	float G_SmithL = NoV * sqrt(NoL * (NoL - NoL * a2) + a2);
	return 0.5 / (G_SmithV + G_SmithL);
}
//[Hammon 2017] Respawn Entertainment GGX-Smith Joint
float Vis_Respawn_Entertainment(float a, float NoV, float NoL) 
{
    return 0.5*mix(2*abs(NoL)*abs(NoV),abs(NoL)+abs(NoV),a);
}
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointAniso(float ax, float ay, float NoV, float NoL, float XoV, float XoL, float YoV, float YoL)
{
	float Vis_SmithV = NoL * length(vec3(ax * XoV, ay * YoV, NoV));
	float Vis_SmithL = NoV * length(vec3(ax * XoL, ay * YoL, NoL));
	return 0.5 / (Vis_SmithV + Vis_SmithL);
}
void main()
{	
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 norm = normalize(normal);
    vec3 tang=normalize(tangent);
    vec3 bita=normalize(bitangent);
    //diffuse
    float diffuse = 0.9*max(dot(lightDir, norm), 0);
    //ambition
    float ambient = 0.3f;
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    float spec = 0.0;
    if(type==1)
    {
        spec=Vis_Implicit();
    }
    else if (type==2)
    {
        spec=Vis_Neumann( dot(norm,viewDir), dot(norm,lightDir));
    }
    else if(type==3)
    {
        spec=Vis_Kelemen( dot(viewDir,halfwayDir));
    }
    else if(type==4)
    {
        spec=Vis_Schlick_GGX(alpha,max(dot(norm,viewDir),0),max(dot(norm,lightDir),0));
    }
    else if(type==5)
    {
        spec=Vis_Smith(alpha,max(dot(norm,viewDir),0),max(dot(norm,lightDir),0));
    }
    else if(type==6)
    {
        spec=Vis_SmithJointApprox(alpha,max(dot(norm,viewDir),0),max(dot(norm,lightDir),0));
    }
    else if(type==7)
    {
        spec= Vis_SmithJointCorrelated(alpha,max(dot(norm,viewDir),0),max(dot(norm,lightDir),0));
    }
    else if(type==8)
    {
        spec=Vis_SmithJointGGX_Unity(alpha,dot(norm,viewDir),dot(norm,lightDir));
    }
    else if(type==9)
    {
        spec=Vis_Respawn_Entertainment(alpha,dot(norm,viewDir),dot(norm,lightDir));
    }
    else if(type==10)
    {
        spec=Vis_SmithJointAniso(alpha, alphay, dot(norm,viewDir), dot(norm,lightDir), dot(tang,viewDir), dot(tang,lightDir), dot(bita,viewDir), dot(bita,lightDir));
    }
    spec*=4*max(dot(norm,viewDir),0)*max(dot(norm,lightDir),0);
    vec3 result=vec3(0);
    if(showDiffuse)
    {
        result = lightcolor * vec3(ambient + diffuse + spec);
    }
    else
    {
        result=lightcolor*spec;
    }

    //tone mapping
    result=result/(result+vec3(1.0));
    FragColor = vec4(result, 1.0);
}