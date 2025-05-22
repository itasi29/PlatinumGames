#include "HandObject.h"
#include "Matrix4x4.h"
#include "Collider/ColliderSphere.h"
#include "Collider/ColliderCapsule.h"
#include "Object/Gate/Gate.h"
#include "Object/Gate/GateManager.h"
#include "Gimmick/Turret/Turret.h"

namespace
{
	const Vec3 PIVOT = Vec3(0.0f, -1.5f, 0.0f);
}

HandObject::HandObject(GateManager& gateMgr) :
	Object3DBase(Priority::MIDDLE, ObjectTag::HAND_OBJ),
	m_gateMgr(gateMgr),
	m_gate(nullptr),
	m_colSize(-1),
	m_isHold(false),
	m_isWarp(false),
	m_isCanWarp(false)
{
}

HandObject::~HandObject()
{
}

void HandObject::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	// 初期化
	Object3DBase::Init(pos, scale, rot, list, isGravity);
	m_initPos = pos;
	m_pivot = PIVOT;
	m_colSize = std::dynamic_pointer_cast<MyEngine::ColliderSphere>(m_collider.at(0))->radius;
}

void HandObject::Update()
{
	// ワープ追加判定
	if (m_gate)
	{
		const auto& gateToTarget = m_rigid.GetPos() - m_gate->GetPos();
		const float size = Vec3::Projection(gateToTarget, m_gate->GetNorm()).Length();
		m_isWarp = size > m_colSize;
	}
	
	// 見た目だけワープの場合
	if (m_isWarp)
	{
		m_prePos = m_warpPos;

		/* 場所変換 */
		const auto& gateCol = m_gate->GetCol();
		const auto& pairGate = m_gate->GetPairGate();
		const auto& gateToTarget = m_rigid.GetPos() - m_gate->GetPos();
		const auto& normRot = Quaternion::GetQuaternion(m_gate->GetNorm(), pairGate->GetNorm());
		const auto& upRot = Quaternion::GetQuaternion(gateCol->dir, pairGate->GetCol()->dir);
		// ゲート上での右方向・上方向動いている大きさを取得
		const auto& prNorm = Vec3::Projection(gateToTarget, m_gate->GetNorm());
		const auto& right = normRot * prNorm;
		const auto& up    = upRot * Vec3::Projection(gateToTarget, gateCol->dir);
		// 位置の取得
		const float size = prNorm.Length();
		m_warpPos = pairGate->GetPos() + right + up + pairGate->GetNorm() * size;
	}
	else
	{
		m_prePos = m_rigid.GetPos();
	}
}

void HandObject::Restart()
{
	m_rigid.SetPos(m_initPos);
	auto vel = m_rigid.GetVelocity();
	vel.y = 0;
	m_rigid.SetVelocity(vel);
}

void HandObject::OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto& tag = colider->GetTag();
	if (tag == ObjectTag::TURRET)
	{
		auto turret = dynamic_cast<Turret*>(colider);
		turret->OnDamage((m_rigid.GetPos() - m_prePos).GetNormalized());
	}
	const auto isFind = std::find(m_groundTag.begin(), m_groundTag.end(), tag) != m_groundTag.end();
	if (isFind)
	{
		m_rigid.SetVelocity(Vec3());
	}
}

void HandObject::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		// ワープ対応削除
		m_isWarp = false;
		// 壁裏から入ったらワープしないようにする
		auto gate = dynamic_cast<Gate*>(colider);
		m_isCanWarp = !gate->CheckWarp(m_rigid.GetPos());
	}
}

void HandObject::OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		// ゲート両方未生成の場合は処理しない
		if (!m_gateMgr.IsCreateBothGates()) return;
		// 持たれていないまたはワープ不可の場合は処理しない
		if (!m_isHold && !m_isCanWarp) return;

		auto gate = dynamic_cast<Gate*>(colider);
		if (!m_isAddTag[colider])
		{
			// スルータグ追加
			m_throughTag.emplace_back(gate->GetHitObjectTag());
			m_isAddTag[colider] = true;
		}
		if (!m_isHold)
		{
			// ワープ判定
			if (gate->CheckWarp(m_rigid.GetPos()))
			{
				gate->OnWarp(m_rigid.GetPos(), m_rigid, true, m_colSize);

				auto pairGate = gate->GetPairGate();
				// スルータグの変更
				m_isAddTag[colider] = false;
				m_isAddTag[pairGate.get()] = true;
				m_throughTag.remove(gate->GetHitObjectTag());
				m_throughTag.push_back(pairGate->GetHitObjectTag());
			}
		}
		else
		{
			if (gate->CheckWarp(m_rigid.GetPos()))	m_gate = gate;
			else									m_gate = nullptr;
		}
	}
}

void HandObject::OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerExit(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::GATE)
	{
		if (!m_gateMgr.IsCreateBothGates()) return;

		// スルータグ外す
		if (m_isAddTag[colider])
		{
			auto gate = dynamic_cast<Gate*>(colider);
			m_throughTag.remove(gate->GetHitObjectTag());
			m_isAddTag[colider] = false;
		}
	}
}

void HandObject::AppModelInfo() const
{
	// スケール・回転・座標を適用
	Matrix4x4 mat;
	Vec3 pos;
	if (m_isWarp)	pos = m_warpPos;
	else			pos = m_rigid.GetPos() + m_pivot;
	mat = Matrix4x4::Scale(m_scale) * Matrix4x4::Rot(m_rotation) * Matrix4x4::Pos(pos);
	MV1SetMatrix(m_modelHandle, mat.GetMATRIX());
}

void HandObject::OnHand()
{
	// 持たれていることに
	m_isHold = true;
	// 重力を無効に
	m_rigid.SetGravity(false);
	// スルータグ追加
	m_throughTag.emplace_back(ObjectTag::PLAYER);
	m_throughTag.emplace_back(ObjectTag::GATE_BULLET);
}

void HandObject::EndHand()
{
	// ワープ可能の場合
	if (m_isWarp)
	{
		// 座標の変更
		m_rigid.SetPos(m_warpPos);
		m_gate = nullptr;
		m_isWarp = false;
	}
	// 持たれていないことに
	m_isHold = false;
	// 重力を有効に
	m_rigid.SetGravity(true);
	// スルータグ削除
	m_throughTag.remove(ObjectTag::PLAYER);
	m_throughTag.remove(ObjectTag::GATE_BULLET);
}

void HandObject::SetPos(const Vec3& pos)
{
	m_rigid.SetPos(pos);
}

void HandObject::OnPlayerWarp()
{
	if (!m_isWarp) return;

	m_rigid.SetPos(m_warpPos);
	m_gate = nullptr;
	m_isWarp = false;
}
