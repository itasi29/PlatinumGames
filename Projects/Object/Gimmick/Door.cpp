#include "Door.h"
#include "Files.h"
#include "SoundManager.h"
#include "Matrix4x4.h"
#include "Collider/ColliderBox.h"

namespace
{
	constexpr float MOVE_SPEED = 0.1f;
	constexpr float MOVE_SIZE = 9.0f;
}

Door::Door() :
	GimmickLinkObject(Priority::STATIC, ObjectTag::DOOR),
	m_isStart(true),
	m_isOpen(false),
	m_isClose(false)
{
}

Door::~Door()
{
}

void Door::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	Object3DBase::Init(pos, scale, rot, list, isGravity);

	// ファイルの読み込み
	m_openSe = FileManager::GetInstance().Load(S_DOOR_OPEN);

	// 法線をボックスコライダーの法線方向に設定
	auto box = dynamic_cast<MyEngine::ColliderBox*>(GetColliderData(0));
	m_right = box->norm;
	// スタート位置を設定
	m_startPos = pos;
}

void Door::Restart()
{
	GimmickLinkObject::Restart();
	m_isClose = true;
	m_rigid.SetPos(m_startPos);
	OnEntryPhysics();
}

void Door::GimmickOnUpdate()
{
	// 開ききっていれば何もしない
	if (m_isOpen) return;

	// 位置を動かす
	auto pos = m_rigid.GetPos();
	pos += m_right * MOVE_SPEED;
	// 距離が一定値まで来たら
	auto len = (pos - m_startPos).SqLength();
	if (len > MOVE_SIZE * MOVE_SIZE)
	{
		// 補正して開ききったことに
		pos = m_startPos + m_right * MOVE_SIZE;
		m_isOpen = true;
	}
	// 位置を決定
	m_rigid.SetPos(pos);
}

void Door::GimmickOffUpdate()
{
	// 閉じ切っていれば何もしない
	if (m_isClose) return;

	// 位置を動かす
	auto pos = m_rigid.GetPos();
	pos -= m_right * MOVE_SPEED;
	// 距離が一定値まで来たら
	auto len = (pos - m_startPos).SqLength();
	if (len < 0.1f)
	{
		// 補正して閉じ切ったことに
		pos = m_startPos;
		m_isClose =  true;
	}
	// 位置を決定
	m_rigid.SetPos(pos);
}

void Door::ProcessGimmickOn()
{
	// 開始でなければサウンドを再生
	if (!m_isStart) SoundManager::GetInstance().PlaySe3D(m_openSe->GetHandle(), shared_from_this());
	// 開始でないことに
	m_isStart = false;
	// 開いていないことに
	m_isOpen = false;
	// Physicsから削除
	OnExistPhysics();
}

void Door::ProcessGimmickOff()
{
	// 開始でなければサウンドを再生
	if (!m_isStart) SoundManager::GetInstance().PlaySe3D(m_openSe->GetHandle(), shared_from_this());
	// 開始でないことに
	m_isStart = false;
	// 閉じていないことに
	m_isClose = false;
	// Physicsに登録
	OnEntryPhysics();
}
