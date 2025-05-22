#include "GateCamera.h"
#include "Game.h"
#include "Object/Gate/Gate.h"

GateCamera::GateCamera() :
	m_baseVertexAngle(0.0f)
{
	m_info.isTps = false;
}

GateCamera::~GateCamera()
{
}

void GateCamera::Update(const CameraInfo& playerCamera, const Vec3& pairBase)
{
	// 上下の回転情報を合わせる
	m_info.vertexAngle = playerCamera.vertexAngle + m_baseVertexAngle;

	// 基準となるベクトルがある場合
	const auto& baseXZ = Vec3(-pairBase.x, 0, -pairBase.z);
	if (baseXZ.SqLength() > 0)
	{
		// 見ている方向をプレイヤーのカメラの向きに合わせて変更
		const auto& rot = Quaternion::GetQuaternion(baseXZ.GetNormalized(), playerCamera.front);
		m_info.front = rot * m_baseViewDir;
	}
	else
	{
		// ベースの向きに設定
		m_info.front = m_baseViewDir;
	}

	// 右ベクトルの更新
	m_info.right = Vec3::Cross(Vec3::Up(), m_info.front);
	if (m_info.right.SqLength() == 0) m_info.right = Vec3::Right();
}

void GateCamera::SetBase(const Vec3& dir, float angle)
{
	m_baseViewDir = dir;
	m_baseVertexAngle = angle;
}
