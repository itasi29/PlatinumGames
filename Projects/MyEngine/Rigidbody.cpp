#include "Rigidbody.h"
#include <cmath>
#include <algorithm>
#include "Common/Game.h"

using namespace MyEngine;

Rigidbody::Rigidbody() :
	m_stayGravityFrame(0),
	m_isGravity(false)
{
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::Init(const Vec3& pos)
{
	m_pos = pos;
	m_nextPos = m_pos;
	m_velocity = Vec3();
	m_dir = Vec3(0, 0, 1);
}

void Rigidbody::SetVelocity(const Vec3& velocity)
{
	m_velocity = velocity;
	// 移動している場合のみ方向を変更
	if (velocity.SqLength())
	{
		m_dir = velocity.GetNormalized();
	}
}

void Rigidbody::AddForce(const Vec3& dir, float power, bool isLimit, float limit, bool isMaxUp)
{
	auto newVel = m_velocity + dir * power;

	// 制限をかける場合
	if (isLimit)
	{
		// 速度方向が力の方向と同じ場合
		bool isSameDir = Vec3::IsUniformDirection(newVel, dir);
		// 制限を超えていた場合
		bool isOverVel = newVel.SqLength() > limit;

		if (isSameDir && isOverVel)
		{
			if (isMaxUp)
			{
				m_velocity = newVel.GetNormalized() * std::sqrt(limit);
			}
			return;
		}
	}

	m_velocity = newVel;
}
