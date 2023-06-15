#include "Triangle.hlsli"
#include "ScopeEffect_PS.h"

sampler BackBuffer;



float2 clampMagnitude(float2 v, float l)
{
	return normalize(v) * min(length(v), l);
}

/// <summary>
/// From CrookR and I modified a bit
/// </summary>
float getparallax(float d, float2 ds, float dfov)
{
	return clamp(1 - pow(abs(rcp(parallax_Radius * ds.y) * (parallax_relativeFogRadius * d * ds.y)), parallax_scopeSwayAmount), 0, parallax_maxTravel);
}

float4 NVGEffect(float4 color, float2 texcoord)
{
	float lum = dot(color.rgb, float3(0.30f, 0.59f, 0.11f));
	float3 nvColor = float3(0, lum * (-log(lum) + 1), 0);

	float LinesOn = 2;
	float LinesOff = 2;
	float OffIntensity = 0.5f;

	float intensity = texcoord.y * BUFFER_HEIGHT % (LinesOn + LinesOff);
	bool larger = intensity < LinesOn;
	intensity = 1.0f - larger * OffIntensity;

	return float4(saturate(nvColor.xyz * intensity * nvIntensity), 1);
}



float4 main(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
{
    float2 pos = texcoord / PixelSize;
	float4 color = tBACKBUFFER.Sample(gSamLinear, texcoord);

	float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	float2 areaCenter = ScopeEffect_Size * 0.5;
	float2 ScreenCenter = screenSize * 0.5 - areaCenter;

	float dx = pos.x - areaCenter.x - ScreenCenter.x;
	float dy = pos.y - areaCenter.y - ScreenCenter.y;
	float r = (ScopeEffect_Size.x)  * 0.5;

	bool isRender = dx * dx + dy * dy < r * r;

	
	//isRender
	float4 colorFinal = color;
	// * isRender;
	//float4 colorFinal = color;
	colorFinal*= isRender;
	//colorFinal.a*= EnableMerge;

	return colorFinal;
}

