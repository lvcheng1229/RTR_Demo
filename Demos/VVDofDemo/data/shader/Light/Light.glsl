// -*- c++ -*-
/** 
  \file Light.glsl
  \author Morgan McGuire (http://graphics.cs.williams.edu), Michael Mara (http://www.illuminationcodified.com)
  Defines helper functions to calculate the light contribution to a specified point.

  In order for the computeShading function to do anything at least one of two
  macros must be defined:

  HAS_LAMBERTIAN_TERM and HAS_GLOSSY_TERM for lambertian
  and glossy reflection respectively.
  
  If NORMALBUMPMAP is defined, then this file must have tan_Z and backside in scope
*/
#ifndef Light_glsl
#define Light_glsl

#ifndef HAS_GLOSSY_TERM
#   error "Light.glsl requires HAS_GLOSSY_TERM to be defined (e.g., by including UniversalMaterial_uniforms.glsl)"
#endif

#ifndef HAS_LAMBERTIAN_TERM
#   error "Light.glsl requires HAS_LAMBERTIAN_TERM to be defined (e.g., by including UniversalMaterial_uniforms.glsl)"
#endif

#include <g3dmath.glsl>

float shadowFetch(sampler2DShadow shadowMap, vec3 coord, vec2 invSize) {
    return shadow2D(shadowMap, 
        vec3(clamp(coord.xy, invSize, vec2(1) - invSize), coord.z))
#       if __VERSION__ <= 120
            .r
#       endif
        ;
}


/** Returns a number between 0 = fully shadowed and 1 = fully lit.  Assumes that the shadowCoord is already within the spotlight cone. */
float shadowMapVisibility(vec3 lightDirection, vec4 lightPosition, vec4 lightAttenuation, vec4 shadowCoord, sampler2DShadow shadowMap, vec2 invSize) {    
    // Compute projected shadow coord.
    vec3 projShadowCoord = shadowCoord.xyz / shadowCoord.w;

    // "side" and "diagonal" offset variables used to produce vectors to the 
    // 8-neighbors, which leads to a smoother shadow result (0.71 is sqrt(2)/2).
    vec4 s = vec4(invSize, -invSize.x, 0.0);
    vec4 d = s * 0.71;

    return
        ((shadowFetch(shadowMap, projShadowCoord, invSize) +
             
          shadowFetch(shadowMap, projShadowCoord + s.xww, invSize) +
          shadowFetch(shadowMap, projShadowCoord - s.xww, invSize) +
          shadowFetch(shadowMap, projShadowCoord + s.wyw, invSize) +
          shadowFetch(shadowMap, projShadowCoord - s.wyw, invSize) +
         
          shadowFetch(shadowMap, projShadowCoord + d.xyw, invSize) +
          shadowFetch(shadowMap, projShadowCoord - d.zyw, invSize) +
          shadowFetch(shadowMap, projShadowCoord + d.zyw, invSize) +
          shadowFetch(shadowMap, projShadowCoord - d.xyw, invSize)) / 9.0);
            
    /*
    // Old code to clamp to zero beyond border. We clamp 1 pixel beyond in order to
    // get correct blended values near the border.
    float(all(bvec4(greaterThanEqual(projShadowCoord.xy, -s.xy),
                    lessThanEqual(projShadowCoord.xy, vec2(1.0, 1.0) + s.xy)))) */

}

/** Below this value, attenuation is treated as zero. This is non-zero only to avoid numerical imprecision. */
const float attenuationThreshold = 2e-17;

/** Returns true if the vector w_i (which points at the light's center) is within the field of view of this light source. */
bool inLightFieldOfView
   (vec3                w_i, 
    vec3                lightLookVector, 
    vec3                lightRightVector, 
    vec3                lightUpVector,
    bool                rectangular, 
    float               cosFOV) {

    if (rectangular) {

        // Project wi onto the light's xz-plane and then normalize
        vec3 w_horizontal = normalize(w_i - dot(w_i, lightRightVector) * lightRightVector);
        vec3 w_vertical   = normalize(w_i - dot(w_i, lightUpVector)    * lightUpVector);

        // Now test against the view cone in each of the planes 
        return 
            (dot(w_horizontal, lightLookVector) < -cosFOV) &&
            (dot(w_vertical,   lightLookVector) < -cosFOV);
    } else {
        return dot(lightLookVector, w_i) < -cosFOV;
    }
}


/** Computes attenuation due to backface or radial falloff */
float computeAttenuation
  (in vec3              n, 
   in vec4              lightPosition, 
   in vec4              lightAttenuation, 
   in vec3              wsPosition, 
   in vec3              lightLook, 
   in vec3              lightUpVector, 
   in vec3              lightRightVector, 
   in bool              lightRectangular, 
   out vec3             w_i) {
   
    // Light vector
    w_i = lightPosition.xyz - wsPosition.xyz * lightPosition.w;
    float lightDistance = length(w_i);
    w_i = normalize(w_i);

    float attenuation =
        lightPosition.w *

        (inLightFieldOfView(w_i, lightLook, lightRightVector, lightUpVector, lightRectangular, lightAttenuation.w) ? 
         
         // Within spotlight cone
         (1.0 / (4.0 * pi * dot( vec3(1.0, lightDistance, lightDistance * lightDistance), lightAttenuation.xyz))) : 
         
         // Outside spotlight cone
         0.0) + (1.0 - lightPosition.w);

#   ifdef NORMALBUMPMAP
        // For a bump mapped surface, do not allow illumination on the back side even if the
        // displacement creates a light-facing surface, since it should be self-shadowed for any 
        // large polygon.
        attenuation *= float(dot(tan_Z.xyz, w_i) * backside > 0.0);
#   endif

    // Attenuation is modulated by the cosine of the angle of incidence
    attenuation *= max(dot(w_i, n), 0.0);

    return attenuation;
}


void computeShading
    (in vec3            wsN, 
    in vec3             wsE, 
    float               attenuation, 
    in float            glossyExponent, 
    in float            glossyCoefficient, 
    in vec3             lightColor, 
    inout vec3          I_lambertian,
    inout vec3          I_glossy, 
    in vec3             wsL) {

    vec3 attLightColor = attenuation * lightColor;

#   if HAS_LAMBERTIAN_TERM
        I_lambertian += attLightColor;
#   endif
        
#   if HAS_GLOSSY_TERM
        if (glossyExponent > 0.0) {
            // cosine of the angle between the normal and the half-vector
            vec3 wsH = normalize(wsL + wsE);
            float cos_h = max(dot(wsH, wsN), 0.0);
            I_glossy += attLightColor * pow(cos_h, glossyExponent);
        }
#   endif
}


/**
 \param lightAttenuation OpenGL distance attenuation polynomial coefficients, with cosine of spotlight angle in the w component
 \param lightDirection   OpenGL spotlight direction
 */
void addLightContribution
   (in vec3             n, 
    in vec3             wsE,
    in vec3             wsPosition, 
    in float            glossyExponent, 
    in float            glossyCoefficient,
    in vec4             lightPosition, 
    in vec4             lightAttenuation,
    in vec3             lightLook, 
    in vec3             lightUpVector, 
    in vec3             lightRightVector, 
    in bool             lightRectangular,
    in vec3             lightColor, 
    inout vec3          I_lambertian,
    inout vec3          I_glossy, 
    out vec3            w_i) {

    float attenuation = computeAttenuation(n, lightPosition, lightAttenuation, wsPosition, lightLook, lightUpVector, lightRightVector, lightRectangular, w_i);

    if (attenuation >= attenuationThreshold) {
        computeShading(n, wsE, attenuation, glossyExponent, glossyCoefficient, lightColor, I_lambertian, I_glossy, w_i);
    }
}


void addShadowedLightContribution
   (in vec3             n, 
    in vec3             wsE,
    in vec3             wsPosition, 
    in float            glossyExponent,
    in float            glossyCoefficient,
    in vec4             lightPosition,
    in vec4             lightAttenuation,
    in vec3             lightLook,
    in vec3             lightUpVector, 
    in vec3             lightRightVector, 
    in bool             lightRectangular,
    in vec3             lightColor, 
    in vec4             shadowCoord,
    in sampler2DShadow  shadowMap,
    in vec2             invShadowMapSize,
    inout vec3          I_lambertian, 
    inout vec3          I_glossy, 
    out vec3            w_i) {

    float attenuation = computeAttenuation(n, lightPosition, lightAttenuation, wsPosition, lightLook, lightUpVector, lightRightVector, lightRectangular, w_i);
    
    if (attenuation >= attenuationThreshold) {
        // The following call assumes that attenuation is non-zero
        attenuation *= shadowMapVisibility(lightLook, lightPosition, lightAttenuation, shadowCoord, shadowMap, invShadowMapSize);

        if (attenuation < attenuationThreshold) {
            // No light due to shadowing
            return;
        }

        computeShading(n, wsE, attenuation, glossyExponent, glossyCoefficient, lightColor, I_lambertian, I_glossy, w_i);
    }
}

#endif
