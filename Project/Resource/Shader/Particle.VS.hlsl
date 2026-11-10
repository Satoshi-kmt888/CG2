#include "Particle.hlsli"

struct ParticleForGPU
{
    float32_t4x4 wvp;
    float32_t4x4 world;
    float32_t4 color;
};
StructuredBuffer<ParticleForGPU> gParticle : register(t0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instance_id : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticle[instance_id].wvp);
    output.texCoord = input.texCoord;
    output.color = gParticle[instance_id].color;
    return output;
}