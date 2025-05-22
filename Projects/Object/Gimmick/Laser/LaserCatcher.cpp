#include "LaserCatcher.h"
#include "Files.h"
#include "SoundManager.h"
#include "EffekseerManager.h"
#include "Collider/ColliderSphere.h"
#include "Object/Gimmick/GimmickLinkObject.h"

LaserCatcher::LaserCatcher() :
	GimmickSendObject(Priority::STATIC, ObjectTag::LASER_CATCHER),
	m_isClear(false)
{
}

LaserCatcher::~LaserCatcher()
{
}

void LaserCatcher::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	Object3DBase::Init(pos, scale, rot, list, isGravity);

	m_catchSe = FileManager::GetInstance().Load(S_CATCH_ENERGY);
	EffekseerManager::GetInstance().LoadFile(E_ENERGY_CLEAR);
}

void LaserCatcher::Update()
{
	if (!m_isClear) return;

	auto& effMgr = EffekseerManager::GetInstance();
	// クリアエフェクトが再生中でないなら
	if (!effMgr.IsPlay(m_playClearEffH))
	{
		// 再度再生
		m_playClearEffH = effMgr.Play(E_ENERGY_CLEAR);
		// エフェクト位置設定
		effMgr.SetInfo(m_playClearEffH, m_rigid.GetPos(), m_rotation);
	}

}

void LaserCatcher::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// レーザー弾だったら
	if (tag == ObjectTag::LASER_BULLET)
	{
		// ギミックを開始
		m_isClear = true;
		m_linkObj->OnGimmick();
		// サウンド再生
		SoundManager::GetInstance().PlaySe3D(m_catchSe->GetHandle(), shared_from_this());
		// クリアエフェクト再生
		auto& effMgr = EffekseerManager::GetInstance();
		m_playClearEffH = effMgr.Play(E_ENERGY_CLEAR);
		// エフェクト位置設定
		effMgr.SetInfo(m_playClearEffH, m_rigid.GetPos(), m_rotation);
	}
}
