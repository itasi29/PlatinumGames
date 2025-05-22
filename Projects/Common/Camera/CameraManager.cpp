#include "CameraManager.h"
#include "Camera.h"
#include "PlayerCamera.h"
#include "GateCamera.h"

CameraManager::CameraManager()
{
	// カメラの生成
	for (int i = 0; i < static_cast<int>(CameraKind::MAX); ++i)
	{
		auto& camera = m_cameras[i];
		// プレイヤーカメラとして生成
		if (i <= static_cast<int>(CameraKind::PLAYER))
		{
			camera = std::make_shared<PlayerCamera>();
		}
		// ゲートカメラとして生成
		else if (i <= static_cast<int>(CameraKind::GATE_B_FROM_A))
		{
			camera = std::make_shared<GateCamera>();
		}
		// 通常カメラとして生成
		else
		{
			camera = std::make_shared<Camera>();
		}
	}
}

CameraManager::~CameraManager()
{
}

void CameraManager::Update()
{
	for (auto& item : m_cameras)
	{
		item->Update();
	}
}

void CameraManager::End()
{
}

void CameraManager::AppInfo(CameraKind kind)
{
	m_cameras[static_cast<size_t>(kind)]->AppInfo();
}
