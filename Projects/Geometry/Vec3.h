#pragma once
#include <DxLib.h>
#include <cmath>
#include <cassert>
#include "Game.h"
#include "Vec2.h"

/// <summary>
/// ベクター3
/// </summary>
class Vec3
{
public:
	float x;
	float y;
	float z;

	Vec3() : 
		x(0.0f), y(0.0f), z(0.0f) 
	{}
	Vec3(float in) :
		x(in), y(in), z(in)
	{}
	Vec3(float inX, float inY, float inZ) :
		x(inX), y(inY), z(inZ)
	{}
	Vec3(const Vec2& vec2) :
		x(vec2.x), y(vec2.y), z(0.0f)
	{}
	Vec3(const Vec3& vec) :
		x(vec.x), y(vec.y), z(vec.z)
	{}
	Vec3(const VECTOR& vec) :
		x(vec.x), y(vec.y), z(vec.z)
	{}

	/// <summary>
	/// 右ベクトル
	/// </summary>
	/// <returns>(1, 0, 0)</returns>
	static Vec3 Right() { return Vec3(1.0f, 0.0f, 0.0f); }
	/// <summary>
	/// 左ベクトル
	/// </summary>
	/// <returns>(-1, 0, 0)</returns>
	static Vec3 Left() { return Vec3(-1.0f, 0.0f, 0.0f); }
	/// <summary>
	/// 上ベクトル
	/// </summary>
	/// <returns>(0, 1, 0)</returns>
	static Vec3 Up() { return Vec3(0.0f, 1.0f, 0.0f); }
	/// <summary>
	/// 下ベクトル
	/// </summary>
	/// <returns>(0, -1, 0)</returns>
	static Vec3 Down() { return Vec3(0.0f, -1.0f, 0.0f); }
	/// <summary>
	/// 正面ベクトル
	/// </summary>
	/// <returns>(0, 0, 1)</returns>
	static Vec3 Front() { return Vec3(0.0f, 0.0f, 1.0f); }
	/// <summary>
	/// 背面ベクトル
	/// </summary>
	/// <returns>(0, 0, -1)</returns>
	static Vec3 Back() { return Vec3(0.0f, 0.0f, -1.0f); }

	/* 演算 */
	Vec3 operator-() const { return Vec3(-x, -y, -z); }
	Vec3 operator+(const Vec3& val) const { return Vec3(x + val.x, y + val.y, z + val.z); }
	Vec3 operator-(const Vec3& val) const { return Vec3(x - val.x, y - val.y, z - val.z); }
	Vec3 operator*(float scale) const { return Vec3(x * scale, y * scale, z * scale); }
	Vec3 operator/(float scale) const
	{
		if (scale < 0.0f || 0.0f < scale)
		{
			return Vec3(x / scale, y / scale, z / scale);
		}
		else
		{
			assert(false && "ゼロ除算しようとしています");
			return Vec3();
		}
	}
	void operator+=(const Vec3& val)
	{
		x += val.x;
		y += val.y;
		z += val.z;
	}
	void operator-=(const Vec3& val)
	{
		x -= val.x;
		y -= val.y;
		z -= val.z;
	}
	void operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
	}
	void operator/=(float scale)
	{
		if (scale < 0.0f || 0.0f < scale)
		{
			x /= scale;
			y /= scale;
			z /= scale;
		}
		else
		{
			assert(false && "ゼロ除算しようとしています");
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
	}
	bool operator==(const Vec3& val) const
	{
		bool isX = x == val.x;
		bool isY = y == val.y;
		bool isZ = z == val.z;
		return isX && isY && isZ;
	}
	bool operator!=(const Vec3& val) const
	{
		bool isX = x != val.x;
		bool isY = y != val.y;
		bool isZ = z != val.z;
		return isX || isY || isZ;
	}

	/// <summary>
	/// XZ方向のみを取得
	/// </summary>
	/// <returns>(x, z)</returns>
	Vec2 GetXZDir() const { return Vec2(x, z); }

	/// <summary>
	/// 長さ取得
	/// </summary>
	/// <returns>長さ</returns>
	float Length() const { return std::hypot(x, y, z); }
	/// <summary>
	/// 長さの2乗を取得
	/// </summary>
	/// <returns>長さの2乗</returns>
	float SqLength() const { return x * x + y * y + z * z; }
	/// <summary>
	/// 正規化したベクトルを取得
	/// </summary>
	/// <returns>正規化ベクトル</returns>
	Vec3 GetNormalized() const
	{
		Vec3 ref;
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
	/// <summary>
	/// 自身の絶対値化
	/// </summary>
	void Abs()
	{
		x = std::abs(x);
		y = std::abs(y);
		z = std::abs(z);
	}

	/// <summary>
	/// DxLib用のVECTORを取得
	/// </summary>
	/// <returns>DxLib::VECTOR</returns>
	DxLib::VECTOR VGet() const { return DxLib::VGet(x, y, z); }
	
public:
	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="item1">ベクトル１</param>
	/// <param name="item2">ベクトル２</param>
	/// <returns>内積結果</returns>
	static float Dot(const Vec3& item1, const Vec3& item2)
	{
		return item1.x * item2.x + item1.y * item2.y + item1.z * item2.z;
	}
	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="item1">ベクトル１</param>
	/// <param name="item2">ベクトル２</param>
	/// <returns>外積結果</returns>
	static Vec3 Cross(const Vec3& item1, const Vec3& item2)
	{
		Vec3 result;

		result.x = item1.y * item2.z - item1.z * item2.y;
		result.y = item1.z * item2.x - item1.x * item2.z;
		result.z = item1.x * item2.y - item1.y * item2.x;

		return result;
	}
	/// <summary>
	/// 射影
	/// </summary>
	/// <param name="projection">射影するベクトル</param>
	/// <param name="dir">射影後の方向となるベクトル</param>
	/// <returns>射影ベクトル</returns>
	static Vec3 Projection(const Vec3& projection, const Vec3& dir)
	{
		auto dirN = dir.GetNormalized();
		auto d = Dot(dirN, projection);
		return dirN * d;
	}
	/// <summary>
	/// 反射
	/// </summary>
	/// <param name="vec">反射させるベクトル</param>
	/// <param name="norm">法線ベクトル</param>
	/// <returns>反射後のベクトル</returns>
	static Vec3 Reflection(const Vec3& vec, const Vec3& norm)
	{
		auto n = Projection(vec, norm);
		return vec - n * 2;
	}
	/// <summary>
	/// 同じ方向を向いているか(誤差許容)
	/// </summary>
	/// <param name="dir1">方向ベクトル１</param>
	/// <param name="dir2">方向ベクトル２</param>
	/// <returns>true:ほぼ同じ方向を向いている/ false:同じ方向を向いていない</returns>
	static bool IsSameDirection(const Vec3& dir1, const Vec3& dir2)
	{
		auto dirN1 = dir1.GetNormalized();
		auto dirN2 = dir2.GetNormalized();

		// 小数第3位までの比較とする
		const int digit = 100;
		int x1 = static_cast<int>(dirN1.x * digit);
		int y1 = static_cast<int>(dirN1.y * digit);
		int z1 = static_cast<int>(dirN1.z * digit);
		int x2 = static_cast<int>(dirN2.x * digit);
		int y2 = static_cast<int>(dirN2.y * digit);
		int z2 = static_cast<int>(dirN2.z * digit);

		bool isX = x1 == x2;
		bool isY = y1 == y2;
		bool isZ = z1 == z2;

		return isX && isY && isZ;
	}
	/// <summary>
	/// 同じ方向を向いているか(angle度未満)
	/// </summary>
	/// <param name="dir1">方向ベクトル１</param>
	/// <param name="dir2">方向ベクトル２</param>
	/// <param name="angle">判定角度(def = 90.0f)</param>
	/// <returns>true:2つのベクトルの内積結果がangle度未満/ false:内積結果がangle度以上</returns>
	static bool IsUniformDirection(const Vec3& dir1, const Vec3& dir2, float angle = 90.0f)
	{
		auto dirN1 = dir1.GetNormalized();
		auto dirN2 = dir2.GetNormalized();

		float dot = Dot(dirN1, dirN2);
		float aDot = std::acosf(dot);

		return aDot < angle * Game::DEG_2_RAD;
	}
};
