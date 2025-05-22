#include "Piston.h"
#include "Easing.h"

Piston::Piston() :
	GimmickLinkObject(Priority::STATIC, ObjectTag::PISTON),
	m_isExpand(false),
	m_isShrink(false)
{
}

Piston::~Piston()
{
}

void Piston::GimmickOnUpdate()
{
	// 拡張完了していれば処理はしない
	if (m_isExpand) return;

	// 移動処理
	auto pos = m_rigid.GetPos();
	pos = Easing::Lerp(pos, m_end, 0.02f);
	// 終了地点まで移動完了すれば
	if ((pos - m_end).SqLength() < 0.001f)
	{
		// 位置を終了地点に補正
		pos = m_end;
		// 拡張完了フラグを立てる
		m_isExpand = true;
	}
	// 位置を設定
	m_rigid.SetPos(pos);
}

void Piston::GimmickOffUpdate()
{
	// 収縮完了していれば処理はしない
	if (m_isShrink) return;

	// 移動処理
	auto pos = m_rigid.GetPos();
	pos = Easing::Lerp(pos, m_start, 0.02f);
	// 開始地点まで移動完了すれば
	if ((pos - m_start).SqLength() < 0.001f)
	{
		// 位置を開始地点に補正
		pos = m_start;
		// 収縮完了フラグを立てる
		m_isShrink = true;
	}
	// 位置を設定
	m_rigid.SetPos(pos);
}

void Piston::ProcessGimmickOn()
{
	// 拡張完了していないことに
	m_isExpand = false;
}

void Piston::ProcessGimmickOff()
{
	// 収縮完了していないことに
	m_isShrink = false;
}
