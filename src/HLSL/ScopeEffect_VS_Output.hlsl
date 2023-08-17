// 定义一个比较运算符
#define cmp -

// 定义一个用于纹理坐标变换的常量缓冲区
cbuffer TextureTransform : register(b1)
{
  float4 TextureScaleOffset; // 纹理坐标的缩放和偏移量
}

// 定义一个用于顶点位置变换的常量缓冲区
cbuffer WorldMatrix : register(b2)
{
  float4x4 WorldMat; // 世界矩阵
}

// 定义一个用于颜色计算的常量缓冲区
cbuffer ColorParams : register(b12)
{
  float4 ColorParams[47]; // 颜色参数
}

// 顶点着色器主函数
void main(
  float4 v0 : POSITION0, // 输入的顶点位置
  float2 v1 : TEXCOORD0, // 输入的纹理坐标
  out float4 o0 : SV_POSITION0, // 输出的顶点位置
  out float4 o1 : TEXCOORD0, // 输出的纹理坐标
  out float4 o2 : COLOR1) // 输出的颜色值
{
    float4 r0,r1,r2; // 定义一些临时变量

    // 将顶点位置从局部空间转换到世界空间
    r0 = mul(v0, WorldMat);
    o0 = r0;

    // 将纹理坐标进行缩放和偏移
    o1.xy = v1.xy * TextureScaleOffset.zw + TextureScaleOffset.xy;
    o1.z = 1;
    o1.w = v1.y;

    //
    // 颜色？别动它为妙
    // 计算颜色值
    //
    r0.x = dot(ColorParams[20], r0); // 点乘第21个颜色参数和顶点位置
    r0.y = dot(ColorParams[21], r0); // 点乘第22个颜色参数和顶点位置
    r0.z = dot(ColorParams[22], r0); // 点乘第23个颜色参数和顶点位置
    
    r1.x = length(r0.xyz); // 计算r0.xyz的长度
    r1.x = r1.x * ColorParams[41].x + -ColorParams[41].z; // 根据第42个颜色参数进行一些运算
    r1.y = cmp(r1.x < 0.0149999997); // 比较r1.x是否小于一个阈值
    r1.z = saturate(r1.x); // 将r1.x限制在0到1之间
    r1.x = cmp(0.75 < r1.x); // 比较r1.x是否大于一个阈值
    r1.w = 66.6666718 * r1.z; // 根据r1.z进行一些运算
    r1.y = r1.y ? r1.w : 1; // 根据r1.y的值选择r1.w或1
    r0.w = 1;
    r0.x = dot(ColorParams[14], r0); // 点乘第15个颜色参数和顶点位置
    r0.x = ColorParams[35].z + r0.x; // 根据第36个颜色参数进行一些运算
    r0.xy = saturate(r0.xx * ColorParams[46].xy + -ColorParams[46].zw); // 根据第47个颜色参数进行一些运算
    r0.y = r0.y + -r0.x;
    r0.x = r1.z * r0.y + r0.x;
    r0.y = 1 + -r0.x;
    r0.y = r0.x * ColorParams[44].w + r0.y; // 根据第45个颜色参数进行一些运算
    r0.z = -0.75 + r1.z;
    r0.w = log2(r1.z); // 计算r1.z的对数
    r0.w = ColorParams[42].w * r0.w; // 根据第43个颜色参数进行一些运算
    r0.w = exp2(r0.w); // 计算r0.w的指数
    r0.z = 4 * r0.z;
    r1.z = 1 + -ColorParams[43].w;
    r0.z = lerp(ColorParams[43].w, 1, r0.z * r1.z); // 根据第44个颜色参数进行线性插值
    r0.z = min(1, r0.z);
    r0.z = min(r0.w, r0.z);
    o2 = 0;
    o2.w = lerp(1, o2.w, o2.w * (r0.z * (r1.y * (r0.y * (r1.x ? o2.w : ColorParams[43].w))))); // 根据r0和r1的值计算o2.w
    o2.xyz = lerp(ColorParams[42].xyz, ColorParams[44].xyz, lerp(ColorParams[43].xyz, ColorParams[45].xyz, o2.xyz)); // 根据颜色参数和o2.xyz进行线性插值

    return;
}


// Shader hash 803ddcff-454d2fd8-4f1db564-f81d41c4
// vs_5_0
//       dcl_globalFlags refactoringAllowed
//       dcl_constantbuffer cb12[47], immediateIndexed
//       dcl_constantbuffer cb1[1], immediateIndexed
//       dcl_constantbuffer cb2[4], immediateIndexed
//       dcl_input v0.xyz
//       dcl_input v1.xy
//       dcl_output_siv o0.xyzw, position
//       dcl_output o1.xyzw
//       dcl_output o2.xyzw
//       dcl_temps 3
//    0: mov r0.xyz, v0.xyzx
//    1: mov r0.w, l(1.0000)
//    2: dp4 r1.x, cb2[0].xyzw, r0.xyzw
//    3: dp4 r1.y, cb2[1].xyzw, r0.xyzw
//    4: dp4 r1.z, cb2[2].xyzw, r0.xyzw
//    5: dp4 r1.w, cb2[3].xyzw, r0.xyzw
//    6: mov o0.xyzw, r1.xyzw
//    7: mad o1.xy, v1.xyxx, cb1[0].zwzz, cb1[0].xyxx
//    8: mov o1.z, l(1.0000)
//    9: mov o1.w, v1.y
//   10: dp4 r0.x, cb12[20].xyzw, r1.xyzw
//   11: dp4 r0.y, cb12[21].xyzw, r1.xyzw
//   12: dp4 r0.z, cb12[22].xyzw, r1.xyzw
//   13: dp3 r1.x, r0.xyzx, r0.xyzx
//   14: sqrt r1.x, r1.x
//   15: mad r1.x, r1.x, cb12[41].x, -cb12[41].z
//   16: lt r1.y, r1.x, l(0.0150)
//   17: mov_sat r1.z, r1.x
//   18: lt r1.x, l(0.7500), r1.x
//   19: mul r1.w, r1.z, l(66.6667)
//   20: movc r1.y, r1.y, r1.w, l(1.0000)
//   21: mov r0.w, l(1.0000)
//   22: dp4 r0.x, cb12[14].xyzw, r0.xyzw
//   23: add r0.x, r0.x, cb12[35].z
//   24: mad_sat r0.xy, r0.xxxx, cb12[46].xyxx, -cb12[46].zwzz
//   25: add r0.y, -r0.x, r0.y
//   26: mad r0.x, r1.z, r0.y, r0.x
//   27: add r0.y, -r0.x, l(1.0000)
//   28: mad r0.y, r0.x, cb12[44].w, r0.y
//   29: add r0.z, r1.z, l(-0.7500)
//   30: log r0.w, r1.z
//   31: mul r0.w, r0.w, cb12[42].w
//   32: exp r0.w, r0.w
//   33: mul r0.z, r0.z, l(4.0000)
//   34: add r1.z, -cb12[43].w, l(1.0000)
//   35: mad r0.z, r0.z, r1.z, cb12[43].w
//   36: min r0.z, r0.z, l(1.0000)
//   37: movc r0.z, r1.x, r0.z, cb12[43].w
//   38: min r0.z, r0.z, r0.w
//   39: mul r0.y, r0.y, r0.z
//   40: mul o2.w, r1.y, r0.y
//   41: add r1.xyz, -cb12[43].xyzx, cb12[45].xyzx
//   42: mad r1.xyz, r0.zzzz, r1.xyzx, cb12[43].xyzx
//   43: add r2.xyz, -cb12[42].xyzx, cb12[44].xyzx
//   44: mad r0.yzw, r0.zzzz, r2.xxyz, cb12[42].xxyz
//   45: add r1.xyz, -r0.yzwy, r1.xyzx
//   46: mad o2.xyz, r0.xxxx, r1.xyzx, r0.yzwy
//   47: ret
