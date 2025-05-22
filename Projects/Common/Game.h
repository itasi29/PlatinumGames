#pragma once

namespace Game
{
	/* 画面サイズ */
	constexpr int WINDOW_W = 1280;
	constexpr int WINDOW_H = 720;
	constexpr int CENTER_X = static_cast<int>(WINDOW_W * 0.5f);
	constexpr int CENTER_Y = static_cast<int>(WINDOW_H * 0.5f);

	// アルファ値
	constexpr int ALPHA_VAL = 255;

	/* FPS */
	constexpr int FPS_FRAME = 16667;

	/* 数学系 */
	constexpr double PI = 3.14159265358979323846;
	constexpr float PI_F = 3.14159265358979323846f;
	constexpr double PI_HALF = PI * 0.5;
	constexpr float PI_HALF_F = PI_F * 0.5f;
	// 度数法を弧度法に変更
	constexpr float DEG_2_RAD = PI_F / 180.0f;	
	// 弧度法を度数法に変更
	constexpr float RAD_2_DEG = 180.0f / PI_F;

	/* アナログ入力系 */
	constexpr float ANALOG_TRIGGER_RANGE = 255.0f;	// 取りうる値の範囲
	constexpr float ANALOG_STICK_RANGE = 32768.0f;	// 取りうる値の範囲
	constexpr float ANALOG_DEAD_ZONE_INSIDE = 0.1f;		// 内側デッドゾーン
	constexpr float ANALOG_DEAD_ZONE_OUTSIDE = 0.9f;	// 外側デッドゾーン

	/* 重力 */
	constexpr float GRAVITY = 0.022f;
	constexpr float GRAVITY_MAX_SPEED = 3.5f;
	constexpr float GRAVITY_MAX_SPEED_SQ = GRAVITY_MAX_SPEED * GRAVITY_MAX_SPEED;
}