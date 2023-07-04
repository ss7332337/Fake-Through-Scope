#include "Triangle.hlsli"

Texture2D outPutRT : register(t6);

///fov 兼容
float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    float2 adjTex = texcoord - float2(0.5,0.5);
    adjTex.y *= GetAspectRatio();
    adjTex *=  rcp(4.0F);
    adjTex += float2(0.5,0.5);
    float2 pos = adjTex / PixelSize;

    float2 basePos = float2(0.5, 0.5) + ScopeEffect_OriPositionOffset * PixelSize;
	float2 texcoordOffset  = (basePos - FTS_ScreenPos * PixelSize) * camDepth;
	float2 texcoordOffsetinPixel  = texcoordOffset * rcp(PixelSize);
    float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	//float2 ftsMov = FTS_ScreenPos * PixelSize;

    float4 color = outPutRT.Sample(gSamLinear ,adjTex + texcoordOffset);
    float dx = pos.x  - screenSize.x / 2 + ScopeEffect_Offset.x;
	float dy = pos.y  - screenSize.y / 2 + ScopeEffect_Offset.y;
	float r = (ScopeEffect_Size.x)  * 0.5;

	float2 destTopLeft = FTS_ScreenPos - ScopeEffect_Size * 0.5 + ScopeEffect_Offset.x;
    float2 destBottomRight = FTS_ScreenPos + ScopeEffect_Size * 0.5 + ScopeEffect_Offset.y;

	bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y);

    
    return color * isRender * EnableMerge;
    //return float4(abseyeDirectionLerpInS.xy*GetPixelSize(),0,1);
}