#include "SceneHelp.h"
#include "EffekseerManager.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Object/Player.h"
#include "Object/Stage/StageManager.h"
#include "Object/Gate/GateManager.h"

namespace
{
	// レンダーターゲット種類
	enum class RTKind
	{
		GATE_A_TEX,	// ゲートAのテクスチャ
		GATE_B_TEX,	// ゲートBのテクスチャ
		TEMP_1,		// 保存用テクスチャ1
		TEMP_2,		// 保存用テクスチャ2
		MAX,
	};
}

SceneHelp::SceneHelp()
{
}

SceneHelp::~SceneHelp()
{
}

SceneHelp& SceneHelp::GetInstance()
{
	static SceneHelp instance;
	return instance;
}

void SceneHelp::Init()
{
	// 画面生成
	m_rtTable.resize(static_cast<int>(RTKind::MAX));
	for (auto& handle : m_rtTable) handle = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
}

void SceneHelp::End()
{
	// 画面削除
	for (auto& handle : m_rtTable) DeleteGraph(handle);
}

void SceneHelp::DrawModel(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int drawRt, CameraKind mainCamera, std::function<void()> func) const
{
	// -値の場合はバックスクリーンに設定
	if (drawRt < 0) drawRt = DX_SCREEN_BACK;

	// ゲートマネージャーがある場合、両方のゲートがある場合だけゲートからの描画を行う
	bool isDrawGate = false;
	if (gateMgr) isDrawGate = gateMgr->IsCreateBothGates();

	// 両方のゲートを生成していた場合
	if (isDrawGate)
	{
		auto rtGateA = m_rtTable[static_cast<int>(RTKind::GATE_A_TEX)];
		auto rtGateB = m_rtTable[static_cast<int>(RTKind::GATE_B_TEX)];

		// それぞれのゲートから見た景色をゲートに張り付けて描画する
		DrawGateBlend(gateMgr, cameraMgr, stageMgr, player, rtGateA, CameraKind::GATE_A, CameraKind::GATE_A_FROM_B, func);
		DrawGateBlend(gateMgr, cameraMgr, stageMgr, player, rtGateB, CameraKind::GATE_B, CameraKind::GATE_B_FROM_A, func);

		// ゲート内の景色を張り付けてプレイヤーから見た景色を描画
		DrawModelBlend(gateMgr, cameraMgr, stageMgr, player, drawRt, rtGateA, rtGateB, mainCamera, cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps(), true, func);
	}
	// していない場合
	else
	{
		// プレイヤーから見た景色をそのまま描画
		DrawModelBlend(gateMgr, cameraMgr, stageMgr, player, drawRt, -1, -1, mainCamera, cameraMgr->GetCamera(CameraKind::PLAYER)->IsTps(), true, func);
	}
}

void SceneHelp::DrawGateBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, CameraKind gate, CameraKind gateFrom, std::function<void()> func) const
{
	auto rtTemp1 = m_rtTable[static_cast<int>(RTKind::TEMP_1)];
	auto rtTemp2 = m_rtTable[static_cast<int>(RTKind::TEMP_2)];

	// ゲートからゲートを見た景色の描画
	{
		SetDrawScreen(rtTemp1);
		ClearDrawScreen();
		cameraMgr->AppInfo(gateFrom);

		stageMgr->Draw();
		if (player)
		{
			player->Draw();
			player->DrawGun();
		}
	}
	// ゲート内にゲートがある場合の描画を行う
	// MEMO: 1度では不十分な可能性があるので3回は行っておく
	for (int i = 0; i < 3; ++i)
	{
		SetDrawScreen(rtTemp2);
		cameraMgr->AppInfo(gateFrom);
		if (i == 0)
		{
			ClearDrawScreen();
			gateMgr->DrawGate(rtTemp1, rtTemp1);
		}
		else
		{
			gateMgr->DrawGate(rtTemp2, rtTemp2);
		}

		stageMgr->Draw();
		if (player)
		{
			player->Draw();
			player->DrawGun();
		}
	}
	// ゲートからの画面を描画してあげる
	DrawModelBlend(gateMgr, cameraMgr, stageMgr, player, rt, rtTemp2, rtTemp2, gate, true, true, func);
}

void SceneHelp::DrawModelBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, int tex1, int tex2, CameraKind kind, bool isPlayerDraw, bool isEffDraw, std::function<void()> func) const
{
	// 使用RTの決定
	SetDrawScreen(rt);
	ClearDrawScreen();

	// カメラの適用
	cameraMgr->AppInfo(kind);

	// ゲートの描画
	if (gateMgr) gateMgr->DrawGate(tex1, tex2);

	// ステージの描画
	stageMgr->Draw();

	// プレイヤーの描画
	if (player) 
	{
		if (isPlayerDraw) player->Draw();
		player->DrawGun();
	}

	// 独自関数があれば実行
	if (func) func();

	// エフェクトの描画
	if (isEffDraw)
	{
		auto& effMgr = EffekseerManager::GetInstance();
		effMgr.SyncCamera();
		effMgr.Draw();
	}
}
