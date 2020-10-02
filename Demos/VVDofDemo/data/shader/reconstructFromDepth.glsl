/**
 \file reconstructFromDepth.glsl
 \author Morgan McGuire, NVIDIA Research

 Routines for reconstructing linear Z, camera-space position, and camera-space face normals from a standard
 or infinite OpenGL projection matrix from G3D.
 */

#ifndef reconstructFromDepth_glsl
#define reconstructFromDepth_glsl

// Note that positions (which may affect z) are snapped during rasterization, but 
// attributes are not.

/* 
 Clipping plane constants for use by reconstructZ

 \param clipInfo = (z_f == -inf()) ? Vector3(z_n, -1.0f, 1.0f) : Vector3(z_n * z_f,  z_n - z_f,  z_f);
 \sa G3D::Projection::reconstructFromDepthClipInfo
*/
float reconstructCSZ(float d, vec3 clipInfo) {
    return clipInfo[0] / (clipInfo[1] * d + clipInfo[2]);
}


/** Reconstruct camera-space P.xyz from screen-space S = (x, y) in
    pixels and camera-space z < 0.  Assumes that the upper-left pixel center
    is at (0.5, 0.5) [but that need not be the location at which the sample tap 
    was placed!]

    Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.

 projInfo = vec4(-2.0f / (width*P[0][0]), 
          -2.0f / (height*P[1][1]),
          ( 1.0f - P[0][2]) / P[0][0], 
          ( 1.0f + P[1][2]) / P[1][1])
    
    where P is the projection matrix that maps camera space points 
    to [-1, 1] x [-1, 1].  That is, Camera::getProjectUnit().

    \sa G3D::Projection::reconstructFromDepthProjInfo
*/
vec3 reconstructCSPosition(vec2 S, float z, vec4 projInfo) {
    return vec3((S.xy * projInfo.xy + projInfo.zw) * z, z);
}

/** Reconstructs screen-space unit normal from screen-space position */
vec3 reconstructCSFaceNormal(vec3 C) {
    return normalize(cross(dFdy(C), dFdx(C)));
}

vec3 reconstructNonUnitCSFaceNormal(vec3 C) {
    return cross(dFdy(C), dFdx(C));
}


/**
 \brief Cast a ray against the surface defined by the depth buffer by ray marching.

 \param cameraAndProjectionMatrix  gl_ProjectionMatrix * mat4(g3d_WorldToCameraMatrix)
 \param clipInfo See reconstructCSZ,  G3D::Projection::reconstructFromDepthClipInfo()
 \param projInfo See reconstructCSPosition, G3D::Projection::reconstructFromDepthProjInfo()
 \param distance The maximum distance to trace.  On return, the actual distance traced
 \param hitTexCoord The [0, 1] texture coordinate in the depth buffer of the hit location
 \param jitterFraction A number between 0 and 1 used to offset the ray randomly
 \param depthIsCSZ If true, treat the depth buffer as a camera-space Z buffer, which 
        is much faster for the ray cast.  If this is a compile-time constant, there should be no cost 
        for the branch or conversion cost at runtime.

 \return True if something was hit, false otherwise

 \cite Based on Tiago Sousa, Nick Kasyan, Nicolas Schulz, Secrets of CryENGINE 3 Graphics Technology, 
    SIGGRAPH 2011 Talk, August 29, 2011, http://www.crytek.com/download/S2011_SecretsCryENGINE3Tech.ppt
*/
bool castRayAgainstDepthBuffer2
   (vec3            wsOrigin, 
    vec3            wsDirection,
    vec3            wsCameraPos,
    vec3            wsCameraZAxis,
    mat4            cameraAndProjectionMatrix,
    sampler2D       depthBuffer,
    vec3            clipInfo,
    vec4            projInfo,
    float           jitterFraction,
    int             numSteps,
    inout float     distance,
    out vec2        hitTexCoord,
    bool            depthBufferIsCSZ) {

    // TODO: Pass actual guard band
    float GUARD_BAND_FRACTION_X = 0.0;
    float GUARD_BAND_FRACTION_Y = 0.0;

    // Current point on the reflection ray in world space
    vec3 P = wsOrigin;

    // Position of the camera along its own Z axis relative to the origin.
    float cameraZ = dot(wsCameraZAxis, wsCameraPos);

    // rayOrigin's camera-space Z value
    float rayOriginZ = dot(wsCameraZAxis, wsOrigin) - cameraZ;

    // Same as rayOriginZ, but this one increments with every step
    float csPZ = rayOriginZ;

    // World space distance for each ray-march step
    float step = distance / numSteps;

    // Amount that csPz increments by for every step
    float csPZInc = dot(wsCameraZAxis, wsDirection * step);

    // Amount that P increments by for every step
    vec3  PInc    = wsDirection * step;

    P    += PInc    * jitterFraction;
    csPZ += csPZInc * jitterFraction;

    // Fetch the camera-space Z value at the texture-coordinate location ts
#   define FETCH_CSZ(ts) (depthBufferIsCSZ ? texture2D(depthBuffer, (ts)).x : reconstructCSZ(texture2D(depthBuffer, (ts)).x, clipInfo))

    for (int s = 0; s < numSteps; ++s) {
        // Project the trace point P into texture space.  Note that the projection
        // matrix maps to [0, 1] coordinates naturally
        vec4 temp = cameraAndProjectionMatrix * vec4(P, 1.0);

        // Homogeneous division and half-pixel offset
        vec2 tsP = (vec2(temp.x, temp.y) / temp.w) * 0.5 + vec2(0.5);
        
        // Expected error in depth buffer reconstruction (very conservative)
        const float epsilon = 0.015;

        // Camera space depth of the background
        float backgroundCSZ = FETCH_CSZ(tsP) - epsilon;

        // If the point on the ray is behind the scene point at the same location...
        // (As an optimization, break out of the loop here but don't handle the result until outside the loop)
        if ((csPZ <= backgroundCSZ)
            
            // Not on skybox
            && (backgroundCSZ > -99999)

            // ...and the background point is farther from the camera than the ray origin 
            // (good for floors and prevents some bleeding of foreground objects, but does not 
            // work well for rays towards the camera!)
            //&& (backgroundCSZ <= rayOriginZ + epsilon * 10)

            // ...and the slope to the background point is close to the one that we wanted
             && ((abs((backgroundCSZ - rayOriginZ) - csPZInc * (s + jitterFraction)) < s * 0.25))
            ) {

            // Hit...or off screen
            hitTexCoord = tsP;
            distance = (s + jitterFraction) * step;

            return (hitTexCoord.y >= GUARD_BAND_FRACTION_Y) && (hitTexCoord.x >= GUARD_BAND_FRACTION_X) && 
                   (hitTexCoord.x <= 1.0 - GUARD_BAND_FRACTION_X) && (hitTexCoord.y <= 1.0 - GUARD_BAND_FRACTION_Y);
        }

        P    += PInc;
        csPZ += csPZInc;
    }
    
#   undef FETCH_CSZ

    // Miss
    return false;
}

#endif
