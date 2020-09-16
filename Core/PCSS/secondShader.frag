#version 330 core
out vec4 FragColor;

in  vec3 FragPos;
in  vec3 Normal;
in  vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D s_ShadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int u_BlockerSearchSamples;
uniform int u_PCFSamples;

uniform mat4 light_view_proj;
uniform mat4 light_view;

uniform float u_LightBias;
uniform float u_BlockerSearchScale;
uniform float u_LightSize;
uniform float u_LightNear;
uniform float u_LightFar;

uniform vec2 Poisson4[4];
uniform vec2 Poisson25[25];
uniform vec2 Poisson32[32];
uniform vec2 Poisson64[64];
uniform vec2 Poisson100[100];
uniform vec2 Poisson128[128];

float pcf_poisson_filter(vec2 uv, float z0, float bias, float filter_radius_uv)
{
    float sum = 0.0;

    for (int i = 0; i < u_PCFSamples; ++i)
    {
        vec2 offset;

        if (u_PCFSamples == 25)
            offset = Poisson25[i];
        else if (u_PCFSamples == 32)
            offset = Poisson32[i];
        else if (u_PCFSamples == 64)
            offset = Poisson64[i];
        else if (u_PCFSamples == 100)
            offset = Poisson100[i];
        else if (u_PCFSamples == 128)
            offset = Poisson128[i];
        else if (u_PCFSamples == 4)
            offset = Poisson4[i];

        offset *= filter_radius_uv;
        float shadow_map_depth = texture(s_ShadowMap, uv + offset).r;
        sum += shadow_map_depth < (z0 - bias) ? 0.0 : 1.0;
    }

    return sum / float(u_PCFSamples);
}

void find_blocker(out float accum_blocker_depth,
                  out float num_blockers,
                  vec2      uv,
                  float     z0,
                  float     bias,
                  float     search_region_radius_uv)
{
    accum_blocker_depth = 0.0;
    num_blockers        = 0.0;
    float biased_depth  = z0 - bias;

    for (int i = 0; i < u_BlockerSearchSamples; ++i)
    {
        vec2 offset;

        if (u_BlockerSearchSamples == 25)
            offset = Poisson25[i];
        else if (u_BlockerSearchSamples == 32)
            offset = Poisson32[i];
        else if (u_BlockerSearchSamples == 64)
            offset = Poisson64[i];
        else if (u_BlockerSearchSamples == 100)
            offset = Poisson100[i];
        else if (u_BlockerSearchSamples == 128)
            offset = Poisson128[i];
        else if (u_BlockerSearchSamples == 4)
            offset = Poisson4[i];

        offset *= search_region_radius_uv;
        float shadow_map_depth = texture(s_ShadowMap, uv + offset).r;

        if (shadow_map_depth < biased_depth)
        {
            accum_blocker_depth += shadow_map_depth;
            num_blockers++;
        }
    }
}
float pcss_filter(vec2 uv, float z, float bias, float z_vs)
{
    // ------------------------
    // STEP 1: blocker search
    // ------------------------
    float accum_blocker_depth, num_blockers;
    // --- --lightsize--  ---
    //  |   \        /     |lightNear
    //  z   --radius--    ---this is shadow map plane
    //  |     \    /
    //  |      \  /
    // ---      \/
    float search_region_radius_uv=u_BlockerSearchScale * u_LightSize * (z_vs - u_LightNear) / z_vs;
    find_blocker(accum_blocker_depth, num_blockers, uv, z, bias, search_region_radius_uv);
    

    // Early out if not in the penumbra
    if (num_blockers == 0.0)
    {
        return 1.0;
    }   

    if(num_blockers==u_BlockerSearchSamples)
    {
        return 0.0;
    }
    
    float avg_blocker_depth = accum_blocker_depth / num_blockers;


    // ------------------------
    // STEP 2: penumbra size
    // ------------------------
    float avg_blocker_depth_vs = u_LightNear + (u_LightFar - u_LightNear) * avg_blocker_depth;   
    float penumbra_radius      = abs(z_vs - avg_blocker_depth_vs) / avg_blocker_depth_vs;  
    float filter_radius        =penumbra_radius * u_LightSize * u_LightNear / z_vs;

    // ------------------------
    // STEP 3: filtering
    // ------------------------
    return pcf_poisson_filter(uv, z, bias, filter_radius);
}
float shadow_occlussion(vec3 p)
{
    // Transform frag position into Light-space.
    vec4 light_space_pos = light_view_proj * vec4(p, 1.0);

    vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    // transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // check whether current frag pos is in shadow
    float bias = u_LightBias;

    vec4 pos_vs = light_view * vec4(p, 1.0);
    pos_vs.xyz /= pos_vs.w;

    if (proj_coords.x > 1.0 || proj_coords.y > 1.0 || proj_coords.z > 1.0 || proj_coords.x < 0.0 || proj_coords.y < 0.0 || proj_coords.z < 0.0)
        return 1.0;

    return pcss_filter(proj_coords.xy, current_depth, bias, -(pos_vs.z));
}

void main()
{           
    vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = shadow_occlussion(FragPos);                    
    vec3 lighting = (ambient + (shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}