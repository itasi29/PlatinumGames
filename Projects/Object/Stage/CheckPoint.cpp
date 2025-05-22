#include "CheckPoint.h"
#include "SaveDataManager.h"
#include "StageManager.h"
#include "Quaternion.h"
#include "Collider/ColliderCapsule.h"
#include "Object/Gimmick/GimmickLinkObject.h"

CheckPoint::CheckPoint(StageManager& mgr, const int no) :
	Collidable(Priority::STATIC, ObjectTag::CHECK_POINT),
    m_mgr(mgr),
    m_no(no),
    m_link(nullptr)
{
}

CheckPoint::~CheckPoint()
{
}

void CheckPoint::Init(const Vec3& pos, const Vec3& dir, float size, float radius, const Vec3& respawnDir)
{
    // Physicsに登録
    OnEntryPhysics();
    // 初期化
    m_rigid.SetPos(pos);
    m_rigid.SetGravity(false);
    // リスポーン時のカメラ方向を設定
	m_respawnDir = respawnDir;
	// コライダーの設定
    const auto& sphere = std::dynamic_pointer_cast<MyEngine::ColliderCapsule>(CreateCollider(MyEngine::ColKind::CAPSULE));
    sphere->center = Vec3();
    sphere->isTrigger = true;
    sphere->dir = dir;
    sphere->size = size;
    sphere->radius = radius;
}


void CheckPoint::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
    MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

    auto tag = colider->GetTag();
	// タグがプレイヤーの場合
    if (tag == ObjectTag::PLAYER)
    {
		// チェックポイントの更新
        m_mgr.UpdateCheckPoint(m_no);
		// ギミックリンクオブジェクトがある場合
        if (m_link)
        {
			// ギミックをオンにする
            m_link->OnGimmick();
        }
    }
}
