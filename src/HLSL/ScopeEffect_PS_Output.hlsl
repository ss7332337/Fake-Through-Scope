#include "Triangle.hlsli"

Texture2D outPutRT : register(t6);


float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    // 使用Sample方法来从纹理中获取颜色值
    float2 pos = texcoord / PixelSize;
	//float2 ftsMov = FTS_ScreenPos * PixelSize;

    float2 adjTex = texcoord - float2(0.5,0.5);
    adjTex.y *= GetAspectRatio();
    adjTex *=  rcp(4.0F);
    adjTex += float2(0.5,0.5);

    float4 color = outPutRT.Sample(gSamLinear ,adjTex);

    return color;
}