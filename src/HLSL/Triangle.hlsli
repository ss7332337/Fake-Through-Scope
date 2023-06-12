struct VertexPosTex
{
    float3 posL : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexPosHTex
{
    float4 posH : SV_POSITION;
    float2 tex : TEXCOORD;
};


cbuffer ResolutionConstantData : register(b2){
	float BUFFER_WIDTH;
	float BUFFER_HEIGHT;
};
cbuffer ScopeEffectData : register(b0)
{
	
	float camDepth;
	float GameFov;
	float ScopeEffect_Zoom;
	float parallax_Radius;

	float parallax_relativeFogRadius;
	float parallax_scopeSwayAmount;
	float parallax_maxTravel;
	float ReticleSize;

	float nvIntensity;
	float BaseWeaponPos;
	float MovePercentage;
	int EnableZMove;
	
	int isCircle;
	int EnableNV;
	int EnableMerge;
	float padding;

	float2 ScopeEffect_Size;
	float2 ScopeEffect_OriPositionOffset;

	float2 ScopeEffect_OriSize;
	float2 ScopeEffect_Offset;

	float3 eyeDirection;
	float padding1 = 0;

	float3 eyeDirectionLerp;
	float padding2 = 0;

	float3 eyeTranslationLerp;
	float padding3 = 0;

	float3 CurrWeaponPos;
	float padding4 = 0;

	float3 CurrRootPos;
	float padding5 = 0;

	row_major float4x4 CameraRotation; 
	
};

 cbuffer eyeDirectionData: register(b1)
 {
 	float3 eyeDirection1;
 	float MovePercentage1;
 	float3 eyeDirectionLerp1;
 	float padding12 = 0;
 	float3 CurrWeaponPos1;
 	float padding14 = 0;
 	float3 CurrRootPos1;
 	float padding15 = 0;
 	row_major float4x4 CameraRotation1;
 };


cbuffer ftsPoint: register(b3)
 {
	row_major float4x4 testingMat;
	row_major float4x4 FtsLocalRotation;
 	row_major float4x4 FtsWorldRotation;
	row_major float4x4 CameraRotation2;
 };

SamplerState gSamLinear : register(s0);
SamplerState gSamReticle : register(s1);
Texture2D tBACKBUFFER : register(t4);
Texture2D ReticleTex : register(t5);


float GetAspectRatio() { return BUFFER_WIDTH * rcp(BUFFER_HEIGHT); }
float2 GetPixelSize() { return float2(rcp(BUFFER_WIDTH), rcp(BUFFER_HEIGHT)); }
float2 GetScreenSize() { return float2(BUFFER_WIDTH, BUFFER_HEIGHT); }
#define AspectRatio GetAspectRatio()
#define PixelSize GetPixelSize()
#define ScreenSize GetScreenSize()
float2 aspect_ratio_correction(float2 tc)
{
	tc.x -= 0.5f;
	tc.x *= AspectRatio;
	tc.x += 0.5f;
	return tc;
}
