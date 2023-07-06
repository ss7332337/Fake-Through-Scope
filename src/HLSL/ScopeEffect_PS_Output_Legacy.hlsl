#include "Triangle.hlsli"

Texture2D outPutRT : register(t6);

///fov 兼容
float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    float4 color = outPutRT.Sample(gSamLinear ,texcoord);
    return color;
}