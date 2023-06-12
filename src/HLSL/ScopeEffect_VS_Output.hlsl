#include "Triangle.hlsli"



// void PostProcessVS(in uint id: SV_VertexID, out float4 position: SV_Position, out float2 texcoord: TEXCOORD)
// {
// 	texcoord.x = (id == 2) ? 2.0 : 0.0;
// 	texcoord.y = (id == 1) ? 2.0 : 0.0;
// 	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
// }

// void main(in uint id: SV_VertexID, out float4 position: SV_Position, out float2 texcoord: TEXCOORD)
// {
// 	texcoord.x = (id == 2) ? 2.0 : 0.0;
// 	texcoord.y = (id == 1) ? 2.0 : 0.0;
// 	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
// }

// VertexPosHTex main(VertexPosTex vIn)
// {
//     VertexPosHTex vOut;
//     vOut.posH = float4(vIn.posL, 1.0f);
//     vOut.tex = vIn.tex;
//     return vOut;
// }

struct VS_INPUT
{
    float4 position : POSITION; // 位置
    float2 texcoord : TEXCOORD0; // 纹理坐标
};

// 在HLSL代码中声明输出结构体，注意要与像素着色器的输入一致
struct VS_OUTPUT
{
    float4 position : SV_Position; // 位置
    float2 texcoord : TEXCOORD0; // 纹理坐标
};

// 在顶点着色器中，将输入的顶点位置和纹理坐标传递给输出
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 target = input.position;
    // target = mul(input.position,FtsLocalRotation);
    // target = mul(target,FtsWorldRotation);
    //target = mul(target,testingMat);
    output.position = target;
    output.texcoord = input.texcoord;
    return output;
}

// void main(in uint id: SV_VertexID, in VERTEX input, out float4 position : SV_Position, out float2 texcoord : TEXCOORD)
// {
//     position = float4(input.Position,1);
//     if(id == 1)
//         position = float4(0.25F,0.5F,0.75F,1);
//     texcoord = input.TexCoord;
// }
