#include "Button.h"
#include "Files.h"
#include "SoundManager.h"
#include "Collider/ColliderBox.h"
#include "Object/Gimmick/GimmickLinkObject.h"

namespace
{
	constexpr float MOVE_SPEED = 0.02f;
	constexpr float MOVE_SIZE = 0.8f;
	constexpr int RELEASE_COUNT = 2;
}

Button::Button() :
	GimmickSendObject(Priority::STATIC, ObjectTag::BUTTON),
	m_count(0),
	m_isRelease(false),
	m_isDown(false),
	m_isUp(false)
{
	// ステートの設定
	m_stateMachine.AddState(State::DOWN, [this]() { OnDown(); }, [this]() { UpdateDown(); }, nullptr);
	m_stateMachine.AddState(State::UP,	 [this]() { OnUp(); },	 [this]() { UpdateUp(); },	 nullptr);
}

Button::~Button()
{
}

void Button::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	Object3DBase::Init(pos, scale, rot, list, isGravity);

	// ファイルの読み込み
	m_pushSe = FileManager::GetInstance().Load(S_BUTTON_PUSH);
	// 初期位置の保存
	m_startPos = pos;
	// ステートの初期化
	m_stateMachine.ChangeState(State::UP);
}

void Button::Update()
{
	// リリース状態の場合
	if (m_isRelease)
	{
		// 一定フレーム経過でアップ状態に遷移
		++m_count;
		if (m_count > RELEASE_COUNT)
		{
			m_stateMachine.ChangeState(State::UP);
			m_isRelease = false;
		}
	}

	m_stateMachine.Update();
}

void Button::OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideStay(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// プレイヤーか手持ちオブジェクトの場合
	if (tag == ObjectTag::PLAYER || tag == ObjectTag::HAND_OBJ)
	{
		// ギミックを開始
		m_linkObj->OnGimmick();

		// リリースしていないことに
		m_isRelease = false;
		// ダウン状態に遷移
		m_stateMachine.ChangeState(State::DOWN);
	}
}

void Button::OnCollideExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo)
{
	MyEngine::Collidable::OnCollideExit(colider, selfIndex, sendIndex, hitInfo);

	auto tag = colider->GetTag();
	// プレイヤーか手持ちオブジェクトの場合
	if (tag == ObjectTag::PLAYER || tag == ObjectTag::HAND_OBJ)
	{
		// ギミックを停止
		m_linkObj->StopGimmick();

		// リリースしている場合は処理終了
		if (m_isRelease) return;
		// カウント初期化
		m_count = 0;
		// リリースしていることに
		m_isRelease = true;
	}
}

void Button::UpdateDown()
{
	if (!m_isDown) return;

	// ボタンを下にずらしていく
	auto pos = m_rigid.GetPos();
	pos.y -= MOVE_SPEED;
	// 一定以上下にずらしたら
	if (pos.y < m_startPos.y - MOVE_SIZE)
	{
		// 位置を補正
		pos.y = m_startPos.y - MOVE_SIZE;
		// ダウンを終了
		m_isDown = false;
	}
	// 位置を設定
	m_rigid.SetPos(pos);
}

void Button::UpdateUp()
{
	if (!m_isUp) return;

	// ボタンを上にずらしていく
	auto pos = m_rigid.GetPos();
	pos.y += MOVE_SPEED;
	// 元の位置に戻ったら
	if (pos.y > m_startPos.y)
	{
		// 位置を補正
		pos.y = m_startPos.y;
		// アップを終了
		m_isUp = false;
	}
	// 位置を設定
	m_rigid.SetPos(pos);
}

void Button::OnDown()
{
	SoundManager::GetInstance().PlaySe3D(m_pushSe->GetHandle(), shared_from_this());
	// ダウンフラグを立てる
	m_isDown = true;
}

void Button::OnUp()
{
	// アップフラグを立てる
	m_isUp = true;
}
