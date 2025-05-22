#include "TurretBullet.h"
#include "Collider/ColliderSphere.h"
#include "Object/Player.h"
#include "EffekseerManager.h"
#include "FileId.h"

namespace
{
	// 当たり半径
	constexpr float RADIUS = 0.2f;
	// 速度
	constexpr float SPEED = 1.0f;
	// ダメージ
	constexpr int DAMAGE = 2;
}

TurretBullet::TurretBullet() :
	Object3DBase(Priority::LOW, ObjectTag::TURRET_BULLET),
	m_player(nullptr),
	m_playEffH(-1),
	m_isExist(true)
{
}

TurretBullet::~TurretBullet()
{
}

void TurretBullet::Init(const Vec3& pos, const Vec3& dir, Player* player)
{
	// Physicsに登録
	OnEntryPhysics();
	// 初期化
	m_rigid.SetPos(pos);
	m_rigid.SetVelocity(dir * SPEED);
	m_rigid.SetGravity(false);
	m_rotation = Quaternion::GetQuaternion(Vec3::Back(), dir);
	// コライダーの設定
	auto collider = std::dynamic_pointer_cast<MyEngine::ColliderSphere>(CreateCollider(MyEngine::ColKind::SPHERE));
	collider->radius = RADIUS;
	collider->isTrigger = true;
	// プレイヤーの設定
	m_player = player;
	// エフェクトの再生
	auto& effMgr = EffekseerManager::GetInstance();
	m_playEffH = effMgr.Play(E_BULLET);
}

void TurretBullet::End()
{
	Object3DBase::End();

	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.Stop(m_playEffH);
}

void TurretBullet::Update()
{
	auto& effMgr = EffekseerManager::GetInstance();
	// エフェクトが終了したら再度再生
	if (!effMgr.IsPlay(m_playEffH)) m_playEffH = effMgr.Play(E_BULLET);
	// エフェクトの情報を設定
	effMgr.SetInfo(m_playEffH, m_rigid.GetPos(), m_rotation);
}

void TurretBullet::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// タグがプレイヤーならダメージを与え、吹き飛ばす方向を渡す
	if (tag == ObjectTag::PLAYER)
	{
		m_player->OnDamage(DAMAGE, m_rigid.GetDir());
	}

	// タグがプレイヤー・タレット・タレットの球でないなら球を終了させる
	if (tag != ObjectTag::TURRET && tag != ObjectTag::TURRET_BULLET)
	{
		m_isExist = false;
	}
}
