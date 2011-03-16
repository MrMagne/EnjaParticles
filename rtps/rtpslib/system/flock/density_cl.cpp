#include "cl_structs.h"

float magnitude(float4 vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}
float dist_squared(float4 vec)
{
    return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
}


__kernel void density(__global float4* pos, __global float* density, __constant struct FLOCKParams* params)
//, __global float4* error)
{
    unsigned int i = get_global_id(0);
    //int num = get_global_size(0);
    int num = params->num;
    if(i > num) return;

    float h = params->smoothing_distance;
    //stuff from Tim's code (need to match #s to papers)
    //float alpha = 315.f/208.f/params->PI/h/h/h;
    float h9 = h*h*h * h*h*h * h*h*h;
    float alpha = 315.f / (64.0f*params->PI*h9);

    float4 p = pos[i] * params->simulation_scale;
    density[i] = 0.0f;

    //super slow way, we need to use grid + sort method to get nearest neighbors
    //this code should never see the light of day on a GPU... just sayin
    for(int j = 0; j < num; j++)
    {
        if(j == i) continue;
        float4 pj = pos[j] * params->simulation_scale;
        float4 r = p - pj;
        //error[i] = r;
        float rlen = magnitude(r);
        if(rlen < h)
        {
            float r2 = dist_squared(r);
            float re2 = h*h;
            if(r2/re2 <= 4.f)
            {
                //float R = sqrt(r2/re2);
                //float Wij = alpha*(2.f/3.f - 9.f*R*R/8.f + 19.f*R*R*R/24.f - 5.f*R*R*R*R/32.f);
                float hr2 = (h*h - r2);
                float Wij = alpha * hr2*hr2*hr2;
                density[i] += params->mass * Wij;
            }
        }
    }

}