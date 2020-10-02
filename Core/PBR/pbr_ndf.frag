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

uniform float alpha;
uniform float alphay;
uniform bool showDiffuse;
uniform int type;
uniform float gamma;

const float PI=3.1415926535;
float Square(float a)
{
    return a*a;
}
//type1:D_Blinn
//type2:D_Beckmann
//type3:D_GGX
//type4:D_GGXaniso
//type5:D_Beckmann_aniso
//type6:D_Hyper_Cauchy
// [Blinn 1977, "Models of light reflection for computer synthesized pictures"]
float D_Blinn( float a2, float NoH )
{
	float n = 2 / a2 - 2;
	return (n+2) / (2*PI) * pow(NoH,n);		// 1 mad, 1 exp, 1 mul, 1 log
}

// [Beckmann 1963, "The scattering of electromagnetic waves from rough surfaces"]
float D_Beckmann( float a2, float NoH )
{
	float NoH2 = NoH * NoH;
	return exp( (NoH2 - 1) / (a2 * NoH2) ) / ( PI * a2 * NoH2 * NoH2 );
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float a2, float NoH )
{
	float d = ( NoH * a2 - NoH ) * NoH + 1;	// 2 mad
	return a2 / ( PI*d*d );					// 4 mul, 1 rcp
}

// Anisotropic GGX
// [Burley 2012, "Physically-Based Shading at Disney"]
float D_GGXaniso( float ax, float ay, float NoH, float XoH, float YoH )
{
// The two formulations are mathematically equivalent
#if 1
	float a2 = ax * ay;
	vec3 V = vec3(ay * XoH, ax * YoH, a2 * NoH);
	float S = dot(V, V);

	return (1.0f / PI) * a2 * Square(a2 / S);
#else
	float d = XoH*XoH / (ax*ax) + YoH*YoH / (ay*ay) + NoH*NoH;
	return 1.0f / ( PI * ax*ay * d*d );
#endif
}
// Anisotropic Beckmann
float D_Beckmann_aniso( float ax, float ay, float NoH, vec3 H, vec3 X, vec3 Y )
{
    float XoH = dot( X, H );
    float YoH = dot( Y, H );
    float d = - (XoH*XoH / (ax*ax) + YoH*YoH / (ay*ay)) / NoH*NoH;
    return exp(d) / ( PI * ax*ay * NoH * NoH * NoH * NoH );
}
float D_Hyper_Cauchy(float alpha,float NoH)
{
    float nom=(gamma-1.0)*pow(2,(gamma-1.0));
    float NoH2=NoH*NoH;
    float a2=alpha*alpha;
    float d=PI*a2*NoH2*NoH2*pow((2+(1-NoH2)/(NoH2*a2)),gamma);
    return nom/d;
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
        spec=D_Blinn(alpha*alpha,dot(norm,halfwayDir));
    }
    else if(type==2)
    {
        spec=D_Beckmann(alpha*alpha,dot(norm,halfwayDir));
    }
    else if(type==3)
    {
        spec=D_GGX(alpha*alpha,dot(norm,halfwayDir));
    }
    else if(type==4)
    {
        spec=D_GGXaniso(alpha,alphay,dot(norm,halfwayDir),dot(halfwayDir,tang),dot(halfwayDir,bita));
    }
    else if(type==5)
    {
        spec=D_Beckmann_aniso(alpha,alphay,dot(norm,halfwayDir),halfwayDir,tang,bita);
    }
    else if(type==6)
    {
       spec=D_Hyper_Cauchy(alpha,dot(norm,halfwayDir));
    }

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