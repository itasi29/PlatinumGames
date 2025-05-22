#pragma once
#include <DxLib.h>
#include <cassert>
#include <unordered_map>
#include <list>
#include <string>
#include "Files.h"
#include "MyDraw.h"

namespace
{
	// ファイルID
	const wchar_t* const FILE_NUM = L"I_Numbers";
	// 切り取り行数
	constexpr int SRC_ROW_NUM = 4;
	// 切り取り列数
	constexpr int SRC_LINE_NUM = 4;
}

class NumUtility
{
private:
	// シェーダー用の定数バッファ
	struct CBuff
	{
		float red;
		float green;
		float blue;
		float padding;
	};

private:
	NumUtility() :
		m_file(nullptr),
		m_cBuff(nullptr),
		m_handle(-1),
		m_psH(-1),
		m_cBuffH(-1)
	{
		// 文字の切り取りリストを作成
		m_currentList[L"."] = 10;
		m_currentList[L":"] = 11;
		m_currentList[L"-"] = 12;
	}

	NumUtility(const NumUtility& mgr) = delete;
	void operator= (const NumUtility& mgr) = delete;

public:
	static NumUtility& GetInstance()
	{
		static NumUtility instance;
		return instance;
	}

	void Init()
	{
		// ファイルの読み込み
		auto& fileMgr = FileManager::GetInstance();
		m_handle = fileMgr.Load(I_NUMBERS, true)->GetHandle();
		m_psH = fileMgr.Load(PS_COLOR, true)->GetHandle();

		// 定数バッファの作成
		m_cBuffH = CreateShaderConstantBuffer(sizeof(CBuff));
		assert(m_cBuffH != -1);
		// 定数バッファの取得
		auto temp = GetBufferShaderConstantBuffer(m_cBuffH);
		m_cBuff = static_cast<CBuff*>(temp);
		assert(m_cBuff != nullptr);
	}

	void End()
	{
		DeleteShaderConstantBuffer(m_cBuffH);
	}

	/// <summary>
	/// 数字の画像描画(1桁のみ)(整数)
	/// </summary>
	/// <param name="x">中心座標</param>
	/// <param name="y">中心座標</param>
	/// <param name="size">サイズ</param>
	/// <param name="color">色</param>
	/// <param name="num">数字(-1だと下の文字を使う)</param>
	/// <param name="c">使用可能文字(".", ":")</param>
	/// <param name="isShadow">true: 影有り /false:影無し</param>
	/// <param name="shadowY">影のX位置</param>
	/// <param name="shadowX">影のY位置</param>
	/// <param name="shadowColor">影の色(def = 0(黒))</param>
	void DrawNumber(int x, int y, float size, unsigned int color, int num, const wchar_t* const c = L"", bool isShadow = false, int shadowX = 2, int shadowY = 2, unsigned int shadowColor = 0)
	{
		assert(-1 <= num && num < 10 && "数字が範囲外です");
		if (num < 0) num = m_currentList.at(c);

		// 切り取り位置
		int srcX = SRC_SIZE_X * (num % SRC_ROW_NUM);
		int srcY = SRC_SIZE_Y * (num / SRC_LINE_NUM);
		// 影描画
		if (isShadow) DrawNumber(x + shadowX, y + shadowY, srcX, srcY, size, shadowColor);
		// 通常描画
		DrawNumber(x, y, srcX, srcY, size, color);
	}
	/// <summary>
	/// 数字の画像描画(1桁以上)(整数)
	/// </summary>
	/// <param name="x">中心座標</param>
	/// <param name="y">中心座標</param>
	/// <param name="size">サイズ</param>
	/// <param name="color">色</param>
	/// <param name="num">数字</param>
	/// <param name="fillNum">0埋め数</param>
	/// <param name="isShadow">true: 影有り /false:影無し</param>
	/// <param name="shadowY">影のX位置</param>
	/// <param name="shadowX">影のY位置</param>
	/// <param name="shadowColor">影の色(def = 0(黒))</param>
	void DrawNumberMore(int x, int y, float size, unsigned int color, int num, int fillNum = 0, bool isShadow = false, int shadowX = 2, int shadowY = 2, unsigned int shadowColor = 0)
	{
		std::list<unsigned int> list;
		// 負の数の場合は正に変換
		const bool isUnder = num < 0;
		if (isUnder) num *= -1;
		// 一番桁から数字を取得していく
		while (true)
		{
			list.emplace_front(num % 10);
			// 次の桁に移行
			num /= 10;
			// 数字が0になると終了
			if (num <= 0) break;
		}
		// 0埋め
		if (static_cast<int>(list.size()) < fillNum) for (int i = static_cast<int>(list.size()); i < fillNum; ++i) list.emplace_front(0);

		int addSize = static_cast<int>(SRC_SIZE_X * size);
		int count = 1;
		x = x - static_cast<int>(addSize * list.size() * 0.5f);
		// 描画
		for (auto& item : list)
		{
			DrawNumber(x, y, size, color, item, L"", isShadow, shadowX, shadowY, shadowColor);
			// 位置更新
			x += addSize;
			++count;
		}
		// 負の数の場合
		if (isUnder)
		{
			// 先頭に"-"を追加
			x -= addSize * count;
			DrawNumber(x, y, size, color, -1, L"-", isShadow, shadowX, shadowY, shadowColor);
		}
	}

public:
	// 切り取りサイズ
	static constexpr int SRC_SIZE_X = 66;
	static constexpr int SRC_SIZE_Y = 130;

private:
	void DrawNumber(int x, int y, int srcX, int srcY, float size, unsigned int color)
	{
		// 色設定
		auto red	= (color & 0xff0000) >> 16;
		auto green	= (color & 0x00ff00) >> 8;
		auto blue	= (color & 0x0000ff);
		m_cBuff->red = red / 255.0f;
		m_cBuff->green = green / 255.0f;
		m_cBuff->blue = blue / 255.0f;
		UpdateShaderConstantBuffer(m_cBuffH);
		SetShaderConstantBuffer(m_cBuffH, DX_SHADERTYPE_PIXEL, 4);
		// 描画
		MyEngine::DrawRectRotaGraph(x, y, srcX, srcY, SRC_SIZE_X, SRC_SIZE_Y, size, 0.0f, m_psH, m_handle);
	}

private:
	std::shared_ptr<FileBase> m_file;
	std::unordered_map<const wchar_t*, int> m_currentList;
	CBuff* m_cBuff;
	int m_handle;
	int m_psH;
	int m_cBuffH;
};