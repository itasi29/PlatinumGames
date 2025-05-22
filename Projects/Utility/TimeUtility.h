#pragma once
#include <sstream>
#include <iomanip>
#include "Font.h"
#include "UIUtility.h"
#include "NumUtility.h"

namespace
{
	constexpr float SLIDE_X = 0.31f;
	constexpr float SLIDE_Y = 0.8f;
	constexpr float NUM_EX_RATE = 0.75f;
}

class TimeUtility
{
public:
	int min;
	int sec;
	int dec;

public:
	static TimeUtility FrameToTime(int frame)
	{
		TimeUtility time;
		// 分に変換
		time.min = frame / 3600;
		frame %= 3600;
		// 秒に変換
		time.sec = frame / 60;
		frame %= 60;
		// 少数以下に変換
		time.dec = static_cast<int>(frame / 60.0f * 100);
		return time;
	}
	/// <summary>
	/// 時間(MM:SS.dd)の描画(中心基準)
	/// 画像として描画し、コンマ以下は小さく描画される
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="frame">時間</param>
	/// <param name="size">画像サイズ(整数部分)</param>
	/// <param name="color">色</param>
	/// <param name="isShadow">true: 影有り /false:影無し</param>
	/// <param name="shadowY">影のX位置</param>
	/// <param name="shadowX">影のY位置</param>
	/// <param name="shadowColor">影の色(def = 0(黒))</param>
	/// <param name="sizeExRate">画像サイズ(少数部分)(def = 0.75)</param>
	static void DrawTime(int x, int y, int frame, float size, unsigned int color, bool isShadow = false, int shadowX = 2, int shadowY = 2, unsigned int shadowColor = 0, float sizeExRate = NUM_EX_RATE)
	{
		auto time = FrameToTime(frame);
		auto& util = NumUtility::GetInstance();
		
		const int add1 = static_cast<int>(NumUtility::SRC_SIZE_X * size);
		const int add2 = static_cast<int>(NumUtility::SRC_SIZE_X * sizeExRate * size);

		// "MM"描画
		int minX = x - add1 * 3;
		util.DrawNumberMore(minX, y, size, color, time.min, 2, isShadow, shadowX, shadowY, shadowColor);
		// ":"描画
		int colonX = x - add1 * 2;
		util.DrawNumber(colonX, y, size, color, -1, L":", isShadow, shadowX, shadowY, shadowColor);
		// "SS"描画
		util.DrawNumberMore(x, y, size, color, time.sec, 2, isShadow, shadowX, shadowY, shadowColor);
		// "."描画
		int dotX = x + add1;
		util.DrawNumber(dotX, y, size, color, -1, L".", isShadow, shadowX, shadowY, shadowColor);
		// "dd"描画
		int decX = x + add1 + add2 * 2;
		int decY = y + static_cast<int>(NumUtility::SRC_SIZE_Y * 0.15f * size * sizeExRate);
		util.DrawNumberMore(decX, decY, size * sizeExRate, color, time.dec, 2, isShadow, shadowX, shadowY, shadowColor);
	}

	/// <summary>
	/// 時間(mm:ss.dd)の描画(左上基準)
	/// 文字列として描画され、全て同じ大きさで描画される
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="frame">時間</param>
	/// <param name="size">フォントサイズ</param>
	/// <param name="color">文字カラー</param>
	/// <param name="startStr">最初に追加する文字列</param>
	/// <param name="endStr">最後に追加する文字列</param>
	/// <param name="isShadow">true: 影有り /false:影無し</param>
	/// <param name="shadowY">影のX位置</param>
	/// <param name="shadowX">影のY位置</param>
	/// <param name="shadowColor">影の色(def = 0(黒))</param>
	static void DrawTimeLeft(int x, int y, int frame, int size, unsigned int color, const wchar_t* const startStr = nullptr, const wchar_t* const endStr = nullptr, bool isShadow = false, int shadowX = 2, int shadowY = 2, unsigned int shadowColor = 0)
	{
		auto time = FrameToTime(frame);
		auto fontH = FontManager::GetInstance().GetHandle(FONT_KAISOTAI, size);

		std::wostringstream oss;
		if (startStr) oss << startStr;
		oss << std::setfill(L'0') << std::setw(2) << time.min << L":";
		oss << std::setfill(L'0') << std::setw(2) << time.sec << L".";
		oss << std::setfill(L'0') << std::setw(2) << time.dec;
		if (endStr) oss << endStr;
		
		if (isShadow) DrawStringToHandle(x + shadowX, y + shadowY, oss.str().c_str(), shadowColor, fontH);
		DrawStringToHandle(x, y, oss.str().c_str(), color, fontH);
	}
};