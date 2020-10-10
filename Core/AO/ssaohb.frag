#version 330 core

#define M_PI 3.14159265f
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;//store linear z value in w
uniform sampler2D texNoise;
uniform sampler2D gNormal;
const float  NUM_STEPS = 4;
const float  NUM_DIRECTIONS = 8;

//RTR4 P99 4.75
uniform float c;
uniform float a;

uniform vec2 InvFullResolution;
uniform float NegInvR2;
uniform float NDotVBias;
uniform float AOMultiplier;
uniform float RadiusToScreen;

const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 
//vec3 UVToView(vec2 uv, float eye_z)
//{
//  return vec3((uv * vec2(a/c,1.0/c)) * eye_z, eye_z);
//}

vec3 FetchViewPos(vec2 UV)
{
  //float ViewDepth = texture(gPosition,UV).w;
  //return UVToView(UV, ViewDepth);
  return texture(gPosition,UV).rgb;
}

//vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
//{
//  vec3 V1 = Pr - P;
//  vec3 V2 = P - Pl;
//  return (dot(V1,V1) < dot(V2,V2)) ? V1 : V2;
//}

//vec3 ReconstructNormal(vec2 UV, vec3 P)
//{
//  vec3 Pr = FetchViewPos(UV + vec2(InvFullResolution.x, 0));
//  vec3 Pl = FetchViewPos(UV + vec2(-InvFullResolution.x, 0));
//  vec3 Pt = FetchViewPos(UV + vec2(0, InvFullResolution.y));
//  vec3 Pb = FetchViewPos(UV + vec2(0, -InvFullResolution.y));
//  return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)));
//}

//----------------------------------------------------------------------------------
float Falloff(float DistanceSquare)
{
  // 1 scalar mad instruction
  return DistanceSquare * NegInvR2 + 1.0;
}

//----------------------------------------------------------------------------------
// P = view-space position at the kernel center
// N = view-space normal at the kernel center
// S = view-space position of the current sample
//----------------------------------------------------------------------------------
float ComputeAO(vec3 P, vec3 N, vec3 S)
{
  vec3 V = S - P;
  float VdotV = dot(V, V);
  float NdotV = dot(N, V) * 1.0/sqrt(VdotV);

  // Use saturate(x) instead of max(x,0.f) because that is faster on Kepler
  return clamp(NdotV - NDotVBias,0,1) * clamp(Falloff(VdotV),0,1);
}

//----------------------------------------------------------------------------------
vec2 RotateDirection(vec2 Dir, vec2 CosSin)
{
  return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
              Dir.x*CosSin.y + Dir.y*CosSin.x);
}

//----------------------------------------------------------------------------------
vec3 GetJitter()
{
  return texture(texNoise, TexCoords * noiseScale).xyz;
}

float ComputeCoarseAO(vec2 FullResUV, float RadiusPixels, vec3 Rand, vec3 ViewPosition, vec3 ViewNormal)
{

  // Divide by NUM_STEPS+1 so that the farthest samples are not fully attenuated
  float StepSizePixels = RadiusPixels / (NUM_STEPS + 1);

  const float Alpha = 2.0 * M_PI / NUM_DIRECTIONS;
  float AO = 0;

  for (float DirectionIndex = 0; DirectionIndex < NUM_DIRECTIONS; ++DirectionIndex)
  {
    float Angle = Alpha * DirectionIndex;

    // Compute normalized 2D direction
    vec2 Direction = RotateDirection(vec2(cos(Angle), sin(Angle)), vec2(cos(Rand.x*Alpha),sin(Rand.x*Alpha)));

    // Jitter starting sample within the first step
    float RayPixels = (Rand.y * StepSizePixels + 1.0);

    for (float StepIndex = 0; StepIndex < NUM_STEPS; ++StepIndex)
    {
      vec2 SnappedUV = round(RayPixels * Direction) * InvFullResolution + FullResUV;
      vec3 S = FetchViewPos(SnappedUV);

      RayPixels += StepSizePixels;

      AO += ComputeAO(ViewPosition, ViewNormal, S);
    }
  }

  AO *= AOMultiplier / (NUM_DIRECTIONS * NUM_STEPS);
  return clamp(1.0 - AO * 2.0,0,1);
}

void main()
{
  vec2 uv = TexCoords;
  vec3 ViewPosition = FetchViewPos(uv);

  // Reconstruct view-space normal from nearest neighbors
  //vec3 ViewNormal = -ReconstructNormal(uv, ViewPosition);
    vec3 ViewNormal=normalize(texture(gNormal,uv).rgb);
  // Compute projection of disk of radius control.R into screen space
  float RadiusPixels = RadiusToScreen / ViewPosition.z;

  // Get jitter vector for the current full-res pixel
  vec3 Rand = GetJitter();

  float AO = ComputeCoarseAO(uv, RadiusPixels, Rand, ViewPosition, ViewNormal);

  FragColor=AO;
  
}