#include "Triangle.hlsli"

Texture2D outPutRT : register(t6);

///fov 兼容

float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    float2 adjTex = texcoord - float2(0.5,0.5);
    adjTex.y *= GetAspectRatio();
    adjTex *=  rcp(2.0F);
    adjTex += float2(0.5,0.5);
    float2 pos = adjTex / PixelSize;
    float2 FTS_ScreenPosInPixel = FTS_ScreenPos* PixelSize;

    float2 basePos = float2(0.5, 0.5) + (ScopeEffect_OriPositionOffset);
	float2 texcoordOffset  = (basePos - FTS_ScreenPosInPixel) * camDepth;
    float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);

    float2 ScopeEffect_OffsetA = (ScopeEffect_Offset);
    float2 ScopeEffect_SizeA = (ScopeEffect_Size);

    float4 color = outPutRT.Sample(gSamLinear ,adjTex + texcoordOffset);
    float dx = texcoord.x  - 0.5 - ScopeEffect_OffsetA.x;
	float dy = texcoord.y  - 0.5 + ScopeEffect_OffsetA.y;
	float r = (ScopeEffect_SizeA.x)  * 0.5;

    // float2 destTopLeft = screenSize * 0.5 - ScopeEffect_Size * 0.5;
    // float2 destBottomRight = screenSize * 0.5 + ScopeEffect_Size * 0.5;
	float2 destTopLeft = 0.5f - ScopeEffect_SizeA.x * 0.5 - ScopeEffect_OffsetA.x;
    float2 destBottomRight = 0.5f + ScopeEffect_SizeA.y * 0.5 + ScopeEffect_OffsetA.y;

    float4 rect = ScopeEffect_Rect + ScopeEffect_OffsetA.xyxy;
	bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (texcoord.x >= rect.x && texcoord.y >= rect.y && texcoord.x <= rect.z && texcoord.y <= rect.w);

    // if(abs(texcoord.x - FTS_ScreenPosInPixel.x) < 0.002 || abs(texcoord.y - FTS_ScreenPosInPixel.y)<0.002)
    // {
    //     return float4(0,1,1,1);
    // }

    // if(abs(texcoord.x - 0.5) < 0.001 || abs(texcoord.y - 0.5)<0.001)
    // {
    //     return float4(1,0,0,1);
    // }


    return color * isRender * EnableMerge;
   
}

// float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
// {
//     float2 adjTex = texcoord - float2(0.5,0.5);
//     adjTex.y *= GetAspectRatio();
//     adjTex *=  rcp(2.0F);
//     adjTex += float2(0.5,0.5);
//     float2 pos = adjTex / PixelSize;

//     float2 basePos = float2(0.5, 0.5) + ScopeEffect_OriPositionOffset * PixelSize;
// 	float2 texcoordOffset  = (basePos - FTS_ScreenPos * PixelSize) * camDepth;
// 	float2 texcoordOffsetinPixel  = texcoordOffset * rcp(PixelSize);
//     float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
// 	//float2 ftsMov = FTS_ScreenPos * PixelSize;

//     float4 color = outPutRT.Sample(gSamLinear ,adjTex + texcoordOffset);
//     float dx = pos.x  - screenSize.x / 2 + ScopeEffect_Offset.x;
// 	float dy = pos.y  - screenSize.y / 2 + ScopeEffect_Offset.y;
// 	float r = (ScopeEffect_Size.x)  * 0.5;

// 	float2 destTopLeft = FTS_ScreenPos - ScopeEffect_Size * 0.5 + ScopeEffect_Offset.x;
//     float2 destBottomRight = FTS_ScreenPos + ScopeEffect_Size * 0.5 + ScopeEffect_Offset.y;

// 	bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y);

    
//     return color * isRender * EnableMerge;
//     //return float4(abseyeDirectionLerpInS.xy*GetPixelSize(),0,1);
// }
