#include "GateManager.h"
#include <cassert>
#include "Gate.h"
#include "GateBullet.h"
#include "GateCamera.h"
#include "CameraManager.h"
#include "StageDataManager.h"
#include "Physics.h"
#include "Collider/CollideHelp.h"
#include "Collider/ColliderCapsule.h"
#include "Collider/ColliderBox.h"

namespace
{
	// 判定数
	constexpr int CHECK_NUM = 4;

	// ゲートのカプセルの長さ
	constexpr float GATE_CAPSULE_SIZE = 4.0f;
	// ゲートの半径
	constexpr float GATE_RADIUS = 2.0f;

	const std::list<ObjectTag> CHECK_TAG_LIST =
	{
		ObjectTag::WALL,
		ObjectTag::FLOOR,
		ObjectTag::ROOF,
		ObjectTag::NO_GATE_WALL,
		ObjectTag::NO_GATE_FLOOR,
		ObjectTag::NO_GATE_ROOF,
		ObjectTag::BUTTON,
		ObjectTag::CATWALK,
		ObjectTag::CATWALK_FENCE,
		ObjectTag::CATWALK_PILLAR,
		ObjectTag::DOOR_ARCH,
		ObjectTag::GATE,
		ObjectTag::LASER_CATCHER,
		ObjectTag::LASER_LAUNCH_PAD,
		ObjectTag::PERFORATED_WALL,
		ObjectTag::PISTON,
		ObjectTag::STAIRS,
	};
}

GateManager::GateManager(const std::shared_ptr<CameraManager>& cameraMgr, const int stageNo) :
	m_cameraMgr(cameraMgr),
	m_gateOrange(nullptr),
	m_gateBlue(nullptr),
	m_stageNo(stageNo),
	m_lastShotKind(GateKind::Orange)
{
	m_isCreate[GateKind::Orange] = false;
	m_isCreate[GateKind::Blue] = false;
}

GateManager::~GateManager()
{
}

void GateManager::Init(const std::weak_ptr<Player>& player)
{
	// プレイヤーのセット
	m_player = player;
	// 固定ゲートの設置
	CreateGateOnTerrain();
}

void GateManager::End()
{
	if (m_isCreate.at(GateKind::Orange))
	{
		m_gateOrange->End();
		m_gateOrange = nullptr;
	}
	if (m_isCreate.at(GateKind::Blue))
	{
		m_gateBlue->End();
		m_gateBlue = nullptr;
	}
}

void GateManager::Update()
{
	// 生成していた場合、更新
	if (m_isCreate.at(GateKind::Blue)) m_gateBlue->Update();
	if (m_isCreate.at(GateKind::Orange)) m_gateOrange->Update();
	// ゲート弾の更新
	for (auto& bullet : m_bulletList) bullet->Update();
	// 壊れた弾の削除
	m_bulletList.remove_if(
		[](const auto& bullet)
		{
			if (bullet->IsBreak())
			{
				bullet->End();
				return true;
			}
			return false;
		});
}

void GateManager::DrawGate(int texA, int texB) const
{
	// 生成していた場合、描画
	if (m_isCreate.at(GateKind::Orange)) m_gateOrange->DrawGate(texA);
	if (m_isCreate.at(GateKind::Blue)) m_gateBlue->DrawGate(texB);
}

void GateManager::Restart()
{
	// ゲートの破棄
	if (m_gateOrange)
	{
		m_gateOrange->End();
		m_gateOrange = nullptr;
	}
	if (m_gateBlue)
	{
		m_gateBlue->End();
		m_gateBlue = nullptr;
	}
	// 未生成に更新
	m_isCreate[GateKind::Orange] = false;
	m_isCreate[GateKind::Blue] = false;
	// ゲート弾を削除
	for (auto& bullet : m_bulletList) bullet->End();
	m_bulletList.clear();
	// 固定ゲートの再設置
	CreateGateOnTerrain();
}

void GateManager::AddBullet(std::shared_ptr<GateBullet> bullet)
{
	// 最後に撃ったゲートの種類を更新
	m_lastShotKind = bullet->GetKind();
	// 弾を追加
	m_bulletList.emplace_back(bullet);
}

void GateManager::CreateGate(GateKind kind, MyEngine::Collidable* collider, const MyEngine::CollideHitInfo& hitInfo, const Vec3& norm, const Vec3& dir)
{
	// ゲートの種類によって生成するゲートを変更
	std::shared_ptr<Gate> gate;
	if (kind == GateKind::Orange)		gate = m_gateOrange;
	else if (kind == GateKind::Blue)	gate = m_gateBlue;
	else								assert(false && "存在しないゲートの種類です");
	
	// 修正用の判定を一時的に生成
	MyEngine::Rigidbody rigid;
	Vec3 pos = hitInfo.hitPos;
	auto col = new MyEngine::ColliderCapsule();
	col->dir = dir;
	col->radius = GATE_RADIUS;
	col->size = GATE_CAPSULE_SIZE;

	int checkCount = 0;
	// 生成時に当たったものを除外データに保存
	MyEngine::Physics::PreHitInfoList_t info;
	info[collider->GetTag()].info = hitInfo;
	info[collider->GetTag()].isCreate = true;
	info[collider->GetTag()].isCheckPallel = true;
	// 当たったタグを除外リストに追加
	std::list<MyEngine::Collidable*> through{ collider, gate.get() };

	// 生成位置に他のオブジェクトがあるかチェック
	MyEngine::Physics::GetInstance().CheckObject(pos, rigid, col, checkCount, CHECK_NUM, true, info, CHECK_TAG_LIST, through);
	// 一時的な判定を削除
	delete col;

	// 生成できる場合
	if (checkCount < CHECK_NUM)
	{
		// ゲートを生成
		if (kind == GateKind::Orange)		CreateGate(m_gateOrange, kind, collider->GetTag(), pos, norm, dir);
		else if (kind == GateKind::Blue)	CreateGate(m_gateBlue, kind, collider->GetTag(), pos, norm, dir);
	}
	// 生成できない場合
	else
	{
		// TODO: エフェクトと失敗SEを流すようにする
	}
}

std::shared_ptr<Gate> GateManager::GetPairGate(GateKind kind) const
{
	// ゲートの種類によってペアゲートを返す
	if (kind == GateKind::Orange)		return m_gateBlue;
	else if (kind == GateKind::Blue)	return m_gateOrange;
	else
	{
		assert(false && "存在しないゲートの種類です");
		// 動作するように片方のゲートを返す
		return m_gateOrange;
	}
}

void GateManager::CreateGateOnTerrain()
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	// 片手ステージでないなら終了
	if (!stageDataMgr.IsOneHand(m_stageNo)) return;

	// 必要データ取得
	const auto& tag = stageDataMgr.GetGateObjectTag(m_stageNo);
	const auto& pos = stageDataMgr.GetGatePos(m_stageNo);
	const auto& norm = stageDataMgr.GetGateNorm(m_stageNo);
	const auto& dir = stageDataMgr.GetGateDir(m_stageNo);
	// ゲート生成
	CreateGate(m_gateBlue, GateKind::Blue, tag, pos, norm, dir);
}

void GateManager::CreateGate(std::shared_ptr<Gate>& gate, GateKind kind, ObjectTag hitTag, const Vec3& pos, const Vec3& norm, const Vec3& dir)
{
	auto pairGate = GetPairGate(kind);
	// ゲートを既に作成している場合
	if (m_isCreate[kind])
	{
		gate->ChangePos(hitTag, pos, norm, dir);
		gate->SetCameraInfo();
		if (pairGate) pairGate->SetCameraInfo();
	}
	// ゲートをまだ作成していない場合
	else
	{
		// ゲートカメラの種類を設定
		CameraKind kind1 = CameraKind::GATE_B;
		CameraKind kind2 = CameraKind::GATE_B_FROM_A;
		if (kind == GateKind::Blue)
		{
			kind1 = CameraKind::GATE_A;
			kind2 = CameraKind::GATE_A_FROM_B;
		}
		// ゲート生成
		gate = std::make_shared<Gate>(std::dynamic_pointer_cast<GateCamera>(m_cameraMgr->GetCamera(kind1)), std::dynamic_pointer_cast<GateCamera>(m_cameraMgr->GetCamera(kind2)), kind);
		// 初期化
		gate->Init(hitTag, pos, norm, dir, m_player);
		// ペアゲートがある場合
		if (pairGate)
		{
			// ペアゲートを設定
			gate->SetPairGate(pairGate);
			pairGate->SetPairGate(gate);

			// カメラ情報を設定
			gate->SetCameraInfo();
			pairGate->SetCameraInfo();
		}
	}

	// 生成済みに更新
	m_isCreate[kind] = true;
}
