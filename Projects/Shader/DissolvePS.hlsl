SamplerState smp : register(s0);
Texture2D<float4> before : register(t0);
Texture2D<float4> after : register(t1);
Texture2D<float4> dissolve : register(t2);

cbuffer GeoInfo : register(b4)
{
    float frame;
    float3 fireColor;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 dif : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 dis = dissolve.Sample(smp, input.uv);
    const float effectRate = 0.2;
    float d = dis.r * effectRate;
    float f = frame * 1.4 - 0.2;
    float threshold = input.uv.y - f - d + effectRate * 0.5;
    if (threshold < 0)
    {
        return after.Sample(smp, input.uv);
    }
    
    float3 fire = float3(0.09, 0.59, 0.59) * pow(1.0 - saturate(threshold), 20);
    float4 col = before.Sample(smp, input.uv);
    col.rgb = col.rgb + fire;
    
    return col;
}