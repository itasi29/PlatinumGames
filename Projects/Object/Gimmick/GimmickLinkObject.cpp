#include "GimmickLinkObject.h"

GimmickLinkObject::GimmickLinkObject(Priority priority, ObjectTag tag) :
	Object3DBase(priority, tag),
	m_isOn(false),
	m_isRelease(false),
	m_count(0)
{
	// ステートの追加
	m_stateMachine.AddState(State::GIMMICK_ON,  [this]() {ProcessGimmickOn(); },  [this]() {GimmickOnUpdate(); },  nullptr);
	m_stateMachine.AddState(State::GIMMICK_OFF, [this]() {ProcessGimmickOff(); }, [this]() {GimmickOffUpdate(); }, nullptr);
}

GimmickLinkObject::~GimmickLinkObject()
{
}

void GimmickLinkObject::Restart()
{
	m_isOn = false;
	m_stateMachine.ChangeState(State::GIMMICK_OFF);
}

void GimmickLinkObject::Update()
{
	// ギミックから離れた場合
	if (m_isRelease)
	{
		// カウント増加
		++m_count;
		// 一定カウント進んだらギミックも停止させる
		if (m_count > 2)
		{
			m_isOn = false;
			m_isRelease = false;
		}
	}
	// ステートの変更
	if (m_isOn) m_stateMachine.ChangeState(State::GIMMICK_ON);
	else		m_stateMachine.ChangeState(State::GIMMICK_OFF);
	// 更新
	m_stateMachine.Update();
}

void GimmickLinkObject::OnGimmick()
{
	// ギミックの起動
	m_isOn = true;
	// ギミックから離れていないことに
	m_isRelease = false;
}

void GimmickLinkObject::StopGimmick()
{
	// ギミックからまだ離れていない時カウント初期化
	if (!m_isRelease)
	{
		m_count = 0;
	}
	// ギミック離れたことに
	m_isRelease = true;
}
