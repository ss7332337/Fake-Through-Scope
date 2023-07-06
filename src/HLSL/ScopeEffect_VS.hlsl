#include "Triangle.hlsli"

void main(in VertexPosTex input, out float4 position : SV_Position, out float2 texcoord : TEXCOORD)
{
	texcoord = input.tex;
	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
}
