#pragma once

namespace MyEngine
{
	/// <summary>
	/// DxLib::DrawGraphと同じ間隔でシェーダを用いて描画
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="psH">使用するピクセルシェーダ</param>
	/// <param name="firstH">使用画像１</param>
	/// <param name="secondH">使用画像２</param>
	/// <param name="thirdH">使用画像３</param>
	/// <param name="fourthH">使用画像４</param>
	/// <param name="fifthH">使用画像５</param>
	/// <param name="sixthH">使用画像６</param>
	/// <param name="isTrans">true:透過する /false:透過しない</param>
	void DrawGraph(int x, int y, int psH, int firstH, int secondH = -1, int thirdH = -1, int fourthH = -1, int fifthH = -1, int sixthH = -1, bool isTrans = true);
	/// <summary>
	/// DxLib::DrawRotaGraphと同じ間隔でシェーダを用いて描画
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="scale">サイズ</param>
	/// <param name="angle">角度</param>
	/// <param name="psH">使用するピクセルシェーダ</param>
	/// <param name="firstH">使用画像１</param>
	/// <param name="secondH">使用画像２</param>
	/// <param name="thirdH">使用画像３</param>
	/// <param name="fourthH">使用画像４</param>
	/// <param name="fifthH">使用画像５</param>
	/// <param name="sixthH">使用画像６</param>
	/// <param name="isTrans">true:透過する /false:透過しない</param>
	void DrawRotaGraph(int x, int y, float scale, float angle, int psH, int firstH, int secondH = -1, int thirdH = -1, int fourthH = -1, int fifthH = -1, int sixthH = -1, bool isTrans = true);
	/// <summary>
	/// DxLib::DrawRectGraphと同じ感覚でシェーダを用いて描画
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="srcX">X切り取り位置</param>
	/// <param name="srcY">Y切り取り位置</param>
	/// <param name="srcW">X切り取りサイズ</param>
	/// <param name="srcH">Y切り取りサイズ</param>
	/// <param name="psH">使用するピクセルシェーダー</param>
	/// <param name="firstH">使用画像１</param>
	/// <param name="secondH">使用画像２</param>
	/// <param name="thirdH">使用画像３</param>
	/// <param name="fourthH">使用画像４</param>
	/// <param name="fifthH">使用画像５</param>
	/// <param name="sixthH">使用画像６</param>
	/// <param name="isTrans">true:透過する /false:透過しない</param>
	void DrawRectGraph(int x, int y, int srcX, int srcY, int srcW, int srcH, int psH, int firstH, int secondH = -1, int thirdH = -1, int fourthH = -1, int fifthH = -1, int sixthH = -1, bool isTrans = true);
	/// <summary>
	/// DxLib::DrawRectRotaGraphと同じ感覚でシェーダを用いて描画
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="srcX">X切り取り位置</param>
	/// <param name="srcY">Y切り取り位置</param>
	/// <param name="srcW">X切り取りサイズ</param>
	/// <param name="srcH">Y切り取りサイズ</param>
	/// <param name="scale">サイズ</param>
	/// <param name="angle">角度</param>
	/// <param name="psH">使用するピクセルシェーダー</param>
	/// <param name="firstH">使用画像１</param>
	/// <param name="secondH">使用画像２</param>
	/// <param name="thirdH">使用画像３</param>
	/// <param name="fourthH">使用画像４</param>
	/// <param name="fifthH">使用画像５</param>
	/// <param name="sixthH">使用画像６</param>
	/// <param name="isTrans">true:透過する /false:透過しない</param>
	void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int srcW, int srcH, float scale, float angle, int psH, int firstH, int secondH = -1, int thirdH = -1, int fourthH = -1, int fifthH = -1, int sixthH = -1, bool isTrans = true);
}
