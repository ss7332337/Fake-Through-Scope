#include "Triangle.hlsli"

sampler BackBuffer;

float4 mainq(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
{
	float2 pos = vpos.xy;

	float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	float2 standredSize = float2(1920,1080);

	float2 diffSize = screenSize*rcp(standredSize);
	float2 areaCenter = ScopeEffect_Size;
	float2 ScreenCenter = screenSize * 0.5 - areaCenter + ScopeEffect_Offset;

	float dx = pos.x - areaCenter.x - ScreenCenter.x;
	float dy = pos.y - areaCenter.y - ScreenCenter.y;
	float r = (ScopeEffect_Size.x)  * 0.5 * diffSize.x;
	bool isRender = dx * dx + dy * dy < r * r;

	return float4(0,1,1,1) * isRender;
}

float4 main(float4 vpos : SV_Position, float2 texcoord : TEXCOORD0) : SV_Target 
{
	float2 pos = texcoord / PixelSize;

	float2 screenSize = float2(BUFFER_WIDTH,BUFFER_HEIGHT);
	float2 standredSize = float2(1920,1080);
	float2 diffSize = screenSize*rcp(standredSize);

	float baseFov = 90;
	float fovDiff = 1;

	float4 color = tBACKBUFFER.Sample(gSamLinear, texcoord);
	float2 corrected_texturecoords = aspect_ratio_correction(texcoord);
	float2 texcoordCorrected = corrected_texturecoords;
	float4 ViewDir = normalize(mul(CameraRotation,float4(eyeDirection,1)));

	float2 ScopeOffset = ViewDir.xy;
	ScopeOffset *= rcp(camDepth);
	ScopeOffset.y *= -AspectRatio;
	float4 abseyeDirectionLerp = mul(CameraRotation,float4(eyeDirectionLerp,1));
	if (abseyeDirectionLerp.y < 0 && abseyeDirectionLerp.y >= -0.001)
		abseyeDirectionLerp.y = -0.001;
	else if (abseyeDirectionLerp.y >= 0 && abseyeDirectionLerp.y <= 0.001)
		abseyeDirectionLerp.y = 0.001;

	// float4 abseyeTranslationLerp = mul(CameraRotation,float4(eyeTranslationLerp,1));
	// if (abseyeTranslationLerp.y < 0 && abseyeTranslationLerp.y >= -0.0025)
	// 	abseyeTranslationLerp.y = -0.0025;
	// else if (abseyeTranslationLerp.y >= 0 && abseyeTranslationLerp.y <= 0.0025)
	// 	abseyeTranslationLerp.y = 0.0025;
	
	float2 eye_velocity = clampMagnitude(abseyeDirectionLerp.xy , 2);

	float2 parallax_offset = float2(0.5f + eye_velocity.x , 0.5f - eye_velocity.y);
	float distToParallax = distance(corrected_texturecoords, parallax_offset);
	float2 scope_center = float2(0.5f, 0.5f) + ScopeOffset + ScopeEffect_Offset * PixelSize * diffSize;
	float distToCenter = distance(corrected_texturecoords, scope_center);

	float4 colorBackup = color;
	float2 areaCenter = ScopeEffect_Size * 0.5 * rcp(fovDiff);
	
	float2 ScreenCenter = screenSize * 0.5 - areaCenter + ScopeOffset * rcp(PixelSize) + ScopeEffect_Offset * diffSize;
	float distanceWeap = 0.5F * sqrt(
	  pow(CurrWeaponPos.x - CurrRootPos.x, 2) 
	+ pow(CurrWeaponPos.y - CurrRootPos.y, 2) 
	+ pow(CurrWeaponPos.z - CurrRootPos.z, 2)
	//pow(CurrWeaponPos.z - CurrRootPos.z, 2)
	);

	float zMove = abs(sqrt((BaseWeaponPos)) - distanceWeap) + 1;
	float zMoveDiff = (1- EnableZMove * MovePercentage)+ EnableZMove * MovePercentage * rcp(zMove);
	if(step (zMoveDiff,0.5) == 1) zMoveDiff = 0.5;
	if(step(zMoveDiff,2) == 0) zMoveDiff = 1.5;

	float dx = pos.x - areaCenter.x - ScreenCenter.x;
	float dy = pos.y - areaCenter.y - ScreenCenter.y;
	float r = (ScopeEffect_Size.x)  * 0.5 * diffSize.x * rcp(fovDiff) * rcp(zMoveDiff);

	float2 destTopLeft = screenSize * 0.5 - ScopeEffect_Size * 0.5;
    float2 destBottomRight = screenSize * 0.5 + ScopeEffect_Size * 0.5;
	destTopLeft *= diffSize.x * rcp( fovDiff) * rcp(zMoveDiff);
    destBottomRight *=diffSize.y * rcp( fovDiff) * rcp( zMoveDiff);


	bool isRender = isCircle ? (dx * dx + dy * dy < r * r) : (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y);
	//bool isRender =  distance(texcoord,float2(0,0)) >= 0.15F;

	float2 ReticlePos;
	float2 ReticleCoord;

	ReticlePos.x = (pos.x - ScreenCenter.x - areaCenter.x) * 16 *rcp(ReticleSize) * AspectRatio + ScreenCenter.x + areaCenter.x;
	ReticlePos.y = (pos.y - ScreenCenter.y - areaCenter.y) * 16 *rcp(ReticleSize)  + ScreenCenter.y + areaCenter.y;
	ReticleCoord = ReticlePos * PixelSize;

	float4 ReticleColor = ReticleTex.Sample(gSamLinear,ReticleCoord);

	float2 destPos = (pos.xy - ScreenCenter - areaCenter 
	) *rcp(ScopeEffect_Zoom)
	+ ScreenCenter 
	+ ScopeEffect_OriPositionOffset * diffSize
	+ areaCenter
	;
	
    float2 destCoord = destPos * PixelSize;

	float4 colorDest= tBACKBUFFER.Sample(gSamLinear,destCoord);

	float power = 0.008 * rcp(diffSize.y);
	color.xyz = lerp(colorBackup.xyz, colorDest.xyz, 1);

	color.r = tBACKBUFFER.Sample(gSamLinear,mad(float2(-power,0),distToCenter,destCoord)).r;
	color.b = tBACKBUFFER.Sample(gSamLinear,mad(float2(power,0),distToCenter,destCoord)).b;
	color.g = tBACKBUFFER.Sample(gSamLinear,destCoord).g;
	
	float4 nColor =  EnableNV * NVGEffect(color,texcoord);
	color = nColor * nColor.a + color * (1 - nColor.a);
	color.rgb *=  (step(distToCenter, 2) * getparallax(distToParallax,diffSize,fovDiff));
	color = ReticleColor * ReticleColor.a + color * (1-ReticleColor.a);
	
	//isRender
	float4 colorFinal = color;
	// * isRender;
	//float4 colorFinal = color;
	colorFinal.a*= (EnableMerge * isRender);

	return colorFinal;
}

