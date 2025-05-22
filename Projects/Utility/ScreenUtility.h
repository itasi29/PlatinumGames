#pragma once
#include <DxLib.h>

class ScreenUtility
{
public:
	/// <summary>
	/// 深度バッファ付きのスクリーンを作成する
	/// </summary>
	/// <param name="screenW">幅</param>
	/// <param name="screenH">高さ</param>
	/// <returns>ハンドル</returns>
	static int MakeDepthRT(int screenW, int screenH)
	{
		SetCreateGraphChannelBitDepth(32);
		SetCreateDrawValidGraphChannelNum(1);
		return MakeScreen(screenW, screenH, false);
	}
};

