
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

    output.position = target;
    output.texcoord = input.texcoord;
    return output;
}

