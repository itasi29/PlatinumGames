#include "Collidable.h"
#include <cassert>
#include "Vec3.h"
#include "DebugDraw.h"
#include "Physics.h"
#include "Collider/Colliders.h"

using namespace MyEngine;
namespace
{
	// 重量停止のフレームレート
	constexpr float STAY_GRAVITY_FRAME_RATE = 0.125f;
	// 重量停止の基本フレーム
	constexpr int STAY_BASE_GRAVITY_FRAME = 1;
}

Collidable::Collidable(Priority priority, ObjectTag tag) :
	m_priority(priority),
	m_tag(tag),
	m_isEntry(false)
{
	// 地面のタグの登録
	m_groundTag.emplace_back(ObjectTag::FLOOR);
	m_groundTag.emplace_back(ObjectTag::FLOOR_MOVE);
	m_groundTag.emplace_back(ObjectTag::NO_GATE_FLOOR);
	m_groundTag.emplace_back(ObjectTag::ROOF);
	m_groundTag.emplace_back(ObjectTag::NO_GATE_ROOF);
	m_groundTag.emplace_back(ObjectTag::STAIRS);
	m_groundTag.emplace_back(ObjectTag::PERFORATED_WALL);
	m_groundTag.emplace_back(ObjectTag::CATWALK);
	m_groundTag.emplace_back(ObjectTag::CATWALK_FENCE);
	// 壁のタグの登録
	m_wallTag.emplace_back(ObjectTag::WALL);
	m_wallTag.emplace_back(ObjectTag::NO_GATE_WALL);
	m_wallTag.emplace_back(ObjectTag::PERFORATED_WALL);
	m_wallTag.emplace_back(ObjectTag::DOOR);
	m_wallTag.emplace_back(ObjectTag::DOOR_ARCH);
}

Collidable::~Collidable()
{
}

void Collidable::End()
{
	// エントリーしている場合
	if (m_isEntry)
	{
		// Physicsから削除
		auto& physics = Physics::GetInstance();
		physics.Exit(shared_from_this());
		m_isEntry = false;
	}
}

ColliderBase* Collidable::GetColliderData(int index) const
{
	return m_collider.at(index).get();
}

void Collidable::OnCollideEnter(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Collide", "当たった", colider, selfIndex, sendIndex);
#endif
}

void Collidable::OnCollideStay(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Collide", "当たっている", colider, selfIndex, sendIndex);
#endif
	const auto& tag = colider->GetTag();
	// 地面に着地している場合
	const auto& isFindGround = std::find(m_groundTag.begin(), m_groundTag.end(), tag) != m_groundTag.end();
	if (isFindGround)
	{
		auto vel = m_rigid.GetVelocity();
		// Y軸の加速度が上を向いてる時は一定時間の重量停止時間を付与
		if (vel.y > 0)
		{
			int frame = STAY_BASE_GRAVITY_FRAME * (static_cast<int>(vel.y / STAY_GRAVITY_FRAME_RATE) + 1);
			m_rigid.SetStayGravityFrame(frame);
		}
		// Y軸の速度を0にする
		vel.y = 0;
		m_rigid.SetVelocity(vel);
	}
	// 壁に当たっている場合
	const auto& isFindWall = std::find(m_wallTag.begin(), m_wallTag.end(), tag) != m_wallTag.end();
	if (isFindWall)
	{
		auto vel = m_rigid.GetVelocity();
		// 壁に向かっている速度を0にする
		const auto& projVel = Vec3::Projection(vel, hitInfo.fixDir);
		vel -= projVel;
		m_rigid.SetVelocity(vel);
	}
}

void Collidable::OnCollideExit(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Collide", "離れた", colider, selfIndex, sendIndex);
#endif
}

void Collidable::OnTriggerEnter(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Trigger", "当たった", colider, selfIndex, sendIndex);
#endif
}

void Collidable::OnTriggerStay(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Trigger", "当たっている", colider, selfIndex, sendIndex);
#endif
}

void Collidable::OnTriggerExit(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo)
{
#ifdef _DEBUG
	SendCollideInfo("Trigger", "離れた", colider, selfIndex, sendIndex);
#endif
}

const Vec3& Collidable::GetPos() const
{
	return m_rigid.GetPos();
}

const Vec3& MyEngine::Collidable::GetVelocity() const
{
	return m_rigid.GetVelocity();
}

void Collidable::AddForce(const Vec3& dir, float power)
{
	m_rigid.AddForce(dir, power);
}

void Collidable::OnEntryPhysics()
{
	// 既にエントリーしている場合は処理を抜ける
	if (m_isEntry) return;
	auto& physics = MyEngine::Physics::GetInstance();
	physics.Entry(shared_from_this());
	m_isEntry = true;
}

void Collidable::OnExistPhysics()
{
	// エントリーしていない場合は処理を抜ける
	if (!m_isEntry) return;
	auto& physics = MyEngine::Physics::GetInstance();
	physics.Exit(shared_from_this());
	m_isEntry = false;
}

std::shared_ptr<ColliderBase> Collidable::CreateCollider(const ColKind& kind)
{
	std::shared_ptr<ColliderBase> col;
	// 種類によってコライダーを生成
	if (kind == ColKind::SPHERE)		col = std::make_shared<ColliderSphere>();
	else if (kind == ColKind::CAPSULE)	col = std::make_shared<ColliderCapsule>();
	else if (kind == ColKind::BOX)		col = std::make_shared<ColliderBox>();

	// 生成できなかった場合はエラー
	if (!col)
	{
		assert(false);
		col = std::make_shared<ColliderSphere>();
	}

	// 生成したコライダーを登録
	m_collider.emplace_back(col);

	return col;
}

#ifdef _DEBUG
#include "ObjectTag.h"
void Collidable::SendCollideInfo(const char* start, const char* const end, Collidable* collider, int selfIndex, int sendIndex)
{
	return;
	printf("%s:%sが%sと%s\n", start, Tags::TAG_NAME.at(m_tag), Tags::TAG_NAME.at(collider->GetTag()), end);
	printf("selfIndex: %d/ sendIndex: %d\n", selfIndex, sendIndex);
}
#endif