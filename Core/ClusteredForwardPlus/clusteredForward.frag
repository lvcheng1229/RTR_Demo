#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 v_position;

uniform sampler2D sampleTexture;
uniform sampler2D u_lightbuffer;
uniform int numLights;

uniform vec3 u_cameraPos;
uniform sampler2D u_clusterbuffer;

uniform float cameraFOVScalar;
uniform float cameraAspect;
uniform float cameraNear;

uniform bool useLogarithmic;
uniform bool useDynamic;

uniform float invRange;
uniform float invDist;

uniform int xSlices;
uniform int ySlices;
uniform int zSlices;

uniform int textureHeight;

uniform mat4 u_viewMatrix;

uniform bool showLightNum;
uniform float logOffset;
struct Light{
    vec3 position;
    float radius;
    vec3 color;
};
  float logScale(float z) {
    return log(z-cameraNear+1) * invRange;
  }

  float ExtractFloat(sampler2D texture, int textureWidth, int textureHeight, int index, int component) {
    float u = float(index + 1) / float(textureWidth + 1);
    int pixel = component / 4;
    float v = float(pixel + 1) / float(textureHeight + 1);
    vec4 texel = texture2D(texture, vec2(u, v));
    int pixelComponent = component - pixel * 4;
    if (pixelComponent == 0) {
      return texel[0];
    } else if (pixelComponent == 1) {
      return texel[1];
    } else if (pixelComponent == 2) {
      return texel[2];
    } else if (pixelComponent == 3) {
      return texel[3];
    }
  }

Light UnpackLight(int index) 
{
    Light light;
    
    float u = float(index + 1) / float(numLights + 1);
    
    vec4 v1 = texture2D(u_lightbuffer, vec2(u, 0.3));
    
    vec4 v2 = texture2D(u_lightbuffer, vec2(u, 0.6));
    
    light.position = v1.xyz;

    light.radius = v1.a;

    light.color = v2.rgb;

    return light;
}
// Cubic approximation of gaussian curve so we falloff to exactly 0 at the light radius
float cubicGaussian(float h) 
{
    if (h < 1.0) return 0.25 * pow(2.0 - h, 3.0) - pow(1.0 - h, 3.0);
    else if (h < 2.0) return 0.25 * pow(2.0 - h, 3.0);
    else return 0.0;
}
void main()
{	
    vec3 fragColor=vec3(0);
    vec3 camPos=u_cameraPos;

    //view space coords
    vec4 viewPos = u_viewMatrix * vec4(v_position,1.0);
    //vec3 viewPos = v_position;
    float halfFrustumHeight = -viewPos.z * cameraFOVScalar;
    float halfFrustumWidth = halfFrustumHeight * cameraAspect;
    
    int zC;
    if(useLogarithmic) 
    {
      if(useDynamic)zC = int( max(0.0, min(float(zSlices-1), logScale(invDist * -viewPos.z) * float(zSlices + logOffset) - float(logOffset))));
      else zC = int( max(0.0, min(float(zSlices-1), logScale(invRange * -viewPos.z) * float(zSlices + logOffset) - float(logOffset))));    
    }
    else
    {
      if(useDynamic) zC = int(min(float(zSlices-1), invDist * -viewPos.z * invRange * float(zSlices)));
      else zC = int(min(float(zSlices-1), invRange * -viewPos.z * invRange * float(zSlices)));
    } 

    int yC = int((viewPos.y + halfFrustumHeight) / (2.0 * halfFrustumHeight) * float(ySlices));
    int xC = int((viewPos.x + halfFrustumWidth) / (2.0 * halfFrustumWidth) * float(xSlices));

    int row = xC + yC * xSlices + zC * xSlices * ySlices;

    int totalClusters=xSlices*ySlices*zSlices;

    //make sure we're safely within the NEXT row with the +1/+1
    float u = float(row+1)/float(totalClusters+1); 

    //pull first "red" value, no need for extractfloat yet
    int clusterNumLights = int(texture2D(u_clusterbuffer, vec2(u,0)).r); 

    for(int i=0;i<numLights;i++)
    {
        if(i > clusterNumLights) break;

        int lightIndex = int(ExtractFloat(u_clusterbuffer, totalClusters, textureHeight, row, i));

        vec3 viewDir = normalize(v_position - camPos.xyz);//note:direction

        Light light=UnpackLight(lightIndex);

        float lightDistance = distance(light.position, v_position);
        
        vec3 L=(light.position - v_position) / lightDistance;//normalize
        
        float lightIntensity = cubicGaussian(2.0 * lightDistance / light.radius);

        float lambertTerm = max(dot(L, normalize(normal)), 0.0);

        fragColor+=lambertTerm*light.color*lightIntensity;
    }

    fragColor+=vec3(0.02);//ambient

    vec3 aa=vec3(0);
    
    if(clusterNumLights<numLights*0.2)aa=vec3(1,0,0);
    else if(clusterNumLights<numLights*0.4)aa=vec3(0,1,0);
    else if(clusterNumLights<numLights*0.6)aa=vec3(0,0,1);
    else if(clusterNumLights<numLights*0.8)aa=vec3(1,1,1);
    else aa=vec3(1,1,0);

    FragColor=vec4(vec3(fragColor),1.0);
    
    if(showLightNum) FragColor=vec4(vec3(aa),1.0);
}