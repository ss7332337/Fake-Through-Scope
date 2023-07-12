#include "Triangle.hlsli"

Texture2D outPutRT : register(t6);


// float4 main(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
// {
// 	float2 pos = texcoord / PixelSize;

// 	float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
// 	float2 standredSize = float2(1920,1080);

// 	float2 diffSize = screenSize*rcp(standredSize);
// 	float2 areaCenter = ScopeEffect_Size;
// 	float2 ScreenCenter = screenSize * 0.5 - areaCenter;
	
// 	float dx = pos.x - areaCenter.x - ScreenCenter.x;
// 	float dy = pos.y - areaCenter.y - ScreenCenter.y;
// 	float r = (ScopeEffect_Size.x)  * 0.5 * diffSize.x;
// 	bool isRender = dx * dx + dy * dy < r * r;

// 	if(!isRender) discard;

// 	return float4(0,1,1,1);
// }

///fov 兼容
float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    float4 color = outPutRT.Sample(gSamLinear ,texcoord);
    return color;
}