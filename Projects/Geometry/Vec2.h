#pragma once
#include <DxLib.h>
#include <cmath>
#include <cassert>
#include "Game.h"

/// <summary>
/// ベクター2
/// </summary>
class Vec2
{
public:
	float x;
	float y;
	
	Vec2() :
		x(0.0f), y(0.0f)
	{}
	Vec2(float in) :
		x(in), y(in)
	{}
	Vec2(float inX, float inY) :
		x(inX), y(inY)
	{}

	/* 演算 */
	Vec2 operator-() const { return Vec2(-x, -y); }
	Vec2 operator+(const Vec2& val) const { return Vec2(x + val.x, y + val.y); }
	Vec2 operator-(const Vec2& val) const { return Vec2(x - val.x, y - val.y); }
	Vec2 operator*(float scale) const { return Vec2(x * scale, y * scale); }
	Vec2 operator/(float scale) const
	{
		if (scale < 0.0f || 0.0f < scale)
		{
			return Vec2(x / scale, y / scale);
		}
		else
		{
			assert(false && "ゼロ除算しようとしています");
			return Vec2();
		}
	}
	void operator+=(const Vec2& val)
	{
		x += val.x;
		y += val.y;
	}
	void operator-=(const Vec2& val)
	{
		x -= val.x;
		y -= val.y;
	}
	void operator*=(float scale)
	{
		x *= scale;
		y *= scale;
	}
	void operator/=(float scale)
	{
		if (scale < 0.0f || 0.0f < scale)
		{
			x /= scale;
			y /= scale;
		}
		else
		{
			assert(false && "ゼロ除算しようとしています");
			x = 0.0f;
			y = 0.0f;
		}
	}
	bool operator==(const Vec2& val) const
	{
		bool isX = x == val.x;
		bool isY = y == val.y;
		return isX && isY;
	}
	bool operator!=(const Vec2& val) const
	{
		bool isX = x != val.x;
		bool isY = y != val.y;
		return isX || isY;
	}

	/// <summary>
	/// 長さ取得
	/// </summary>
	/// <returns></returns>
	float Length() const { return std::hypot(x, y); }
	/// <summary>
	/// 長さの2乗を取得
	/// </summary>
	/// <returns></returns>
	float SqLength() const { return x * x + y * y; }
	/// <summary>
	/// 正規化したベクトルを取得
	/// </summary>
	/// <returns></returns>
	Vec2 GetNormalized() const
	{
		Vec2 ref;
		float len = Length();

		if (len > 0.0f)
		{
			ref = *this / len;
		}

		return ref;
	}
	/// <summary>
	/// 自身の正規化
	/// </summary>
	void Normalize()
	{
		float len = Length();

		if (len > 0.0f)
		{
			*this /= len;
		}
	}
	void Abs()
	{
		x = std::abs(x);
		y = std::abs(y);
	}

	/// <summary>
	/// DxLib用のVECTORを取得
	/// </summary>
	/// <returns>DxLib::VECTOR</returns>
	DxLib::VECTOR VGet() const { return DxLib::VGet(x, y, 0.0f); }

public:
	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="item1">ベクトル１</param>
	/// <param name="item2">ベクトル２</param>
	/// <returns>内積結果</returns>
	static float Dot(const Vec2& item1, const Vec2& item2)
	{
		return item1.x * item2.x + item1.y * item2.y;
	}
	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="item1">ベクトル１</param>
	/// <param name="item2">ベクトル２</param>
	/// <returns>外積結果</returns>
	static float Cross(const Vec2& item1, const Vec2& item2)
	{
		return item1.x * item2.y - item1.y * item2.x;
	}
	/// <summary>
	/// 射影
	/// </summary>
	/// <param name="projection">射影するベクトル</param>
	/// <param name="base">射影後の方向となるベクトル</param>
	/// <returns>射影ベクトル</returns>
	static Vec2 Projection(const Vec2& projection, const Vec2& base)
	{
		auto projectionN = projection.GetNormalized();
		return projectionN * Dot(base, projectionN);
	}
	/// <summary>
	/// 同じ方向を向いているか(誤差許容)
	/// </summary>
	/// <param name="dir1">方向ベクトル１</param>
	/// <param name="dir2">方向ベクトル２</param>
	/// <returns>true:ほぼ同じ方向を向いている/ false:同じ方向を向いていない</returns>
	static bool IsSameDirection(const Vec2& dir1, const Vec2& dir2)
	{
		auto dirN1 = dir1.GetNormalized();
		auto dirN2 = dir2.GetNormalized();

		// 小数第3位までの比較とする
		const int digit = 100;
		int x1 = static_cast<int>(dirN1.x * digit);
		int y1 = static_cast<int>(dirN1.y * digit);
		int x2 = static_cast<int>(dirN2.x * digit);
		int y2 = static_cast<int>(dirN2.y * digit);

		bool isX = x1 == x2;
		bool isY = y1 == y2;

		return isX && isY;
	}
	/// <summary>
	/// 同じ方向を向いているか(angle度未満)
	/// </summary>
	/// <param name="dir1">方向ベクトル１</param>
	/// <param name="dir2">方向ベクトル２</param>
	/// <param name="angle">判定角度(def = 90.0f)</param>
	/// <returns>true:2つのベクトルの内積結果がangle度未満/ false:内積結果がangle度以上</returns>
	static bool IsUniformDirection(const Vec2& dir1, const Vec2& dir2, float angle = 90.0f)
	{
		auto dirN1 = dir1.GetNormalized();
		auto dirN2 = dir2.GetNormalized();

		float dot = Dot(dirN1, dirN2);

		return std::acosf(dot) < angle * Game::DEG_2_RAD;
	}
};

