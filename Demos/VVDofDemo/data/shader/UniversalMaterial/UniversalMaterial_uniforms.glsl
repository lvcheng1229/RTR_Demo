// -*- c++ -*-
/** \file UniversalMaterial/UniversalMaterial_uniforms.glsl */

#ifndef UniversalMaterial_uniforms_glsl
#define UniversalMaterial_uniforms_glsl

#expect NUM_LIGHTMAP_DIRECTIONS "0, 1, or 3"

#ifdef NORMALBUMPMAP
#   expect PARALLAXSTEPS "int"
#   if (PARALLAXSTEPS == 0)
#       define bumpMap bumpMapBlinn78
#   elif (PARALLAXSTEPS <= 1)
#       define bumpMap bumpMapWelsh04
#   else // PARALLAXSTEPS > 1
#       define bumpMap bumpMapTatarchuk06
#   endif

    /** xyz = normal, w = bump height */
    uniform sampler2D   normalBumpMap;

#   if (PARALLAXSTEPS > 0)
        /** Multiplier for bump map.  Typically on the range [0, 0.05]
          This increases with texture scale and bump height. */
        uniform float       bumpMapScale;

        /** How high is the 0-level of the bump map? On the range [0, 1] */
        uniform float       bumpMapBias;
#   endif    
#endif


#if NUM_LIGHTMAP_DIRECTIONS > 0
    uniform float lightMapRadianceScale;
    
    // Only the first constant is used, by design
    uniform float lightMapConstant;

    uniform sampler2D lightMap0;
#   if NUM_LIGHTMAP_DIRECTIONS == 3
#       ifdef NORMALBUMPMAP
#           include <lightMap.glsl>
#       endif
        uniform sampler2D lightMap1;
        uniform sampler2D lightMap2;
#   endif
#endif


#foreach (NAME, name, i) in (GLOSSY, glossy, 4), (LAMBERTIAN, lambertian, 4), (EMISSIVE, emissive, 3), (TRANSMISSIVE, transmissive, 4), (CUSTOM, custom, 4)
#   ifdef $(NAME)CONSTANT
        uniform vec$(i)     $(name)Constant;
#   endif

#   ifdef $(NAME)MAP
        uniform sampler2D   $(name)Map;
#   endif
#endforeach

#if defined(EMISSIVECONSTANT) || defined(EMISSIVEMAP)
    uniform float       emissiveRadianceScale;
#endif

// Macros used by Light.glsl
#if defined(GLOSSYCONSTANT) || defined(GLOSSYMAP)
#   define HAS_GLOSSY_TERM 1
#else
#   define HAS_GLOSSY_TERM 0
#endif

#if defined(LAMBERTIANCONSTANT) || defined(LAMBERTIANMAP)
#   define HAS_LAMBERTIAN_TERM 1
#else
#   define HAS_LAMBERTIAN_TERM 0
#endif

#endif
