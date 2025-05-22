SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> rt : register(t1);
Texture2D<float4> dissolve : register(t2);

cbuffer UserData : register(b5)
{
    float3 center;
    float radius;
    float3 dir;
    float size;
    float frame;
    bool isCreate;
}

struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 svPosition : SV_POSITION; // 座標( プロジェクション空間 )
    float4 Position : POSITION0; // 座標( ワールド空間 )
    float4 P : POSITION1; // 座標( プロジェクション空間 )
    float4 VPosition : POSITION2; // 座標( ワールド空間 * ビュー )
};

static const uint UINT_MAX = 0xffffffffu;

uint XorShift(uint val)
{
    val ^= (val << 13);
    val ^= (val >> 17);
    val ^= (val << 15);
    return val;
}

float Random(float val)
{
    float ref = float(XorShift(asuint(val))) / float(UINT_MAX);
    return ref;
}

float3 GetNearPos(float3 c, float3 d, float s, float3 p)
{
    float3 hDir = d * s * 0.5;
    float3 start = c + hDir;
    float3 end = c - hDir;
    
    float3 sToE = end - start;
    float3 sToP = p - start;
    float rate = dot(sToE, sToP) / (sToE.x * sToE.x + sToE.y * sToE.y + sToE.z * sToE.z);
    rate = max(min(rate, 1.0), 0.0);
    
    return start + sToE * rate;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // RTの画像取得場所計算
    float2 location;
    location.x = input.P.x / input.P.w;
    location.y = input.P.y / input.P.w;
    location.y *= -1;
    location.x = (location.x + 1) * 0.5;
    location.y = (location.y + 1) * 0.5;
    
    // ゲートの縁に近づくほど枠線を出す
    float3 near = GetNearPos(center, dir, size, input.Position.xyz);
    float dis = length(near - input.Position.xyz);
    float rate = (dis / radius);
    // 枠線を少しずらすために取得
    float val = frame * 0.001;
    float rand = dissolve.Sample(smp, float2(input.TexCoords0.x + sin(val), input.TexCoords0.y + cos(val))).r;
    rand = rand * 2 - 1;
    // 枠線を出す場合
    if (rate > 0)
    {
        // 範囲を小さく
        rate = pow(rate, 5);
        // 割合調整
        rate = rate * 0.9 + rand * 0.1;
    }
    // 追加色(メイン)
    float4 addMain = float4(0, 0, 0, 1);
    if (!isCreate)
    {
        addMain.rbg = dissolve.Sample(smp, float2(input.TexCoords0.y + sin(val + 1.73), input.TexCoords0.x + cos(val + 2.78))).rbg * -1;
    }
    // 追加色(枠線)
    float add = rand * rate * 0.5;
    float4 addBase = float4(add, add, add, 1);
    
    float4 main = rt.Sample(smp, location);
    float4 base = tex.Sample(smp, location);
    return lerp(main + addMain, base + addBase, rate);
}