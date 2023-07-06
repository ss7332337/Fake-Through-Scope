#include "Triangle.hlsli"

sampler BackBuffer;



float4 main(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
{
	 float2 basePos = float2(0.5, 0.5) + ScopeEffect_OriPositionOffset * PixelSize;
	// float2 texcoordOffset  = (basePos - FTS_ScreenPos * PixelSize) * camDepth;
	// float2 texcoordOffsetinPixel  = texcoordOffset * rcp(PixelSize);
	
    // float2 pos = texcoord / PixelSize;
	// float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	
	// float2 areaCenter = ScopeEffect_Size * 0.5;

	// float dx = pos.x  - screenSize.x / 2 + ScopeEffect_Offset.x;
	// float dy = pos.y  - screenSize.y / 2 + ScopeEffect_Offset.y;
	// float r = (ScopeEffect_Size.x)  * 0.5;

	// float2 destTopLeft = FTS_ScreenPos - ScopeEffect_Size * 0.5 + ScopeEffect_Offset.x;
    // float2 destBottomRight = FTS_ScreenPos + ScopeEffect_Size * 0.5 + ScopeEffect_Offset.y;

	// bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y);

	// float2 ReticlePos;
	// float2 ReticleCoord;

	float4 abseyeDirectionLerp = mul(float4(eyeDirectionLerp,1),CameraRotation);
	if (abseyeDirectionLerp.y < 0 && abseyeDirectionLerp.y >= -0.001)
		abseyeDirectionLerp.y = -0.001;
	else if (abseyeDirectionLerp.y >= 0 && abseyeDirectionLerp.y <= 0.001)
		abseyeDirectionLerp.y = 0.001;

    float2 eye_velocity = clampMagnitude(abseyeDirectionLerp.xy , 2);

    float2 FTS_ScreenPosInPixel = FTS_ScreenPos * PixelSize;
	float2 adjTex = aspect_ratio_correction(texcoord);
	float2 mulTex = (texcoord - float2(0.5,0.5)) * rcp(ScopeEffect_Zoom) + float2(0.5,0.5);
	float4 color = tBACKBUFFER.Sample(gSamLinear, mulTex);
	
    float2 parallax_offset = float2(0.5 + eye_velocity.x  , 0.5 - eye_velocity.y);
	float distToParallax = distance(adjTex, parallax_offset);
	float2 scope_center = float2(0.5,0.5) + ScopeEffect_Offset * PixelSize;
	float distToCenter = distance(adjTex, scope_center);

	float2 ReticleCoord = aspect_ratio_correction(texcoord);
	ReticleCoord = ((ReticleCoord - float2(0.5,0.5)) * 16 *rcp(ReticleSize)) + float2(0.5,0.5);

	float4 ReticleColor = ReticleTex.Sample(gSamLinear,ReticleCoord);
	color = ReticleColor * ReticleColor.a + color * (1-ReticleColor.a);

	color.rgb *=   (step(distToCenter, 2) * getparallax(distToParallax,float2(1,1),1));
	float4 colorFinal = color;
	// // * isRender;
	// colorFinal*= isRender * EnableMerge;

	return colorFinal;
	//return float4(screenSize * PixelSize,0,1);

}
