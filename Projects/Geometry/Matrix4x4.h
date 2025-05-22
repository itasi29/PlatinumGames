#pragma once
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Vec3.h"
#include "Quaternion.h"

/// <summary>
/// 4x4行列
/// </summary>
class Matrix4x4
{
public:
	float m[4][4];
	// 行->列

	Matrix4x4() :
		m{ 0 }
	{
		m[0][0] = 1.0f;
		m[1][1] = 1.0f;
		m[2][2] = 1.0f;
		m[3][3] = 1.0f;
	}
	Matrix4x4(DxLib::MATRIX mat)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				m[row][line] = mat.m[row][line];
			}
		}
	}

	/* 演算 */
	Matrix4x4 operator+ (const Matrix4x4& mat) const
	{
		Matrix4x4 result;

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = m[row][line] + mat.m[row][line];
			}
		}

		return result;
	}
	Matrix4x4 operator- (const Matrix4x4& mat) const
	{
		Matrix4x4 result;

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = m[row][line] - mat.m[row][line];
			}
		}

		return result;
	}
	Matrix4x4 operator* (const Matrix4x4& mat) const
	{
		Matrix4x4 result;

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = Dot(mat, line, row);
			}
		}

		return result;
	}
	Matrix4x4 operator/ (float div) const
	{
		Matrix4x4 result;

		if (div == 0.0f)
		{
			assert(false && "ゼロ除算しようとしています");
			return result;
		}

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = m[row][line] / div;
			}
		}

		return result;
	}

	/// <summary>
	/// 全値を0にする
	/// </summary>
	void Zero()
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				m[row][line] = 0.0f;
			}
		}
	}
	/// <summary>
	/// 単位行列にする
	/// </summary>
	void Identity()
	{
		Zero();
		m[0][0] = 1.0f;
		m[1][1] = 1.0f;
		m[2][2] = 1.0f;
		m[3][3] = 1.0f;
	}
	/// <summary>
	/// 転置行列を返す
	/// </summary>
	/// <returns>転置行列</returns>
	Matrix4x4 Transpose()
	{
		Matrix4x4 result;

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = m[line][row];
			}
		}

		return result;
	}
	/// <summary>
	/// 逆行列を返す
	/// </summary>
	/// <param name="isCorrect">true:正確な結果を返すが計算が重い /false:簡易的な結果を返すが計算が軽い</param>
	/// <returns>逆行列</returns>
	Matrix4x4 Inverse(bool isCorrect = false)
	{
		Matrix4x4 result;

		// 簡略なやり方
		if (!isCorrect)
		{
			// 回転部分の転置
			for (int row = 0; row < 3; ++row)
			{
				for (int line = 0; line < 3; ++line)
				{
					result.m[row][line] = m[line][row];
				}
			}
			// 平行移動部分の値反転
			for (int i = 0; i < 3; ++i)
			{
				result.m[3][i] = -m[3][i];
			}
		}
		// 正確なやり方
		else
		{
			float det;
			// REVIEW: 間違ってたら公式見直して
			det = m[0][0] * m[1][1] * m[2][2] * m[3][3] +
				m[0][0] * m[1][2] * m[2][3] * m[3][1] +
				m[0][0] * m[1][3] * m[2][1] * m[3][2] +
				m[0][1] * m[1][0] * m[2][3] * m[3][2] +
				m[0][1] * m[1][2] * m[2][0] * m[3][3] +
				m[0][1] * m[1][3] * m[2][2] * m[3][0] +
				m[0][2] * m[1][0] * m[2][1] * m[3][3] +
				m[0][2] * m[1][1] * m[2][3] * m[3][0] +
				m[0][2] * m[1][3] * m[2][0] * m[3][1] +
				m[0][3] * m[1][0] * m[2][2] * m[3][1] +
				m[0][3] * m[1][1] * m[2][0] * m[3][2] +
				m[0][3] * m[1][2] * m[2][1] * m[3][0] -
				m[0][0] * m[1][1] * m[2][3] * m[3][2] -
				m[0][0] * m[1][2] * m[2][1] * m[3][3] -
				m[0][0] * m[1][3] * m[2][2] * m[3][1] -
				m[0][1] * m[1][0] * m[2][2] * m[3][3] -
				m[0][1] * m[1][1] * m[2][3] * m[3][0] -
				m[0][1] * m[1][3] * m[2][0] * m[3][2] -
				m[0][2] * m[1][0] * m[2][3] * m[3][1] -
				m[0][2] * m[1][1] * m[2][0] * m[3][3] -
				m[0][2] * m[1][3] * m[2][1] * m[3][0] -
				m[0][3] * m[1][0] * m[2][1] * m[3][2] -
				m[0][3] * m[1][1] * m[2][2] * m[3][0] -
				m[0][3] * m[1][2] * m[2][0] * m[3][1];

			if (det != 0.0f)
			{
				// REVIEW: 間違ってたら公式見直して
				result.m[0][0] = m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2] - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] - m[1][3] * m[2][2] * m[3][1];
				result.m[0][1] = m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] + m[0][3] * m[2][2] * m[3][1] - m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2];
				result.m[0][2] = m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2] - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] - m[0][3] * m[1][2] * m[3][1];
				result.m[0][3] = m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] + m[0][3] * m[1][2] * m[2][1] - m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2];

				result.m[1][0] = m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] + m[1][3] * m[2][2] * m[3][0] - m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2];
				result.m[1][1] = m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2] - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] - m[0][3] * m[2][2] * m[3][0];
				result.m[1][2] = m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] + m[0][3] * m[1][2] * m[3][0] - m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2];
				result.m[1][3] = m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2] - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] - m[0][3] * m[1][2] * m[2][0];

				result.m[2][0] = m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1] - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] - m[1][3] * m[2][1] * m[3][0];
				result.m[2][1] = m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] + m[0][3] * m[2][1] * m[3][0] - m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1];
				result.m[2][2] = m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1] - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] - m[0][3] * m[1][1] * m[3][0];
				result.m[2][3] = m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] + m[0][3] * m[1][1] * m[2][0] - m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1];

				result.m[3][0] = m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] + m[1][2] * m[2][1] * m[3][0] - m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1];
				result.m[3][1] = m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1] - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] - m[0][2] * m[2][1] * m[3][0];
				result.m[3][2] = m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] + m[0][2] * m[1][1] * m[3][0] - m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1];
				result.m[3][3] = m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] - m[0][2] * m[1][1] * m[2][0];

				result = result / det;
			}
		}


		return result;
	}

	/// <summary>
	/// 座標を返す
	/// </summary>
	/// <returns>座標</returns>
	Vec3 GetPos() const 
	{ 
		return Vec3(m[3][0], m[3][1], m[3][2]); 
	}
	/// <summary>
	/// 回転行列をクオータニオンにして返す
	/// </summary>
	/// <returns>回転</returns>
	Quaternion GetRot() const
	{
		// 行列の各要素からクオータニオンの成分を計算
		float px = m[0][0] - m[1][1] - m[2][2] + 1.0f;
		float py = -m[0][0] + m[1][1] - m[2][2] + 1.0f;
		float pz = -m[0][0] - m[1][1] + m[2][2] + 1.0f;
		float pw = m[0][0] + m[1][1] + m[2][2] + 1.0f;

		// 最大の成分を選択するための列挙型
		enum Select
		{
			PX,
			PY,
			PZ,
			PW
		};
		Select select = Select::PX;
		float max = px;
		if (max < py)
		{
			select = Select::PY;
			max = py;
		}
		if (max < pz)
		{
			select = Select::PZ;
			max = pz;
		}
		if (max < pw)
		{
			select = Select::PW;
			max = pw;
		}

		Quaternion res;

		// 最大の成分に基づいてクオータニオンを計算
		if (select == Select::PX)
		{
			float x = std::sqrtf(px) * 0.5f;
			float d = 1.0f / (4 * x);
			res.x = x;
			res.y = (m[1][0] + m[0][1]) * d;
			res.z = (m[0][2] + m[2][0]) * d;
			res.w = (m[2][1] - m[1][2]) * d;
		}
		else if (select == Select::PY)
		{
			float y = std::sqrtf(py) * 0.5f;
			float d = 1.0f / (4 * y);
			res.x = (m[1][0] + m[0][1]) * d;
			res.y = y;
			res.z = (m[2][1] + m[1][2]) * d;
			res.w = (m[0][2] - m[2][0]) * d;
		}
		else if (select == Select::PZ)
		{
			float z = std::sqrtf(pz) * 0.5f;
			float d = 1.0f / (4 * z);
			res.x = (m[0][2] + m[2][0]) * d;
			res.y = (m[2][1] + m[1][2]) * d;
			res.z = z;
			res.w = (m[1][0] - m[0][1]) * d;
		}
		else if (select == Select::PW)
		{
			float w = std::sqrtf(pw) * 0.5f;
			float d = 1.0f / (4 * w);
			res.w = (m[2][1] - m[1][2]) * d;
			res.z = (m[0][2] - m[2][0]) * d;
			res.y = (m[1][0] - m[0][1]) * d;
			res.x = w;
		}

		// 計算したクオータニオンを返す
		return res;

	}
	/// <summary>
	/// DxLib用の行列を返す
	/// </summary>
	/// <returns>DxLib::MATRIX</returns>
	MATRIX GetMATRIX() const
	{
		MATRIX result;

		for (int row = 0; row < 4; ++row)
		{
			for (int line = 0; line < 4; ++line)
			{
				result.m[row][line] = m[row][line];
			}
		}

		return result;
	}

private:
	/// <summary>
	/// 左辺の列と右辺の行を計算する
	/// </summary>
	/// <param name="mat">右辺側の行列</param>
	/// <param name="line">列</param>
	/// <param name="row">行</param>
	/// <returns>計算した値</returns>
	float Dot(const Matrix4x4& mat, int line, int row) const
	{
		float result = 0.0f;

		for (int i = 0; i < 4; ++i)
		{
			float val = m[row][i] * mat.m[i][line];
			result += val;
		}

		return result;
	}

public:
	/// <summary>
	/// ベクトルをスケールにした行列を返す
	/// </summary>
	/// <param name="size">スケール</param>
	/// <returns>スケールに値が入った行列</returns>
	static Matrix4x4 Scale(const Vec3& size)
	{
		Matrix4x4 result;
		result.Identity();

		result.m[0][0] = size.x;
		result.m[1][1] = size.y;
		result.m[2][2] = size.z;

		return result;
	}
	/// <summary>
	/// ベクトルを座標にした行列を返す
	/// </summary>
	/// <param name="pos">座標</param>
	/// <returns>座標が入った行列</returns>
	static Matrix4x4 Pos(const Vec3& pos)
	{
		Matrix4x4 mat;
		mat.Identity();

		mat.m[3][0] = pos.x;
		mat.m[3][1] = pos.y;
		mat.m[3][2] = pos.z;

		return mat;
	}
	/// <summary>
	/// クオータニオンを回転行列にして返す
	/// </summary>
	/// <param name="q">クオータニオン(回転)</param>
	/// <returns>回転行列</returns>
	static Matrix4x4 Rot(const Quaternion& q)
	{
		Matrix4x4 mat;

		// クオータニオンの各成分を使って行列の要素を計算
		auto xy2 = q.x * q.y * 2;
		auto xz2 = q.x * q.z * 2;
		auto xw2 = q.x * q.w * 2;
		auto yz2 = q.y * q.z * 2;
		auto yw2 = q.y * q.w * 2;
		auto zw2 = q.z * q.w * 2;
		auto ww2 = q.w * q.w * 2;

		// 行列の各要素を計算
		auto r00 = ww2 + 2 * q.x * q.x - 1;
		auto r01 = xy2 + zw2;
		auto r02 = xz2 - yw2;

		auto r10 = xy2 - zw2;
		auto r11 = ww2 + 2 * q.y * q.y - 1;
		auto r12 = yz2 + xw2;

		auto r20 = xz2 + yw2;
		auto r21 = yz2 - xw2;
		auto r22 = ww2 + 2 * q.z * q.z - 1;

		// 計算した要素を行列に設定
		mat.m[0][0] = r00;
		mat.m[0][1] = r01;
		mat.m[0][2] = r02;
		mat.m[1][0] = r10;
		mat.m[1][1] = r11;
		mat.m[1][2] = r12;
		mat.m[2][0] = r20;
		mat.m[2][1] = r21;
		mat.m[2][2] = r22;

		// 作成した行列を返す
		return mat;

	}
};


