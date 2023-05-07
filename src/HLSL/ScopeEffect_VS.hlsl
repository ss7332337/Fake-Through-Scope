#include "Triangle.hlsli"

void PostProcessVS(in uint id: SV_VertexID, out float4 position: SV_Position, out float2 texcoord: TEXCOORD)
{
	texcoord.x = (id == 2) ? 2.0 : 0.0;
	texcoord.y = (id == 1) ? 2.0 : 0.0;
	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
}

void main(in uint id : SV_VertexID, out float4 position : SV_Position, out float2 texcoord : TEXCOORD)
{
    texcoord.x = (id == 2) ? 2.0 : 0.0;
	texcoord.y = (id == 1) ? 2.0 : 0.0;

	float distanceWeap = sqrt(
	  pow(CurrWeaponPos.x - CurrRootPos.x, 2) 
	+ pow(CurrWeaponPos.y - CurrRootPos.y, 2) 
	+ pow(CurrWeaponPos.z - CurrRootPos.z, 2)
	//pow(CurrWeaponPos.z - CurrRootPos.z, 2)
	);

	float zMove = abs(saturate((sqrt((BaseWeaponPos)) - distanceWeap))) * MovePercentage;
	
	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0 - zMove);
}
