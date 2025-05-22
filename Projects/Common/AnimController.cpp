#include "AnimController.h"
#include <DxLib.h>
#include <cassert>
#include "StringUtility.h"
#include "FileManager.h"

namespace
{
	// IDテーブルの列挙型
	enum class FileIndex
	{
		NAME,
		SPEED,
	};

	// デフォルトブレンドスピード
	constexpr float DEF_BLEND_SPEED = 1 / 16.0f;
}

AnimController::AnimController() :
	m_updateFunc(nullptr),
	m_modelH(-1),
	m_nowAnimIndex(-1),
	m_nowId(-1),
	m_preId(-1),
	m_nowAttachIndex(-1),
	m_preAttachIndex(-1),
	m_blendRate(0.0f),
	m_blendSpeed(DEF_BLEND_SPEED),
	m_isLoop(true),
	m_isStopEnd(false),
	m_isPreStopEnd(false),
	m_isUnique(false),
	m_isPreUnique(false)
{
}

AnimController::~AnimController()
{
}

void AnimController::Init(const wchar_t* const path, int modelH, int id)
{
	// IDテーブルの読み込み
	LoadIdTable(path);
	// アニメーションの初期化
	m_updateFunc = &AnimController::NormalUpdate;
	m_modelH = modelH;
	auto index = MV1GetAnimIndex(m_modelH, m_idTable.at(id).name.c_str());
	assert(index != -1 && "存在しないアニメーション名です");
	m_nowId = id;
	// アニメーションのアタッチ
	m_nowAttachIndex = MV1AttachAnim(modelH, index);
}

void AnimController::Update(float speed, float rate)
{
	assert(m_nowAttachIndex != -1 && "アニメーションがアタッチされていません");
	(this->*m_updateFunc)(speed, rate);
}

bool AnimController::Change(int id, bool isTopStart, bool isStopEnd, bool isUnique, bool isBlendChange, float blendSpeed)
{
	// アニメーション番号を取得
	auto index = MV1GetAnimIndex(m_modelH, m_idTable.at(id).name.c_str());
	// 存在しないアニメーション名の場合はエラー
	if (index == -1)
	{
		assert(false && "存在しないアニメーション名です");
		return false;
	}

	// 同じアニメーションの場合は変更しない
	if (m_nowAnimIndex == index) return false;
	// 変更処理
	m_nowAnimIndex = index;
	m_isLoop = false;
	m_preId = m_nowId;
	m_isPreStopEnd = m_isStopEnd;
	m_isPreUnique = m_isUnique;
	m_isStopEnd = isStopEnd;
	m_isUnique = isUnique;
	m_nowId = id;

	// ブレンドの変更有りの場合
	if (isBlendChange)	BlendChange(index, blendSpeed);
	// ブレンドの変更無しの場合
	else				NoBlendChange(index);

	// アニメーション再生位置の初期化
	if (isTopStart) MV1SetAttachAnimTime(m_modelH, m_nowAttachIndex, 0.0f);

	// 変更が行われたことに
	return true;
}

void AnimController::LoadIdTable(const wchar_t* const path)
{
	auto& fileMgr = FileManager::GetInstance();
	const auto& list = fileMgr.LoadCsv(path);
	for (auto& item : list)
	{
		// 登録
		Info info;
		info.name = StringUtility::StringToWString(item.at(static_cast<int>(FileIndex::NAME)));
		info.speed = std::stof(item.at(static_cast<int>(FileIndex::SPEED)));
		m_idTable.push_back(info);
	}
}

void AnimController::NormalUpdate(float speed, float rate)
{
	if (m_isUnique) AnimUniqueUpdate(m_nowId, m_nowAttachIndex, rate);
	else			AnimUpdate(m_nowId, m_nowAttachIndex, speed, true, m_isStopEnd);
}

void AnimController::BlendUpdate(float speed, float rate)
{
	// それぞれのアニメーションの更新
	if (m_isUnique) AnimUniqueUpdate(m_nowId, m_nowAttachIndex, rate);
	else			AnimUpdate(m_nowId, m_nowAttachIndex, speed, true, m_isStopEnd);
	if (m_isPreUnique)	AnimUniqueUpdate(m_preId, m_preAttachIndex, rate);
	else				AnimUpdate(m_preId, m_preAttachIndex, speed, false, m_isPreStopEnd);

	// ブレンドを進める
	m_blendRate += m_blendSpeed;
	// ブレンド率が1.0を超えたらアニメーションブレンドの終了
	if (m_blendRate > 1.0f)
	{
		m_blendRate = 1.0f;
		// 古いアニメーションの削除
		MV1DetachAnim(m_modelH, m_preAttachIndex);
		m_preAttachIndex = -1;
		// アップデート先の変更
		m_updateFunc = &AnimController::NormalUpdate;
	}
	// ブレンド率の適用
	MV1SetAttachAnimBlendRate(m_modelH, m_preAttachIndex, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_modelH, m_nowAttachIndex, m_blendRate);
}

void AnimController::AnimUpdate(int id, int attachIndex, float speed, bool isNow, bool isStopEnd)
{
	// アニメーションの時間取得
	auto totalTime = MV1GetAttachAnimTotalTime(m_modelH, attachIndex);
	auto time = MV1GetAttachAnimTime(m_modelH, attachIndex);
	// スピード分アニメを進める
	speed = std::max<float>(0.0f, std::min<float>(1.0f, speed));
	time += m_idTable.at(id).speed * speed;
	// 総裁時間を超えたら再生時間から総再生時間分へらす
	while (time >= totalTime)
	{
		time -= totalTime;
		// 現在ならばループをtrueに
		if (isNow) m_isLoop = true;
		// 最後で終了させる場合は時間を総再生時間にして抜ける
		if (isStopEnd)
		{
			time = totalTime;
			break;
		}
	}
	// 時間の適用
	MV1SetAttachAnimTime(m_modelH, attachIndex, time);
}

void AnimController::AnimUniqueUpdate(int id, int attachIndex, float rate)
{
	// アニメーションの時間取得
	auto totalTime = MV1GetAttachAnimTotalTime(m_modelH, m_nowAttachIndex);
	// 時間の適用
	auto time = totalTime * rate;
	auto err = MV1SetAttachAnimTime(m_modelH, m_nowAttachIndex, time);
}

void AnimController::BlendChange(int animIndex, float blendSpeed)
{
	// 古い情報が残っていれば削除する
	if (m_preAttachIndex > -1)
	{
		MV1DetachAnim(m_modelH, m_preAttachIndex);
	}
	// 現在の情報を古い情報として保存する
	m_preAttachIndex = m_nowAttachIndex;
	// 新しくアニメーションをアタッチする
	m_nowAttachIndex = MV1AttachAnim(m_modelH, animIndex);

	// ブレンドの変更
	if (blendSpeed < 0)
	{
		blendSpeed = DEF_BLEND_SPEED;
	}
	m_blendSpeed = blendSpeed;
	m_blendRate = 0.0f;
	MV1SetAttachAnimBlendRate(m_modelH, m_preAttachIndex, 1.0f);
	MV1SetAttachAnimBlendRate(m_modelH, m_nowAttachIndex, 0.0f);

	// Update先の変更
	m_updateFunc = &AnimController::BlendUpdate;
}

void AnimController::NoBlendChange(int animIndex)
{
	// 古い情報が残っていれば削除
	if (m_preAttachIndex > -1)
	{
		MV1DetachAnim(m_modelH, m_preAttachIndex);
		m_preAttachIndex = -1;
	}
	// アニメーションの削除
	MV1DetachAnim(m_modelH, m_nowAttachIndex);
	// 新しいアニメーションをアタッチ
	m_nowAttachIndex = MV1AttachAnim(m_modelH, animIndex);

	// Update先の変更
	m_updateFunc = &AnimController::NormalUpdate;
}
