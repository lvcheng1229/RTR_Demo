// -*- c++ -*-
/** \file LocalLightingEnvironment/LocalLightingEnvironment_LightUniforms.glsl */

#ifndef LocalLightingEnvironment_LightUniforms_glsl
#define LocalLightingEnvironment_LightUniforms_glsl

#expect NUM_LIGHTS "Integer number of direct light sources (and shadow maps)"

#for (int I = 0; I < NUM_LIGHTS; ++I) 
    /** Modelview projection matrix used for the light's shadow map */
    uniform mat4        lightMVP$(I);

    /** In world units */
    uniform float       shadowMapBias$(I);

    /** World space light position */
    uniform vec4        lightPosition$(I);

    /** Power of the light */
    uniform vec3        lightColor$(I);

    /** Spot light facing direction (unit length) */
    uniform vec3        lightDirection$(I);

    /** w element is the spotlight cutoff angle.*/
    uniform vec4        lightAttenuation$(I);

    /** Is this light's field of view rectangular? */
    uniform bool        lightRectangular$(I);

    uniform vec3        lightUpVector$(I);

    uniform vec3        lightRightVector$(I);

#   if castsShadows$(I)
        uniform sampler2DShadow shadowMap$(I);
        uniform vec2            shadowMapInvSize$(I);
#   endif
#endfor

#endif
