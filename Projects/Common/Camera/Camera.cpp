#include "Camera.h"
#include <DxLib.h>
#include "Game.h"
#include "Easing.h"
#include "Collider/ColliderSphere.h"
#include "Object/Gate/Gate.h"
#include "Object/Gate/GateManager.h"

namespace
{
	// デフォルト値
	constexpr float DEF_NEAR = 0.75f;	// ニア
	constexpr float DEF_FAR = 90.0f;	// ファー
	constexpr float DEF_FOV_ANGLE = 60.0f;	// 視野角
	constexpr float DEF_FOV_RADIAN = DEF_FOV_ANGLE * Game::DEG_2_RAD;	// 視野角(ラジアン)

	constexpr float CAMERA_DISTANCE = 10.0f;	// カメラの距離
	const Vec3 DIR_RIGHT_BASE = Vec3(-1, 0, 0);	// 右方向
	const Vec3 DIR_FRONT_BASE = Vec3(0, 0, -1);	// 前方向
	constexpr float START_VERTEX_ANGLE = 0.0f;	// 縦方向の回転角度
	const Quaternion START_VERTEX_ROT = Quaternion::AngleAxis(START_VERTEX_ANGLE, DIR_RIGHT_BASE);	// 縦方向の回転
}

Camera::Camera()
{
	// 初期化
	m_info.n = DEF_NEAR;
	m_info.f = DEF_FAR;
	m_info.fov = DEF_FOV_RADIAN;
	m_info.front = DIR_FRONT_BASE;
	m_info.right = DIR_RIGHT_BASE;
	m_info.vertexAngle = START_VERTEX_ANGLE;
	m_info.vertexRot = START_VERTEX_ROT;
	m_info.look = m_info.vertexRot * m_info.front;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// 次の移動位置を計算
	m_info.vertexRot = Quaternion::AngleAxis(m_info.vertexAngle, m_info.right);
	m_info.look = m_info.vertexRot * m_info.front;
	auto newPos = m_info.targetPos - m_info.look * CAMERA_DISTANCE;
	// 少しずつ移動
	auto& nowPos = m_info.cameraPos;
	nowPos = Easing::Lerp(nowPos, newPos, 0.1f);
}

void Camera::SetNearFar(float n, float f)
{
	// -1以下の場合はデフォルト値を設定
	if (n < 0) n = DEF_NEAR;
	m_info.n = n;
	if (f < 0) f = DEF_FAR;
	m_info.f = f;
}

void Camera::SetFov(float angle)
{
	// -1以下の場合はデフォルト値を設定
	if (angle < 0) angle = DEF_FOV_ANGLE;
	m_info.fov = angle * Game::DEG_2_RAD;
}

void Camera::SetFront(const Vec3& front, float vertexAngle)
{
	// 正規化
	const auto& nFront = front.GetNormalized();
	// 現在の正面方向とのクォータニオンを計算
	const auto& q = Quaternion::GetQuaternion(m_info.front, nFront);
	// 正面方向を更新
	m_info.front = nFront;
	// 右方向を更新
	m_info.right = q * m_info.right;
	// 縦方向の回転を更新
	m_info.vertexAngle = vertexAngle;
	m_info.vertexRot = Quaternion::AngleAxis(m_info.vertexAngle, m_info.right);
	// 見ている方向を更新
	m_info.look = m_info.vertexRot * m_info.front;
}

/// <summary>
/// 初期位置
/// </summary>
/// <param name="targetPos">ターゲットの座標</param>
void Camera::SetTargetPos(const Vec3& targetPos)
{
	m_info.targetPos = targetPos;
	auto cameraPos = targetPos - m_info.look * CAMERA_DISTANCE;
	m_info.cameraPos = cameraPos;
}

void Camera::SetTps(bool isTps)
{
	m_info.isTps = isTps;
}

void Camera::AppInfo()
{
	// ニア・ファーを設定
	SetCameraNearFar(m_info.n, m_info.f);
	// 視野角を設定
	SetupCamera_Perspective(m_info.fov);
	// カメラの上方向を設定
	auto upVec = Vec3::Up().VGet();
	// 固定化でないなら正面、右方向から上を計算
	if (!m_info.isUpFixed) upVec = Vec3::Cross(m_info.look, m_info.right).VGet();
	// TPS, FPS別でカメラの位置を設定
	if (m_info.isTps)
	{
		SetCameraPositionAndTargetAndUpVec(m_info.cameraPos.VGet(), m_info.targetPos.VGet(), upVec);
	}
	else
	{
		SetCameraPositionAndTargetAndUpVec(m_info.targetPos.VGet(), (m_info.targetPos + m_info.look * CAMERA_DISTANCE).VGet(), upVec);
	}
}

const Vec3& Camera::GetPos() const
{
	// TPSの場合はカメラの位置を返す
	if (m_info.isTps)
	{
		return m_info.cameraPos;
	}
	// FPSの場合はターゲットの位置を返す
	else
	{
		return m_info.targetPos;
	}
}

float Camera::GetDistance() const
{
	return CAMERA_DISTANCE;
}
