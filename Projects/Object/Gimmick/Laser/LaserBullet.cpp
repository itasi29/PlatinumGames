#include "LaserBullet.h"
#include "Physics.h"
#include "MathHelp.h"
#include "FileId.h"
#include "EffekseerManager.h"
#include "LaserLaunchPad.h"
#include "Collider/ColliderSphere.h"
#include "Collider/ColliderBox.h"
#include "Object/Gate/Gate.h"
#include "Object/Gate/GateManager.h"

namespace
{
	constexpr float MOVE_SPEED = 0.5f;
	constexpr float COL_RADIUS = 1.0f;
}

LaserBullet::LaserBullet(LaserLaunchPad* launcPad, const GateManager* gateMgr, const int reflectNum) :
	Object3DBase(Priority::LOW, ObjectTag::LASER_BULLET),
	m_gateMgr(gateMgr),
	m_maxReflectNum(reflectNum),
	m_launchPad(launcPad),
	m_effHandle(-1),
	m_reflectNum(0),
	m_isCanWarp(false),
	m_isWarp(false),
	m_isPreWarp(false),
	m_isAddTag{ false }
{
}

LaserBullet::~LaserBullet()
{
}

void LaserBullet::Init(const Vec3& pos, const Vec3& dir)
{
	// Physicsに登録
	OnEntryPhysics();
	// 物理情報の設定
	m_rigid.SetGravity(false);
	m_rigid.SetPos(pos);
	m_rigid.SetVelocity(dir.GetNormalized() * MOVE_SPEED);
	// コライダーの設定
	auto col = std::dynamic_pointer_cast<MyEngine::ColliderSphere>(CreateCollider(MyEngine::ColKind::SPHERE));
	col->isTrigger = false;
	col->radius = COL_RADIUS;

	// エフェクトの再生
	m_effHandle = EffekseerManager::GetInstance().Play(E_LASER_BULLET);
	m_reflectNum = 0;

	// スルータグの設定
	m_throughTag.push_back(ObjectTag::PLAYER);
	m_throughTag.push_back(ObjectTag::GATE_BULLET);
	m_throughTag.push_back(ObjectTag::TURRET);
	m_throughTag.push_back(ObjectTag::TURRET_BULLET);
	m_throughTag.push_back(ObjectTag::LASER_LAUNCH_PAD);
}

void LaserBullet::End()
{
	Collidable::End();
	EffekseerManager::GetInstance().Stop(m_effHandle);
}

void LaserBullet::Update()
{
	// 1つ前のワープ状態を保存
	m_isPreWarp = m_isWarp;
	// ワープ情報リセット
	m_isWarp = false;

	const auto& effMgr = EffekseerManager::GetInstance();
	// エフェクトが再生されていない場合は再度再生
	if (!effMgr.IsPlay(m_effHandle)) m_effHandle = EffekseerManager::GetInstance().Play(E_LASER_BULLET);
	// エフェクトの情報を設定
	effMgr.SetInfo(m_effHandle, m_rigid.GetPos(), Quaternion());
}

void LaserBullet::OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// 地面または壁に当たった場合は反射
	bool isFindGround = std::find(m_groundTag.begin(), m_groundTag.end(), tag) != m_groundTag.end();
	bool isFindWall = std::find(m_wallTag.begin(), m_wallTag.end(), tag) != m_wallTag.end();
	if ((isFindGround || isFindWall) && !m_isPreWarp && !m_isWarp)
	{
		auto col = dynamic_cast<MyEngine::ColliderBox*>(colider->GetColliderData(sendIndex));

		// 反射
		auto dir = m_rigid.GetDir();
		const auto& nearPos = GetNearestPtOnBox(m_rigid.GetPos(), colider->GetPos() + col->center, col->size, col->rotation);
		const auto& norm = (m_rigid.GetPos() - nearPos).GetNormalized();
		dir = Vec3::Reflection(dir, norm);
		m_rigid.SetVelocity(dir * MOVE_SPEED);

		// 一定回数反射したら削除
		++m_reflectNum;
		if (m_reflectNum >= m_maxReflectNum)
		{
			m_launchPad->DestoryBullet();
		}
	}
}

void LaserBullet::OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	// MEMO: 地面に着地したことによる速度0かを防ぐためにオーバーロード
}

void LaserBullet::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// 起動装置に当たった場合
	if (tag == ObjectTag::LASER_CATCHER)
	{
		// クリアしたことに
		m_launchPad->OnClear();
		// 弾の削除
		m_launchPad->DestoryBullet();
	}
	else if (tag == ObjectTag::GATE)
	{
		if (!m_gateMgr->IsCreateBothGates()) return;

		// 入ったときに既にワープできる場合は壁裏から入っているのでワープしないようにする
		auto gate = dynamic_cast<Gate*>(colider);
		m_isCanWarp = !gate->CheckWarp(m_rigid.GetPos());
	}
}

void LaserBullet::OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerStay(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		if (!m_isCanWarp || !m_gateMgr->IsCreateBothGates()) return;

		if (!m_isAddTag[colider])
		{
			// スルータグ追加
			auto gate = dynamic_cast<Gate*>(colider);
			m_throughTag.emplace_back(gate->GetHitObjectTag());
			m_isAddTag[colider] = true;
		}

		auto gate = dynamic_cast<Gate*>(colider);
		// ワープ処理
		if (gate->CheckWarp(m_rigid.GetPos()))
		{
			gate->OnWarp(m_rigid.GetPos(), m_rigid, false, COL_RADIUS);
			m_isWarp = true;

			auto pairGate = gate->GetPairGate();
			// スルータグの変更
			m_isAddTag[colider] = false;
			m_isAddTag[pairGate.get()] = true;
			m_throughTag.remove(gate->GetHitObjectTag());
			m_throughTag.push_back(pairGate->GetHitObjectTag());
		}
	}
}

void LaserBullet::OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerExit(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		if (m_isAddTag[colider])
		{
			// スルータグ外す
			auto gate = dynamic_cast<Gate*>(colider);
			m_throughTag.remove(gate->GetHitObjectTag());
			m_isAddTag[colider] = false;
		}
	}
}
