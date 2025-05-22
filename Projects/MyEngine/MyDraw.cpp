#include "MyDraw.h"
#include <DxLib.h>
#include <array>
#include <cassert>

void StartCommonProcess(std::array<VERTEX2DSHADER, 4>& vertices, int& w, int& h, int firstH)
{
	// テクスチャのサイズを取得
	GetGraphSize(firstH, &w, &h);

	// 頂点情報の設定
	for (auto& v : vertices)
	{
		v.dif = GetColorU8(255, 255, 255, 255);
		v.spc = GetColorU8(0, 0, 0, 0);
		v.rhw = 1.0f;
	}
}

void EndCommonProcess(const std::array<VERTEX2DSHADER, 4> vertices, int psH, int firstH, int secondH, int thirdH, int fourthH, int fifthH, int sixthH, bool isTrans)
{
	// アルファの設定
	if (isTrans) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 今からの描画に使う画像を設定します
	SetUseTextureToShader(0, firstH);
	if (secondH != -1) SetUseTextureToShader(1, secondH);
	if (thirdH != -1) SetUseTextureToShader(2, thirdH);
	if (fourthH != -1) SetUseTextureToShader(3, fourthH);
	if (fifthH != -1) SetUseTextureToShader(4, fifthH);
	if (sixthH != -1) SetUseTextureToShader(5, sixthH);
	// 今から使うシェーダーを設定
	SetUsePixelShader(psH);

	auto err = DrawPrimitive2DToShader(vertices.data(), // アドレス
		static_cast<int>(vertices.size()), // 頂点数
		static_cast<int>(DX_PRIMTYPE_TRIANGLESTRIP));	// トポロジー：どういう風に繋げて描画するか

	assert(err != -1 && "描画に失敗(シェーダ付き)");

	// アルファの解除
	if (isTrans) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 使用したテクスチャの解放
	SetUseTextureToShader(0, -1);
	SetUseTextureToShader(1, -1);
	SetUseTextureToShader(2, -1);
	SetUseTextureToShader(3, -1);
	SetUseTextureToShader(4, -1);
	SetUseTextureToShader(5, -1);
}

void MyEngine::DrawGraph(int x, int y, int psH, int firstH, int secondH, int thirdH, int fourthH, int fifthH, int sixthH, bool isTrans)
{
	int w, h;
	std::array<VERTEX2DSHADER, 4> vertices;
	StartCommonProcess(vertices, w, h, firstH);

	// 矩形になるように並べていく
	// Zの字になるように並べます
	// 左上
	vertices[0].pos = VGet(static_cast<float>(x), static_cast<float>(y), 0.0f);
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;
	// 右上
	vertices[1].pos = VGet(static_cast<float>(x + w), static_cast<float>(y), 0.0f);
	vertices[1].u = 1.0f;
	vertices[1].v = 0.0f;
	// 左下
	vertices[2].pos = VGet(static_cast<float>(x), static_cast<float>(y + h), 0.0f);
	vertices[2].u = 0.0f;
	vertices[2].v = 1.0f;
	// 右下
	vertices[3].pos = VGet(static_cast<float>(x + w), static_cast<float>(y + h), 0.0f);
	vertices[3].u = 1.0f;
	vertices[3].v = 1.0f;

	EndCommonProcess(vertices, psH, firstH, secondH, thirdH, fourthH, fifthH, sixthH, isTrans);
}

void MyEngine::DrawRotaGraph(int x, int y, float scale, float angle, int psH, int firstH, int secondH, int thirdH, int fourthH, int fifthH, int sixthH, bool isTrans)
{
	int w, h;
	std::array<VERTEX2DSHADER, 4> vertices;
	StartCommonProcess(vertices, w, h, firstH);

	float offsetX = w * 0.5f * scale;
	float offsetY = h * 0.5f * scale;

	// 矩形になるように並べていく
	// Zの字になるように並べます
	// 左上
	vertices[0].pos = VGet(x - offsetX, y - offsetY, 0.0f);
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;
	// 右上
	vertices[1].pos = VGet(x + offsetX, y - offsetY, 0.0f);
	vertices[1].u = 1.0f;
	vertices[1].v = 0.0f;
	// 左下
	vertices[2].pos = VGet(x - offsetX, y + offsetY, 0.0f);
	vertices[2].u = 0.0f;
	vertices[2].v = 1.0f;
	// 右下
	vertices[3].pos = VGet(x + offsetX, y + offsetY, 0.0f);
	vertices[3].u = 1.0f;
	vertices[3].v = 1.0f;

	EndCommonProcess(vertices, psH, firstH, secondH, thirdH, fourthH, fifthH, sixthH, isTrans);
}

void MyEngine::DrawRectGraph(int x, int y, int srcX, int srcY, int srcW, int srcH, int psH, int firstH, int secondH, int thirdH, int fourthH, int fifthH, int sixthH, bool isTrans)
{
	int w, h;
	std::array<VERTEX2DSHADER, 4> vertices;
	StartCommonProcess(vertices, w, h, firstH);

	float uvStartX = static_cast<float>(srcX) / static_cast<float>(w);
	float uvStartY = static_cast<float>(srcY) / static_cast<float>(h);
	float uvEndX = static_cast<float>(srcX + srcW) / static_cast<float>(w);
	float uvEndY = static_cast<float>(srcY + srcH) / static_cast<float>(h);

	// 矩形になるように並べていく
	// Zの字になるように並べます
	// 左上
	vertices[0].pos = VGet(static_cast<float>(x), static_cast<float>(y), 0.0f);
	vertices[0].u = uvStartX;
	vertices[0].v = uvStartY;
	// 右上
	vertices[1].pos = VGet(static_cast<float>(x + srcW), static_cast<float>(y), 0.0f);
	vertices[1].u = uvEndX;
	vertices[1].v = uvStartY;
	// 左下
	vertices[2].pos = VGet(static_cast<float>(x), static_cast<float>(y + srcH), 0.0f);
	vertices[2].u = uvStartX;
	vertices[2].v = uvEndY;
	// 右下
	vertices[3].pos = VGet(static_cast<float>(x + srcW), static_cast<float>(y + srcH), 0.0f);
	vertices[3].u = uvEndX;
	vertices[3].v = uvEndY;

	EndCommonProcess(vertices, psH, firstH, secondH, thirdH, fourthH, fifthH, sixthH, isTrans);
}

void MyEngine::DrawRectRotaGraph(int x, int y, int srcX, int srcY, int srcW, int srcH, float scale, float angle, int psH, int firstH, int secondH, int thirdH, int fourthH, int fifthH, int sixthH, bool isTrans)
{
	int w, h;
	std::array<VERTEX2DSHADER, 4> vertices;
	StartCommonProcess(vertices, w, h, firstH);

	float offsetX = srcW * 0.5f * scale;
	float offsetY = srcH * 0.5f * scale;

	float uvStartX = static_cast<float>(srcX) / static_cast<float>(w);
	float uvStartY = static_cast<float>(srcY) / static_cast<float>(h);
	float uvEndX = static_cast<float>(srcX + srcW) / static_cast<float>(w);
	float uvEndY = static_cast<float>(srcY + srcH) / static_cast<float>(h);

	// 矩形になるように並べていく
	// Zの字になるように並べます
	// 左上
	vertices[0].pos = VGet(x - offsetX, y - offsetY, 0.0f);
	vertices[0].u = uvStartX;
	vertices[0].v = uvStartY;
	// 右上
	vertices[1].pos = VGet(x + offsetX, y - offsetY, 0.0f);
	vertices[1].u = uvEndX;
	vertices[1].v = uvStartY;
	// 左下
	vertices[2].pos = VGet(x - offsetX, y + offsetY, 0.0f);
	vertices[2].u = uvStartX;
	vertices[2].v = uvEndY;
	// 右下
	vertices[3].pos = VGet(x + offsetX, y + offsetY, 0.0f);
	vertices[3].u = uvEndX;
	vertices[3].v = uvEndY;

	EndCommonProcess(vertices, psH, firstH, secondH, thirdH, fourthH, fifthH, sixthH, isTrans);
}
