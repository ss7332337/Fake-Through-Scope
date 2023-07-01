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

float4 ToScreen(float4 ori)
{
	float4 result = mul(CameraRotation,ori);
	

	float4 ndcPosition = result / result.z;
	result = mul(projMat,result);
	
	// result.x = (ndcPosition.x + 1.0f) / 2.0f * BUFFER_WIDTH;   // X coordinate in pixels
	// result.y = (1.0f - ndcPosition.y) / 2.0f * BUFFER_HEIGHT;  // Y coordinate in pixels
	// result.z = ndcPosition.z;                                 // Z coordinate is the depth value
	// result.w = 1;

	return result;
}

float4 main(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
{
	float2 basePos = float2(0.5, 0.5) + ScopeEffect_OriPositionOffset * PixelSize;
	float2 texcoordOffset  = (basePos - FTS_ScreenPos * PixelSize) * camDepth;
	float2 texcoordOffsetinPixel  = texcoordOffset * rcp(PixelSize);
	
    float2 pos = texcoord / PixelSize;
	float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	float4 color = tBACKBUFFER.Sample(gSamLinear, texcoord + texcoordOffset);
	float2 areaCenter = ScopeEffect_Size * 0.5;

	float dx = pos.x  - screenSize.x / 2 + ScopeEffect_Offset.x;
	float dy = pos.y  - screenSize.y / 2 + ScopeEffect_Offset.y;
	float r = (ScopeEffect_Size.x)  * 0.5;

	float2 destTopLeft = FTS_ScreenPos - ScopeEffect_Size * 0.5 + ScopeEffect_Offset.x;
    float2 destBottomRight = FTS_ScreenPos + ScopeEffect_Size * 0.5 + ScopeEffect_Offset.y;

	bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y);

	float2 ReticlePos;
	float2 ReticleCoord;

	ReticleCoord = aspect_ratio_correction(texcoord);
	ReticleCoord = (ReticleCoord - FTS_ScreenPos * PixelSize) * 16 *rcp(ReticleSize) + FTS_ScreenPos * PixelSize;

	float4 ReticleColor = ReticleTex.Sample(gSamLinear,ReticleCoord);
	color = ReticleColor * ReticleColor.a + color * (1-ReticleColor.a);

	float4 colorFinal = color;
	// * isRender;
	colorFinal*= isRender * EnableMerge;
	if(abs(pos.x - FTS_ScreenPos.x) <=0.5  || abs(pos.y - FTS_ScreenPos.y) <=0.5)
		return float4(0,1,1,1); 

	return colorFinal;
	//return float4(screenSize * PixelSize,0,1);

}