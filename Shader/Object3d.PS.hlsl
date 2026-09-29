#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t lightType;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

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
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float32_t3 reflectLight = reflect(gDirectionalLight.directiom, normalize(input.normal));
    
    if (textureColor.a <= 0.0f)
    {
        discard;
    }
    
    if (output.color.a == 0.0f)
    {
        discard;
    }
    
    switch (gMaterial.lightType)
    {
        case 0:
            output.color = gMaterial.color * textureColor;
            break;
        
        case 1:
        {
                float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
                float cos = saturate(NdotL);
                output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                output.color.a = gMaterial.color.a * textureColor.a;
                float RdotE = dot(reflectLight, toEye);
                float specularPow = pow(saturate(RdotE), gMaterial.shininess);
                break;
            }
        case 2:
        {
                float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
                float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                
                float RdotE = dot(reflectLight, toEye);
                float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            
                float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
                output.color.rgb = diffuse + specular;
            
                output.color.a = gMaterial.color.a * textureColor.a;
                break;
            }
    }
    
    return output;
}