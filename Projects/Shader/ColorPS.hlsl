SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

cbuffer GeoInfo : register(b4)
{
    float red;
    float green;
    float blue;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 dif : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 col = tex.Sample(smp, input.uv);
    col.r = red * col.r;
    col.g = green * col.g;
    col.b = blue * col.b;
    
	return col;
}