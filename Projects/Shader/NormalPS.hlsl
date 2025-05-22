SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> norm : register(t1);
// ↑頂点シェーダから通ってくる画像(モデル本体が持っている画像)
// ↓自分が渡している画像

struct PointLight
{
    float3 pos;      // 位置
    float range;     // 有効範囲
    float intensity; // 強さ
    float3 color;    // 色
};

cbuffer LightBuffer : register(b4)
{
    PointLight lights[50];
    int lightNum;
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

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 depht : SV_Target1;
};

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    // 法線マップから法線ベクトルを取得
    float3 nmap = norm.Sample(smp, input.TexCoords0).rgb;
    nmap = normalize(nmap * 2 - 1);
    float3 N = normalize(nmap.x * input.Tangent - nmap.y * input.Binormal + nmap.z * input.Normal);
    
    PS_OUTPUT o;
    o.color = float4(N * 0.5 + 0.5, 1);
//    return o;
    
    float3 finalColor = float3(0, 0, 0);
    // 各ポイントライトからの影響を計算
    for (int i = 0; i < lightNum; i++)
    {
        // ライトまでのベクトル
        float3 lightDir = lights[i].pos - input.Position.xyz;
        float distance = length(lightDir);
        
        // ライトまでの距離が有効範囲外ならスキップ
        if (distance > lights[i].range) continue;
        
        // 距離による減衰
        float attenuation = saturate(1 - distance / lights[i].range);
        
        // 正規化
        lightDir = normalize(lightDir);
        // ライトの影響を加算
        float NdotL = dot(N, lightDir);
        finalColor += lights[i].color * lights[i].intensity * attenuation * NdotL;
    }
    const float ambient = 0.35;
    float3 ambColor = float3(ambient, ambient, ambient);
    finalColor = saturate(finalColor + ambColor);
    
    float4 col = tex.Sample(smp, input.TexCoords0);
    
    PS_OUTPUT outdata;
    outdata.color = float4(col.rgb * finalColor, col.a);
// 深度値
    float dep = input.P.z / input.P.w; // wで割ってあげないと0～1にならない
    dep = pow(dep, 20);
    outdata.depht = float4(dep, dep, dep, 1.0);
    
    return outdata;
}