#include "GateDelete.h"
#include "GateManager.h"
#include "EffekseerManager.h"
#include "FileId.h"
#include "Collider/ColliderBase.h"

namespace
{
	constexpr int DELAY_FRAME = 60;
}

GateDelete::GateDelete(GateManager& gateMgr) :
	Object3DBase(Priority::STATIC, ObjectTag::GATE_DELETE),
	m_gateMgr(gateMgr),
	m_effH1(-1),
	m_effH2(-1),
	m_delayFrame(0),
	m_isDelay(true)
{
}

GateDelete::~GateDelete()
{
}

void GateDelete::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	Object3DBase::Init(pos, scale, rot, list, isGravity);
	m_scale = scale * 50.0f;

	PlayEff(m_effH1);

	m_delayFrame = 0;
	m_isDelay = true;
}

void GateDelete::Update()
{
	// TODO: エフェクトのループ再生を行う
	auto& effMgr = EffekseerManager::GetInstance();

	// エフェクトが終了したらもう一度再生する
	if (!effMgr.IsPlay(m_effH1))
	{
		PlayEff(m_effH1);
	}

	// もう一つのに関しては1つ目と半分ずらして再生する
	if (m_isDelay)
	{
		++m_delayFrame;
		if (m_delayFrame > DELAY_FRAME)
		{
			m_isDelay = false;
			PlayEff(m_effH2);
		}
	}
	// エフェクトが終了したらもう一度再生する
	else if (!effMgr.IsPlay(m_effH2))
	{
		PlayEff(m_effH2);
	}
}

void GateDelete::OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnTriggerEnter(colider, selfIndex, sendIndex, hitInfo);

	const auto tag = colider->GetTag();
	if (tag == ObjectTag::PLAYER)
	{
		m_gateMgr.Restart();
	}
}

void GateDelete::PlayEff(int& handle)
{
	auto& effMgr = EffekseerManager::GetInstance();

	handle = effMgr.Play(E_GATE_DELETE);
	effMgr.SetInfo(handle, m_rigid.GetPos() + m_collider.at(0)->center, m_rotation, m_scale);
}
