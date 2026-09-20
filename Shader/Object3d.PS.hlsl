#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t lightType;
    float32_t4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float32_t4(input.texCoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    PixelShaderOutput output;
    switch (gMaterial.lightType)
    {
        case 0:
            output.color = gMaterial.color * textureColor;
            break;
        
        case 1:
        {
            
                float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
                float cos = saturate(NdotL);
                output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
                break;
            }
        case 2:
        {
                float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
                float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
                break;
            }
    }
    
    return output;
}