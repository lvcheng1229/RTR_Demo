#ifndef g3dmath_glsl
#define g3dmath_glsl
#include <compatibility.glsl>

/**
  \file g3dmath.glsl
  \author Morgan McGuire (http://graphics.cs.williams.edu), Michael Mara (http://www.illuminationcodified.com)
  Defines basic mathematical constants and functions used in several shaders
*/
#define pi (3.1415926)
#define invPi (0.318309886)
// 1.0 / (8.0 pi); used for normalizing the glossy lobe
#define inv8pi (0.0397887358)

float square(in float x) {
    return x * x;
}

/** Computes x<sup>5</sup> */
float pow5(in float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

float maxComponent(vec2 a) {
    return max(a.x, a.y);
}

/** Compute Schlick's approximation of the Fresnel coefficient.  The
    original goes to 1.0 at glancing angles, which makes objects
    appear to glow, so we clamp it.

    We never put a Fresnel term on perfectly diffuse surfaces, so if
    F0 is exactly black we keep the result black.
    */
// Must match GLG3D/UniversalBSDF.h
vec3 computeF(in vec3 F0, in float cos_i) {
    return (F0.r + F0.g + F0.b > 0.0) ? (mix(F0, vec3(1.0), clamp(pow5(1.0 - cos_i), 0.0, 0.3))) : F0;
}

/** Matches UniversalBSDF::unpackGlossyExponent*/
float unpackGlossyExponent(in float e) {
    return square((e * 255.0 - 1.0) * (1.0 / 253.0)) * 1024.0 + 1.0;
}

float packGlossyExponent(in float x) {
    // Never let the exponent go above the max representable non-mirror value in a uint8
    return (clamp(sqrt((x - 1.0f) * (1.0f / 1024.0f)), 0.0f, 1.0f) * 253.0f + 1.0f) * (1.0f / 255.0f);
}

/**
 Computes pow2(mipLevel), avoiding the expensive log2 call needed for the 
 actual MIP level.
 */
float computeSampleRate(vec2 texCoord, vec2 samplerSize) {
    texCoord *= samplerSize;
    return maxComponent(max(abs(dFdx(texCoord)), abs(dFdy(texCoord))));
}


/** http://blog.selfshadow.com/2011/07/22/specular-showdown/ */ 
float computeToksvigGlossyExponent(float rawGlossyExponent, float rawNormalLength) {
    float ft = rawNormalLength / lerp(rawGlossyExponent, 1.0, rawNormalLength);
    float scale = (1.0 + ft * rawGlossyExponent) / (1.0 + rawGlossyExponent);
    return scale * rawGlossyExponent;
}


#endif