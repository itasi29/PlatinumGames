#include "MoveFloor.h"
#include "MoveFloorManager.h"

namespace
{
	constexpr float MOVE_SPEED = 0.1f;

	// 停止カウント
	constexpr int STOP_COUNT = 180;
}

MoveFloor::MoveFloor(MoveFloorManager& mgr) :
	Object3DBase(Priority::STATIC, ObjectTag::FLOOR_MOVE),
	m_mgr(mgr),
	m_updateFunc(nullptr),
	m_nowCheckPt(0),
	m_nextCheckPt(1),
	m_count(0),
	m_isEnd(false),
	m_isStop(false),
	m_isArrival(false)
{
}

MoveFloor::~MoveFloor()
{
}

void MoveFloor::InitDir(const Vec3& dir)
{
	m_rigid.SetVelocity(dir * MOVE_SPEED);
}

void MoveFloor::Update()
{
	(this->*m_updateFunc)();
}

void MoveFloor::SetFunc(bool isLoop)
{
	if (isLoop)	m_updateFunc = &MoveFloor::LoopModeUpdate;
	else		m_updateFunc = &MoveFloor::NoLoopModeUpdate;
}

void MoveFloor::StopMove()
{
	// 止まっていれば処理しない
	if (m_isStop) return;
	m_isStop = true;

	// 移動ベクトルを保存
	m_saveVel = m_rigid.GetVelocity();
	// 速度を0に変更
	m_rigid.SetVelocity(Vec3());
}

void MoveFloor::ReStartMove()
{
	// 動いていれば処理しない
	if (!m_isStop) return;
	m_isStop = false;

	// 保存していた移動ベクトルを代入
	m_rigid.SetVelocity(m_saveVel);
}

void MoveFloor::LoopModeUpdate()
{
	// 移動先に到着している場合
	if (m_isArrival)
	{
		++m_count;
		// 停止カウント未満なら停止したたま
		if (m_count < STOP_COUNT) return;

		// 到達していないことに
		m_isArrival = false;
		// 次のチェックポイントまでの移動ベクトルに変更
		const auto& dir = m_mgr.GetMoveDir(m_rigid.GetPos(), m_nextCheckPt);
		m_rigid.SetVelocity(dir * MOVE_SPEED);
	}
	// していない場合
	else
	{
		// 次の移動場所まで到達していなければ終了
		if (!m_mgr.CheckMovedCheckPt(m_rigid.GetPos(), m_nextCheckPt)) return;

		m_count = 0;
		// 到達したことに
		m_isArrival = true;
		// 現在と次のCPを変換
		std::swap(m_nowCheckPt, m_nextCheckPt);
		// 位置の補正
		const auto& pos = m_mgr.GetCheckPtPos(m_nowCheckPt);
		m_rigid.SetPos(pos);
		// 移動を0に
		m_rigid.SetVelocity(Vec3());
	}
}

void MoveFloor::NoLoopModeUpdate()
{
	// 次のチェックポイントまで動いていなければ終了
	if (!m_mgr.CheckMovedCheckPt(m_rigid.GetPos(), m_nextCheckPt)) return;

	// チェックポイントを次へ
	++m_nowCheckPt;
	++m_nextCheckPt;
	// 全てのチェックポイント-1分を通っていたら終了
	if (m_nextCheckPt >= m_mgr.GetCheckPtNum())
	{
		End();
		m_isEnd = true;
		return;
	}
	// 位置の補正
	const auto& pos = m_mgr.GetCheckPtPos(m_nowCheckPt);
	m_rigid.SetPos(pos);
	// 次のチェックポイントまでの移動ベクトルに変更
	const auto& dir = m_mgr.GetMoveDir(pos, m_nextCheckPt);
	m_rigid.SetVelocity(dir * MOVE_SPEED);
}
