// -*- c++ -*-
/** \file LocalLightingEnvironment/LocalLightingEnvironment_environmentMapUniforms.glsl */

#ifndef LocalLightingEnvironment_environmentMapUniforms_glsl
#define LocalLightingEnvironment_environmentMapUniforms_glsl

#expect NUM_ENVIRONMENT_MAPS "integer for number of environment maps to be blended"

#for (int i = 0; i < NUM_ENVIRONMENT_MAPS; ++i)
    /** The cube map with default OpenGL MIP levels */
    uniform samplerCube environmentMap$(i);

    /** Includes the weight for interpolation factors, the environment map's native scaling,
        and a factor of PI */
    uniform float       environmentMapScale$(i);

    /** log2(environmentMap.width * sqrt(3)) */
    uniform float       environmentMapGlossyMIPConstant$(i);
#endfor

#endif
