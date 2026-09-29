#include "Object3d.hlsli"

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct TransformationMatrix
{
    float32_t4x4 wvp;
    float32_t4x4 world;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.wvp);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.world));
    output.worldPosition = mul(input.position, gTransformationMatrix.world).xyz;
    return output;
}