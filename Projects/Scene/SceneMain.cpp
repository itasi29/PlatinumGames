#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
// 便利系
#include "ScreenUtility.h"
#include "CursorUtility.h"
#include "UIUtility.h"
#include "TimeUtility.h"
#include "MyDraw.h"
// コモン系
#include "Game.h"
#include "Input.h"
#include "CommandList.h"
// マネージャー系
#include "Files.h"
#include "Font.h"
#include "SoundManager.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "SceneManager.h"
#include "EffekseerManager.h"
#include "CameraManager.h"
// その他
#include "SceneHelp.h"
#include "SceneStageSelect.h"
#include "SceneResult.h"
#include "Collidable.h"
#include "Camera.h"
#include "PlayerCamera.h"
#include "Object/Object3DBase.h"
#include "Object/Player.h"
#include "Object/Stage/StageManager.h"
#include "Object/Gate/GateManager.h"

namespace
{
	enum class RTKind
	{
		BEFORE_RESTART,
		AFTER_RESTART,
		MAX,
	};
	enum class DeathUICurrent : int
	{
		CONTINUE,
		RETURN_STAGE_SELECT,
		MAX,
	};

	/* タイムUI */
	constexpr float FILE_SIZE_TIME = 0.5f;				// 画像サイズ
	constexpr float TIME_SIZE = 0.25f;					// 文字サイズ
	constexpr unsigned int COLOR_TIME = 0xffffff;		// カラー
	// バー描画位置
	constexpr int DRAW_TIME_X = 132;
	constexpr int DRAW_TIME_Y = 160;
	// 文字描画位置
	constexpr int DRAW_TIME_STR_X = 80;
	constexpr int DRAW_TIME_STR_Y = 160;

	// サイトの画像サイズ
	constexpr float FILE_SIZE_SITE = 0.09375f;

	/* ゲート関連UI */
	constexpr float FILE_SIZE_CREATE_GATE = 0.375f;					// 画像サイズ
	constexpr float DRAW_CREATE_GATE_ANGLE = 30 * Game::DEG_2_RAD;	// 画像回転具合
	constexpr int SHOT_GATE_RADIUS = 6;								// 最後に発射したゲートの半径
	constexpr int SHOT_GATE_LINE_THICKNESS = 2;						// 最後に発射したゲートの線の太さ
	constexpr unsigned int COLOR_GATE_BLUE = 0x1376b5;				// 青ゲートの色
	constexpr unsigned int COLOR_GATE_ORANGE = 0xf19149;			// オレンジゲートの色
	// 描画位置(青)
	constexpr int DRAW_CREATE_GATE_BLUE_X = 610;
	constexpr int DRAW_CREATE_GATE_BLUE_Y = 330;
	// 描画位置(オレンジ)
	constexpr int DRAW_CREATE_GATE_ORANGE_X = 670;
	constexpr int DRAW_CREATE_GATE_ORANGE_Y = 390;
	// 描画位置(青、最後に発射時)
	constexpr int DRAW_SHOT_GATE_BLUE_X = 575;
	constexpr int DRAW_SHOT_GATE_BLUE_Y = 368;
	// 描画位置(オレンジ、最後に発射時)
	constexpr int DRAW_SHOT_GATE_ORANGE_X = 705;
	constexpr int DRAW_SHOT_GATE_ORANGE_Y = 352;

	/* 死亡UI(文字列) */
	const wchar_t* const STR_DEATH_WINDOW = L"ＹＯＵ　ＤＩＥＤ";	// 文字列
	constexpr int DRAW_DEATH_WINDOW_STR_SUB_Y = 100;				// ウィンドウとの差分
	constexpr int FONT_SIZE_DEATH_WINDOW = 48;						// フォントサイズ
	constexpr unsigned int COLOR_DEATH_WINDOW = 0xffffff;			// カラー
	// 選択文字列
	const std::vector<std::wstring> STR_DEATH =
	{
		L"リスタート",
		L"ステージ選択へ"
	};
	/* 死亡UI(画像) */
	constexpr int DRAW_DEATH_UI_FRAME_X_INTERVL = 300;	// 描画間隔
	constexpr int DRAW_DEATH_UI_OUT_VAL = 360;			// 画面外位置
	constexpr float FILE_SIZE_DEATH_UI_WINDOW = 0.5f;	// ウィンドウサイズ
	constexpr float FILE_SIZE_DEATH_UI = 0.75f;			// フレームサイズ
	constexpr int FONT_SIZE_DEATH_UI = 32;				// フォントサイズ
	constexpr unsigned int COLOR_SELECT = 0x000000;		// 選択文字の色
	constexpr unsigned int COLOR_NOT_SELECT = 0xffffff;	// 未選択文字の色
	constexpr int WAVE_SIZE_DEATH_UI = 16;				// 文字ウェーブの大きさ
	constexpr float WAVE_SPEED_DEATH_UI = 2.0f;			// 文字ウェーブのスピード
	// 死亡UIの描画位置
	constexpr int DRAW_DEATH_UI_FRAME_X = 380;
	constexpr int DRAW_DEATH_UI_FRAME_Y = 450;

	/* リスタート用 */
	constexpr int RESTART_FRAME = 30;						// かかるフレーム
	constexpr float RESTART_SPEED = 1.0f / RESTART_FRAME;	// フレーム増加スピード
	// リスタート時のカラーDissolveの色
	constexpr float COLOR_FIRE_R = 0.09f;
	constexpr float COLOR_FIRE_G = 0.59f;
	constexpr float COLOR_FIRE_B = 0.59f;
}

SceneMain::SceneMain(const int stageNo) :
	SceneBase(SceneKind::MAIN),
	m_updateFunc(&SceneMain::MainUpdate),
	m_drawFunc(&SceneMain::DrawMain),
	m_stageNo(stageNo),
	m_cBuff(nullptr),
	m_cBuffH(-1),
	m_time(0),
	m_current(0),
	m_count(0)
{
}

void SceneMain::AsyncInit()
{
	// ファイルの読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[PS_GATE]					= fileMgr.Load(PS_GATE);
	m_files[I_TIME_BAR]					= fileMgr.Load(I_TIME_BAR);
	m_files[I_LOOK_SITE]				= fileMgr.Load(I_LOOK_SITE);
	m_files[I_CREATE_GATE_ORANGE]		= fileMgr.Load(I_CREATE_GATE_ORANGE);
	m_files[I_CREATE_GATE_BLUE]			= fileMgr.Load(I_CREATE_GATE_BLUE);
	m_files[I_NOT_CREATE_GATE_ORANGE]	= fileMgr.Load(I_NOT_CREATE_GATE_ORANGE);
	m_files[I_NOT_CREATE_GATE_BLUE]		= fileMgr.Load(I_NOT_CREATE_GATE_BLUE);
	m_files[I_OPTION_WINDOW]			= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_COMMON_FRAME]				= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_COMMON_SELECT_FRAME]		= fileMgr.Load(I_COMMON_SELECT_FRAME);
	m_files[I_DISSOLVE]					= fileMgr.Load(I_DISSOLVE);
	m_files[B_MAIN]						= fileMgr.Load(B_MAIN);
	m_files[PS_DISSOLVE]				= fileMgr.Load(PS_DISSOLVE);
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.LoadFile(E_BULLET);
	effMgr.LoadFile(E_GATE_BULLET_BLUE);
	effMgr.LoadFile(E_GATE_BULLET_ORANGE);
	effMgr.LoadFile(E_DEATH_TURRET);
	effMgr.LoadFile(E_MUZZLE_FLASH);
	effMgr.LoadFile(E_GATE_DELETE);
	effMgr.LoadFile(E_LASER_BULLET);
	// 画面生成
	m_rtTable.resize(static_cast<int>(RTKind::MAX));
	for (auto& handle : m_rtTable) handle = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
	// VRAMにメモリを確保
	m_cBuffH = CreateShaderConstantBuffer(sizeof(CBuff));
	m_cBuff = static_cast<CBuff*>(GetBufferShaderConstantBuffer(m_cBuffH));

	// マネージャー生成
	m_stageMgr = std::make_shared<StageManager>(m_stageNo);
	m_cameraMgr = std::make_shared<CameraManager>();
	m_gateMgr = std::make_shared<GateManager>(m_cameraMgr, m_stageNo);
	// プレイヤー生成
	m_player = std::make_shared<Player>(std::dynamic_pointer_cast<PlayerCamera>(m_cameraMgr->GetCamera(CameraKind::PLAYER)), m_gateMgr);
	// 初期化(非同期)
	m_stageMgr->AsyncInit();
	m_player->AsyncInit();
}

void SceneMain::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_MAIN)->GetHandle());
	// UI生成
	m_deathUIWindow = UIMoveData::Make(Game::CENTER_X, -DRAW_DEATH_UI_OUT_VAL);
	m_deathUIList = UIMoveData::Make(static_cast<int>(DeathUICurrent::MAX), DRAW_DEATH_UI_FRAME_X, -DRAW_DEATH_UI_OUT_VAL, DRAW_DEATH_UI_FRAME_X_INTERVL, 0);
	// 初期化(同期)
	m_stageMgr->Init(m_player.get(), m_gateMgr.get());
	m_stageMgr->AppLights();
	m_gateMgr->Init(m_player);
	SoundManager::GetInstance().SetSeCenter(m_player);
	m_player->Init(m_stageMgr->GetCheckPoint(), m_stageMgr->GetCheckPointDir(), StageDataManager::GetInstance().IsOneHand(m_stageNo));
	m_cBuff->fireRed	= COLOR_FIRE_R;
	m_cBuff->fireGreen	= COLOR_FIRE_G;
	m_cBuff->fireBlue	= COLOR_FIRE_B;
}

void SceneMain::End()
{
	m_gateMgr->End();
	m_stageMgr->End();
	m_cameraMgr->End();
	m_player->End();

	DeleteShaderConstantBuffer(m_cBuffH);
	for (auto& handle : m_rtTable) DeleteGraph(handle);
	EffekseerManager::GetInstance().ReleaseFile();
	SoundManager::GetInstance().ResetSeCenter();
}

void SceneMain::Update(bool isFade)
{
	// UI更新
	++m_count;
	m_deathUIWindow->Update();
	for (auto& data : m_deathUIList) data->Update();
	// カメラ更新
	m_cameraMgr->Update();

	if (isFade) return;
	(this->*m_updateFunc)();
}

void SceneMain::Draw() const
{
	(this->*m_drawFunc)();
}

void SceneMain::OnRestart()
{
	const auto& help = SceneHelp::GetInstance();
	// リスタート前の画面を保存
	int beforeRt = m_rtTable.at(static_cast<int>(RTKind::BEFORE_RESTART));
	help.DrawModel(m_gateMgr, m_cameraMgr, m_stageMgr, m_player, beforeRt);
	// 各種リスタート処理
	m_player->Restart(m_stageMgr->GetCheckPoint(), m_stageMgr->GetCheckPointDir());
	m_gateMgr->Restart();
	m_stageMgr->Restart();
	// リスタート後の画面を保存
	int afterRt = m_rtTable.at(static_cast<int>(RTKind::AFTER_RESTART));
	help.DrawModel(m_gateMgr, m_cameraMgr, m_stageMgr, m_player, afterRt);
	// 描画先をバックスクリーンに戻す
	SetDrawScreen(DX_SCREEN_BACK);
	// 初期化
	m_cBuff->frame = 0;
	// 関数先変更
	m_updateFunc = &SceneMain::RestartUpdate;
	m_drawFunc = &SceneMain::DrawRestart;
}

void SceneMain::MainUpdate()
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::MAIN, this);
		return;
	}
	// 各種更新
	m_gateMgr->Update();
	m_stageMgr->Update();
	m_player->Update();
	// 時間更新
	++m_time;
	// クリア処理
	if (m_stageMgr->CheckClear())
	{
		saveDataMgr.OnClear(m_stageNo - 1, m_time);
		auto next = std::make_shared<SceneResult>(m_stageNo, m_time);
		m_scnMgr.Change(next);
		return;
	}
	// 死亡したら更新先変更
	if (m_player->IsDeath())
	{
		m_updateFunc = &SceneMain::GameOverUpdate;
		UIMoveData::ChangeVertical(m_deathUIList, DRAW_DEATH_UI_FRAME_Y);
		m_deathUIWindow->ChangeVertical(Game::CENTER_Y);
		m_count = 0;
	}
}

void SceneMain::GameOverUpdate()
{
	if (CursorUtility::CursorUp<DeathUICurrent>(m_current, DeathUICurrent::MAX, Command::BTN_LEFT))		m_count = 0;
	if (CursorUtility::CursorDown<DeathUICurrent>(m_current, DeathUICurrent::MAX, Command::BTN_RIGHT))	m_count = 0;

	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OK))
	{
		if (m_current == static_cast<int>(DeathUICurrent::CONTINUE))
		{
			OnRestart();
			UIMoveData::ChangeVertical(m_deathUIList, -DRAW_DEATH_UI_OUT_VAL);
			m_deathUIWindow->ChangeVertical(-DRAW_DEATH_UI_OUT_VAL);
		}
		else if (m_current == static_cast<int>(DeathUICurrent::RETURN_STAGE_SELECT))
		{
			auto next = std::make_shared<SceneStageSelect>();
			m_scnMgr.Change(next);
		}
		else
		{
			assert(false);
		}
		return;
	}

	// 各種更新(見た目だけ更新)
	m_gateMgr->Update();
	m_stageMgr->Update();
	m_player->Update();
}

void SceneMain::RestartUpdate()
{
	m_cBuff->frame += RESTART_SPEED;

	if (m_cBuff->frame > 1.0f)
	{
		// 関数先の変更
		m_updateFunc = &SceneMain::MainUpdate;
		m_drawFunc = &SceneMain::DrawMain;
	}
}

void SceneMain::DrawMain() const
{
	// ゲートの生成状況に合わせてモデルを描画
	SceneHelp::GetInstance().DrawModel(m_gateMgr, m_cameraMgr, m_stageMgr, m_player);
	// UI描画
	DrawUI();
}

void SceneMain::DrawRestart() const
{
	int beforeRt = m_rtTable.at(static_cast<int>(RTKind::BEFORE_RESTART));
	int afterRt = m_rtTable.at(static_cast<int>(RTKind::AFTER_RESTART));
	int dissolveH = m_files.at(I_DISSOLVE)->GetHandle();
	int dissolvePS = m_files.at(PS_DISSOLVE)->GetHandle();

	UpdateShaderConstantBuffer(m_cBuffH);
	SetShaderConstantBuffer(m_cBuffH, DX_SHADERTYPE_PIXEL, 4);
	MyEngine::DrawGraph(0, 0, dissolvePS, beforeRt, afterRt, dissolveH);
}

void SceneMain::DrawUI() const
{
	SetUseZBuffer3D(false);
	DrawCommonUI();
	DrawExistUI();
	DrawDeathUI();
	SetUseZBuffer3D(true);
}

void SceneMain::DrawCommonUI() const
{
	// 時間描画
	DrawRotaGraphFast(DRAW_TIME_X, DRAW_TIME_Y, FILE_SIZE_TIME, 0.0f, m_files.at(I_TIME_BAR)->GetHandle(), true);
	TimeUtility::DrawTime(DRAW_TIME_STR_X, DRAW_TIME_STR_Y, m_time, TIME_SIZE, COLOR_TIME);
	// プレイヤー情報描画
	m_player->DrawHpUI();
	m_player->DrawPadUI();
	m_player->DrawDamageFillter();
}

void SceneMain::DrawExistUI() const
{
	// プレイヤーが死んでいる場合は処理終了
	if (m_player->IsDeath()) return;
	auto& stageDataMgr = StageDataManager::GetInstance();

	// サイトの描画
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, FILE_SIZE_SITE, 0.0f, m_files.at(I_LOOK_SITE)->GetHandle(), true);

	int handle;
	// オレンジゲート
	if (m_gateMgr->IsCreate(GateKind::Orange))	handle = m_files.at(I_CREATE_GATE_ORANGE)->GetHandle();
	else										handle = m_files.at(I_NOT_CREATE_GATE_ORANGE)->GetHandle();
	DrawRotaGraphFast(DRAW_CREATE_GATE_ORANGE_X, DRAW_CREATE_GATE_ORANGE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);
	// 片手ステージなら
	if (stageDataMgr.IsOneHand(m_stageNo))
	{
		constexpr int DRAW_CREATE_GATE_ONE_HAND_X = 610;
		constexpr int DRAW_CREATE_GATE_ONE_HAND_Y = 330;
		constexpr float DRAW_CREATE_GATE_ANGLE_ONE_HAND = 210 * Game::DEG_2_RAD;
		// オレンジゲートを描画
		if (m_gateMgr->IsCreate(GateKind::Orange))	handle = m_files.at(I_CREATE_GATE_ORANGE)->GetHandle();
		else										handle = m_files.at(I_NOT_CREATE_GATE_ORANGE)->GetHandle();
		DrawRotaGraphFast(DRAW_CREATE_GATE_ONE_HAND_X, DRAW_CREATE_GATE_ONE_HAND_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE_ONE_HAND, handle, true);
	}
	// 両手ステージなら
	else
	{
		// 青ゲートを描画
		if (m_gateMgr->IsCreate(GateKind::Blue))	handle = m_files.at(I_CREATE_GATE_BLUE)->GetHandle();
		else										handle = m_files.at(I_NOT_CREATE_GATE_BLUE)->GetHandle();
		DrawRotaGraphFast(DRAW_CREATE_GATE_BLUE_X, DRAW_CREATE_GATE_BLUE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);
	}

	int x = DRAW_SHOT_GATE_BLUE_X;
	int y = DRAW_SHOT_GATE_BLUE_Y;
	unsigned int color = COLOR_GATE_BLUE;
	GateKind kind = GateKind::Blue;
	if (m_gateMgr->GetLastShotKind() == GateKind::Orange)
	{
		x = DRAW_SHOT_GATE_ORANGE_X;
		y = DRAW_SHOT_GATE_ORANGE_Y;
		color = COLOR_GATE_ORANGE;
		kind = GateKind::Orange;
	}
	// HACK: 何故かLineThicknessを変更した際に塗りつぶしができない為2個重ねている
	DrawCircle(x, y, SHOT_GATE_RADIUS, color, false, SHOT_GATE_LINE_THICKNESS);
	if (m_gateMgr->IsCreate(kind))  DrawCircle(x, y, SHOT_GATE_RADIUS, color, true);
}

void SceneMain::DrawDeathUI() const
{
	// プレイヤーが生きている場合は処理終了
	if (!m_player->IsDeath()) return;

	// 画像ハンドル
	int selectH = m_files.at(I_COMMON_SELECT_FRAME)->GetHandle();
	int notSelectH = m_files.at(I_COMMON_FRAME)->GetHandle();
	// 描画
	DrawRotaGraphFast(m_deathUIWindow->x, m_deathUIWindow->y, FILE_SIZE_DEATH_UI_WINDOW, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	int x = UIUtility::GetSubCenterFromLen(m_deathUIWindow->x, STR_DEATH_WINDOW, FONT_SIZE_DEATH_WINDOW);
	DrawStringToHandle(x, m_deathUIWindow->y - DRAW_DEATH_WINDOW_STR_SUB_Y, STR_DEATH_WINDOW, COLOR_DEATH_WINDOW, FontManager::GetInstance().GetHandle(FONT_KAISOTAI, FONT_SIZE_DEATH_WINDOW));
	UIUtility::DrawFrameAndStr(m_deathUIList, FILE_SIZE_DEATH_UI, 0.0f, FONT_SIZE_DEATH_UI, m_current, STR_DEATH, selectH, notSelectH, COLOR_SELECT, COLOR_NOT_SELECT, UIUtility::DrawStrType::WAVE, WAVE_SIZE_DEATH_UI, m_count * WAVE_SPEED_DEATH_UI);
}
