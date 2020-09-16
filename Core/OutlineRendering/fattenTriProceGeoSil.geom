#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 30) out;

in vec3 toGsNormal[];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float edgeWidth;
void genTtri(vec3 va,vec3 vb,vec3 vc,mat4 vp)
{
    gl_Position=vp*vec4(va,1.0);
    EmitVertex();
    gl_Position=vp*vec4(vb,1.0);
    EmitVertex();
    gl_Position=vp*vec4(vc,1.0);
    EmitVertex();
    EndPrimitive();
}
void main()
{
    mat4 mvp=projection*view;

    vec3 v0=vec3(model*gl_in[0].gl_Position);
    vec3 v1=vec3(model*gl_in[1].gl_Position);
    vec3 v2=vec3(model*gl_in[2].gl_Position);

    vec3 E0=normalize(v1-v0);
    vec3 E1=normalize(v2-v1);
    vec3 E2=normalize(v0-v2);

    vec3 normal=mat3(transpose(inverse(model)))*toGsNormal[0];

    vec3 verticalE0=normalize(cross(E0,normal))*edgeWidth;
    vec3 verticalE1=normalize(cross(E1,normal))*edgeWidth;
    vec3 verticalE2=normalize(cross(E2,normal))*edgeWidth;

    vec3 v00=v0+verticalE2;
    vec3 v01=v0+verticalE0;
    
    vec3 v10=v1+verticalE0;
    vec3 v11=v1+verticalE1;

    vec3 v20=v2+verticalE1;
    vec3 v21=v2+verticalE2;

    genTtri(v0,v1,v2,mvp);

    genTtri(v0,v00,v01,mvp);
    genTtri(v1,v10,v11,mvp);
    genTtri(v2,v20,v21,mvp);

    genTtri(v0,v01,v1,mvp);
    genTtri(v1,v01,v10,mvp);

    genTtri(v1,v11,v2,mvp);
    genTtri(v2,v11,v20,mvp);

    genTtri(v2,v21,v0,mvp);
    genTtri(v0,v21,v00,mvp);

}