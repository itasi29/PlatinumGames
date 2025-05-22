// 構造体定義 --------------------------------------------------------------------

// マテリアルパラメータ
struct DX_D3D11_CONST_MATERIAL
{
    float4 Diffuse; // ディフューズカラー
    float4 Specular; // スペキュラカラー
    float4 Ambient_Emissive; // マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

    float Power; // スペキュラの強さ
    float TypeParam0; // マテリアルタイプパラメータ0
    float TypeParam1; // マテリアルタイプパラメータ1
    float TypeParam2; // マテリアルタイプパラメータ2
};

// マクロ定義 -----------------------------------

#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM			3			// テクスチャ座標変換行列の転置行列の数
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM					54			// トライアングルリスト一つで同時に使用するローカル→ワールド行列の最大数

// データ型定義 ---------------------------------

// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
    float4x4 AntiViewportMatrix; // アンチビューポート行列
    float4x4 ProjectionMatrix; // ビュー　→　プロジェクション行列
    float4 ViewMatrix[3]; // ワールド　→　ビュー行列
    float4 LocalWorldMatrix[3]; // ローカル　→　ワールド行列

    float4 ToonOutLineSize; // トゥーンの輪郭線の大きさ
    float DiffuseSource; // ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
    float SpecularSource; // スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
    float MulSpecularColor; // スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
    float Padding;
};

// その他の行列
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
    float4 ShadowMapLightViewProjectionMatrix[3][4]; // シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
    float4 TextureMatrix[DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM][2]; // テクスチャ座標操作用行列
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	float4 mat[ DX_D3D11_VS_CONST_WORLD_MAT_NUM*3 ] ;					// ローカル　→　ワールド行列
};



// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1)
{
    DX_D3D11_VS_CONST_BUFFER_BASE g_Base;
};

// その他の行列
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX : register(b2)
{
    DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX g_OtherMatrix;
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX : register(b3)
{
    DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX g_LocalWorldMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION; // 座標( ローカル空間 )
    float3 Normal : NORMAL0; // 法線( ローカル空間 )
    float3 Tangent : TANGENT0; // 接戦ベクトル
    float3 Binormal : BINORMAL0; // 従法線ベクトル
    float4 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 TexCoords1 : TEXCOORD1; // サブテクスチャ座標
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    
    // 法線あり
    
	// スキニングメッシュ
    int4 BlendIndices0 : BLENDINDICES0; // ボーン処理用 Float型定数配列インデックス０
    float4 BlendWeight0 : BLENDWEIGHT0; // ボーン処理用ウエイト値０
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 svPosition : SV_POSITION; // 座標( プロジェクション空間 )    //Pと同様
    float4 Position : POSITION0; // 座標(ワールド空間 )
    float4 P : POSITION1; // 座標( プロジェクション空間 )               //3D→2Dに正規化された座標
    float4 VPosition : POSITION2; // 座標(ワールド空間*ビュー )
};

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    
    int4 lBoneFloatIndex = VSInput.BlendIndices0;
    float4 lLocalWorldMatrix[3];
    // 重みづけ加算
    lLocalWorldMatrix[0] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 0] * VSInput.BlendWeight0.x;
    lLocalWorldMatrix[1] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 1] * VSInput.BlendWeight0.x;
    lLocalWorldMatrix[2] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 2] * VSInput.BlendWeight0.x;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 0] * VSInput.BlendWeight0.y;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 1] * VSInput.BlendWeight0.y;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 2] * VSInput.BlendWeight0.y;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 0] * VSInput.BlendWeight0.z;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 1] * VSInput.BlendWeight0.z;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 2] * VSInput.BlendWeight0.z;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 0] * VSInput.BlendWeight0.w;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 1] * VSInput.BlendWeight0.w;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 2] * VSInput.BlendWeight0.w;
    

    // 座標計算( ローカル→ビュー→プロジェクション )
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;
    lWorldPosition.x = dot(lLocalPosition, lLocalWorldMatrix[0]);
    lWorldPosition.y = dot(lLocalPosition, lLocalWorldMatrix[1]);
    lWorldPosition.z = dot(lLocalPosition, lLocalWorldMatrix[2]);
    lWorldPosition.w = 1.0f;

    lViewPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
    lViewPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
    lViewPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
    lViewPosition.w = 1.0f;

    VSOutput.Position.x = dot(lViewPosition, g_Base.ProjectionMatrix[0]);
    VSOutput.Position.y = dot(lViewPosition, g_Base.ProjectionMatrix[1]);
    VSOutput.Position.z = dot(lViewPosition, g_Base.ProjectionMatrix[2]);
    VSOutput.Position.w = dot(lViewPosition, g_Base.ProjectionMatrix[3]);

    VSOutput.Position = lWorldPosition;
    VSOutput.VPosition = lViewPosition;
    VSOutput.P = mul(lViewPosition, g_Base.ProjectionMatrix);
    VSOutput.svPosition = VSOutput.P;
    VSOutput.TexCoords0 = VSInput.TexCoords0;
    
    float4 normal = float4(VSInput.Normal, 0);
    VSOutput.Normal.x = dot(normal, lLocalWorldMatrix[0]);
    VSOutput.Normal.y = dot(normal, lLocalWorldMatrix[1]);
    VSOutput.Normal.z = dot(normal, lLocalWorldMatrix[2]);
    VSOutput.Normal = normalize(VSOutput.Normal);

    float4 tangent = float4(VSInput.Tangent, 0);
    VSOutput.Tangent.x = dot(tangent, lLocalWorldMatrix[0]);
    VSOutput.Tangent.y = dot(tangent, lLocalWorldMatrix[1]);
    VSOutput.Tangent.z = dot(tangent, lLocalWorldMatrix[2]);
    VSOutput.Tangent = normalize(VSOutput.Tangent);

    float4 binormal = float4(VSInput.Binormal, 0);
    VSOutput.Binormal.x = dot(binormal, lLocalWorldMatrix[0]);
    VSOutput.Binormal.y = dot(binormal, lLocalWorldMatrix[1]);
    VSOutput.Binormal.z = dot(binormal, lLocalWorldMatrix[2]);
    VSOutput.Binormal = normalize(VSOutput.Binormal);
    
    return VSOutput;
}