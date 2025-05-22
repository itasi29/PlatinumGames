#include "LaserLaunchPad.h"
#include "Files.h"
#include "SoundManager.h"
#include "LaserBullet.h"
#include "Collider/ColliderSphere.h"

namespace
{
	// 生成間隔
	constexpr int CREATE_COUNT = 60;
	// 発射距離
	constexpr float FIRINGER_LENGTH = 3.0f;
}

LaserLaunchPad::LaserLaunchPad(const GateManager* gateMgr, const int reflectionNum) :
	Object3DBase(Priority::STATIC, ObjectTag::LASER_LAUNCH_PAD),
	m_reflectionNum(reflectionNum),
	m_bullet(nullptr),
	m_gateMgr(gateMgr),
	m_createCount(0),
	m_isClear(false)
{
}

LaserLaunchPad::~LaserLaunchPad()
{
}

void LaserLaunchPad::Init(const Vec3& dir)
{
	// 発射方向
	m_firingDir = -dir;
	// ファイル読み込み
	m_shotSe = FileManager::GetInstance().Load(S_SHOT_ENERGY);
}

void LaserLaunchPad::End()
{
	Object3DBase::End();
	if (m_bullet)
	{
		m_bullet->End();
		m_bullet = nullptr;
	}
}

void LaserLaunchPad::Restart()
{
	m_isClear = false;
	m_createCount = 0;
	DestoryBullet();
}

void LaserLaunchPad::Update()
{
	// クリアされていれば処理しない
	if (m_isClear) return;

	// 弾が生成されている場合、弾の更新だけして処理終了
	if (m_bullet)
	{
		m_bullet->Update();
		return;
	}

	// 生成カウント
	++m_createCount;
	if (m_createCount > CREATE_COUNT)
	{
		SoundManager::GetInstance().PlaySe3D(m_shotSe->GetHandle(), shared_from_this());
		// 弾生成
		m_bullet = std::make_shared<LaserBullet>(this, m_gateMgr, m_reflectionNum);
		auto pos = m_rigid.GetPos() + m_firingDir * FIRINGER_LENGTH;
		m_bullet->Init(pos, m_firingDir);
		// カウント初期化
		m_createCount = 0;
	}
}

void LaserLaunchPad::DestoryBullet()
{
	// 弾が存在しない場合は終了
	if (!m_bullet) return;

	m_bullet->End();
	m_bullet = nullptr;
}
