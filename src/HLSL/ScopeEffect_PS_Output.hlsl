//#include "Triangle.hlsli"

Texture2D outPutRT : register(t5);

// float4 main(VertexPosHTex vout) :SV_Target
// {


// 	//adjtexcoord -= float2(0.5,0.5);
// 	//adjtexcoord += float2(0.5,0.5);
	
// 	// adjtexcoord -= float2(0.5,0.5);
// 	// adjtexcoord.x *= (1920/2048);
// 	// adjtexcoord *= 0.25F;
// 	// adjtexcoord += float2(0.5,0.5);
// 	//adjtexcoord -= float2(0,0.25);
// 	return outPutRT.Sample(gSamLinear, vout.tex);
// }

SamplerState gSamLinear : register(s0);
float4 main(float4 position : SV_Position ,float2 texcoord : TEXCOORD0) : SV_Target
{
    // 使用Sample方法来从纹理中获取颜色值
    
    float2 adjTex = texcoord - float2(0.5,0.5);
    adjTex *= rcp(4.0F);
    adjTex += float2(0.5,0.5);
    
    float4 color = outPutRT.Sample(gSamLinear ,adjTex);
    //color.rgb*=0.75F;
    return color;
}