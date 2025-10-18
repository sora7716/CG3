#include "Sprite.hlsli"

struct TransformationMatrix {
    float4x4 wvp;
    float4x4 world;
};
ConstantBuffer<TransformationMatrix> gTrasformationMatrix : register(b0);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(input.position, gTrasformationMatrix.wvp);
    output.texcoord = input.texcoord;
    output.normal = output.normal;
    return output;
}