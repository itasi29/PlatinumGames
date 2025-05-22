#pragma once
#include "Vec3.h"
#include <cmath>
#include <cassert>
#include "Utility.h"

/// <summary>
/// クオータニオン
/// </summary>
class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion() :
		x(0.0f), y(0.0f), z(0.0f), w(1.0f)
	{}
	Quaternion(float inX, float inY, float inZ, float inW) :
		x(inX), y(inY), z(inZ), w(inW)
	{}

	/// <summary>
	/// 共役クオータニオン
	/// </summary>
	/// <returns>共役クオータニオン</returns>
	Quaternion Conjugated() const { return Quaternion(-x, -y, -z, w); }

	/* 演算子オーバーロード */
	Quaternion operator* (const Quaternion& q) const
	{
		Quaternion res;

		res.x = x * q.w + w * q.x - z * q.y + y * q.z;
		res.y = y * q.w + z * q.x + w * q.y - x * q.z;
		res.z = z * q.w - y * q.x + x * q.y + w * q.z;
		res.w = w * q.w - x * q.x - y * q.y - z * q.z;

		return res;
	}
	Vec3 operator* (const Vec3& vec) const
	{
		Quaternion posQ = Quaternion(vec.x, vec.y, vec.z, 1.0f);

		Quaternion newPos = *this * posQ * this->Conjugated();

		return Vec3(newPos.x, newPos.y, newPos.z);
	}

	/// <summary>
	/// 軸を取得
	/// </summary>
	/// <returns>軸</returns>
	Vec3 GetAxis() const
	{
		Vec3 axis(x, y, z);

		axis.Normalize();

		return axis;
	}
	/// <summary>
	/// 角度を取得(ラジアン)
	/// </summary>
	/// <returns>ラジアン</returns>
	float GetRadian() const
	{
		return 2 * std::acosf(w);
	}
	/// <summary>
	/// 角度を取得(度数法)
	/// </summary>
	/// <returns>度数法</returns>
	float GetAngle() const
	{
		return 2 * std::acosf(w) * Game::RAD_2_DEG;
	}

public:
	/// <summary>
	/// クオータニオン同士の内積
	/// </summary>
	/// <param name="q1">右辺</param>
	/// <param name="q2">左辺</param>
	/// <returns>内積</returns>
	static float Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
	}
	/// <summary>
	/// 軸を基準に回転させるクオータニオンを作成
	/// </summary>
	/// <param name="angle">回転度合い(度数法)</param>
	/// <param name="axis">軸</param>
	/// <returns>クオータニオン</returns>
	static Quaternion AngleAxis(float angle, const Vec3& axis)
	{
		Quaternion result;

		float halfRad = angle * Game::DEG_2_RAD * 0.5f;
		float sin = std::sin(halfRad);
		float cos = std::cos(halfRad);
		auto normAxis = axis.GetNormalized();
		assert(normAxis.SqLength() > 0.0f && "軸がありません");

		result = Quaternion(normAxis.x * sin, normAxis.y * sin, normAxis.z * sin, cos);

		return result;
	}
	/// <summary>
	/// 2つのベクトル間のクオータニオンを取得する
	/// </summary>
	/// <param name="v1">ベクトル１</param>
	/// <param name="v2">ベクトル２</param>
	/// <param name="axisWhenParallel">2ベクトル間が平行の場合の軸</param>
	/// <returns>クオータニオン</returns>
	static Quaternion GetQuaternion(const Vec3& v1, const Vec3& v2, const Vec3& axisWhenParallel = Vec3::Up())
	{
		auto norm1 = v1.GetNormalized();
		auto norm2 = v2.GetNormalized();
		float dot = Vec3::Dot(norm1, norm2);
		// 2ベクトルが平行の場合、回転しないクオータニオンを返す
		if (dot > 0.9999f) return Quaternion();
		// 角度と軸を取得
		float angle = std::acosf(dot) * Game::RAD_2_DEG;
		auto axis = Vec3::Cross(norm1, norm2);

		if (axis.SqLength() < 0.0001f)
		{
			axis = axisWhenParallel;
		}

		return AngleAxis(angle, axis);
	}
	/// <summary>
	/// クオータニオンからオイラー角に変換
	/// </summary>
	/// <param name="q">クオータニオン</param>
	/// <returns>オイラー角(候補2つ)</returns>
	static Tuple<Vec3, Vec3> GetEuler(const Quaternion& q)
	{
		auto sinX = 2 * q.y * q.z - 2 * q.x * q.w;

		// X軸の回転が0度付近の場合、0になるか360で差が大きいので0に丸める
		if (std::abs(sinX) < 0.001f)
		{
			sinX = 0.0f;
		}

		auto x = std::asinf(-sinX);
		// X軸の回転がジンバルロック状態になっている場合(90度付近)
		if (isnan(x) || std::abs((std::abs(x) - Game::PI_F * 0.5f)) < 0.001f)
		{
			x = Game::PI_F * 0.5f;
			if (std::signbit(-sinX)) x *= -1;
			return Tuple<Vec3, Vec3>(ToEulerAngleZimbalLock(x, 0.0f, q), ToEulerAngleZimbalLock(x, 0.0f, q));
		}
		// ジンバルロック状態になっていない
		else
		{
			auto x2 = Game::PI_F - x;
			return Tuple<Vec3, Vec3>(GetEuler(x, q), GetEuler(x2, q));
		}
	}
private:
	/// <summary>
	/// オイラー変換(ジンバルロックなし)
	/// </summary>
	/// <param name="x">x軸周りの回転</param>
	/// <param name="q">クオータニオン</param>
	/// <returns>オイラー角</returns>
	static Vec3 GetEuler(float x, const Quaternion& q)
	{
		auto cosX = std::cosf(x);

		auto sinY = (2 * q.x * q.z + 2 * q.y * q.w) / cosX;
		auto cosY = (2 * std::powf(q.w, 2) + 2 * std::powf(q.z, 2) - 1) / cosX;
		auto y = std::atan2(sinY, cosY);

		auto sinZ = (2 * q.x * q.y + 2 * q.y * q.w) / cosX;
		auto cosZ = (2 * std::powf(q.w, 2) + 2 * std::powf(q.y, 2) - 1) / cosX;
		auto z = std::atan2(sinZ, cosZ);

		Vec3 res;
		res.x = Normalize(x * Game::RAD_2_DEG);
		res.y = Normalize(y * Game::RAD_2_DEG);
		res.z = Normalize(z * Game::RAD_2_DEG);
		return res;
	}
	/// <summary>
	/// オイラー変換(ジンバルロックの場合)
	/// </summary>
	/// <param name="x">x軸周りの回転</param>
	/// <param name="z">z軸周りの回転(0.0fでいい？)</param>
	/// <param name="q">クオータニオン</param>
	/// <returns>オイラー角</returns>
	static Vec3 ToEulerAngleZimbalLock(float x, float z, const Quaternion& q)
	{
		float y = 0.0f;
		if (x > 0)
		{
			auto yMinusZ = std::atan2f(2 * q.x * q.y - 2 * q.z * q.w, 2 * std::powf(q.w, 2) + 2 * std::powf(q.x, 2) - 1);
			y = yMinusZ + z;
		}
		else
		{
			auto yPlusZ = std::atan2f(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * std::powf(q.w, 2) + 2 * std::powf(q.x, 2) - 1);
			y = yPlusZ - z;
		}

		Vec3 res;
		res.x = Normalize(x * Game::RAD_2_DEG);
		res.y = Normalize(y * Game::RAD_2_DEG);
		res.z = Normalize(z * Game::RAD_2_DEG);
		return res;
	}
	/// <summary>
	/// 範囲の制限
	/// </summary>
	/// <param name="in">値</param>
	/// <returns>0~359.999f</returns>
	static float Normalize(float in)
	{
		return std::fmod(in > 0.0f ? in : in + 360.0f, 360.0f);
	}
};

