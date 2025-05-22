#include "SceneTutorial.h"
#include <DxLib.h>
#include "StringUtility.h"
#include "Input.h"
#include "Files.h"
#include "CommandList.h"
#include "TimeUtility.h"
#include "SceneManager.h"
#include "SceneResult.h"
#include "SceneHelp.h"
#include "MyDraw.h"
#include "NovelManager.h"
#include "EffekseerManager.h"
#include "SoundManager.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "CameraManager.h"
#include "PlayerCamera.h"
#include "Object/Player.h"
#include "Object/Gate/GateManager.h"
#include "Object/Stage/StageManager.h"

namespace
{
	// レンダーターゲット種類
	enum class RTKind
	{
		BEFORE_RESTART,
		AFTER_RESTART,
		MAX,
	};
	// ファイル要素番号
	enum class FileIndex
	{
		IS_DRAW_CLEAR,
		KIND,
		INFO_1,
		INFO_2,
		INFO_3,
		IS_DRAW_ALLOW,
		ALLOW_POS,
		IS_DRAW_GOAL,
		GOAL_TEXT_ID,
		IS_DRAW_NOVEL,
		NOVEL_ID,
	};

	// データマスターパス
	const wchar_t* const MASTER_PATH = L"Data/Master/TutorialData.csv";
	// ステージ名
	const wchar_t* const STAGE_NAME = L"チュートリアル";
	// ステージ番号
	constexpr int STAGE_NO = 1;

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

	/* リスタート用 */
	constexpr int RESTART_FRAME = 30;						// かかるフレーム
	constexpr float RESTART_SPEED = 1.0f / RESTART_FRAME;	// フレーム増加スピード
	// リスタート時のカラーDissolveの色
	constexpr float COLOR_FIRE_R = 0.09f;
	constexpr float COLOR_FIRE_G = 0.59f;
	constexpr float COLOR_FIRE_B = 0.59f;

	/* ノベルデータUI */
	constexpr int UI_MOVE_FRAME = 20;			// UI移動フレーム
	constexpr int UI_STAY_FRAME = 40;			// UI移動ステイフレーム
	constexpr int TEXT_SPACE_W = 64;			// テキスト画像の空白スペース
	constexpr int TEXT_MOVE_CIRCLE = 60 * 20;	// テキストの移動サイクル
	constexpr int TEXT_DRAW_RANGE_W = 375;		// テキスト画像の描画範囲
	// フレーム描画位置
	constexpr int DRAW_TUTORIAL_FRAME_X = 1050;	
	constexpr int DRAW_TUTORIAL_FRAME_Y = 48;
	// フレーム移動時増加量
	constexpr int MOVE_TUTORIAL_FRAME_X = Game::CENTER_X - DRAW_TUTORIAL_FRAME_X;	
	constexpr int MOVE_TUTORIAL_FRAME_Y = Game::CENTER_Y - DRAW_TUTORIAL_FRAME_Y;
	// 移動後のテキスト画像描画位置
	constexpr int DRAW_TEXT_RECT_X = 866;		
	constexpr int DRAW_TEXT_RECT_Y = 34;

	/* クリア演出系 */
	constexpr int STAMP_FIRST_FRAME = 20;								// 縮小フレーム1
	constexpr int STAMP_SECOND_FRAME = STAMP_FIRST_FRAME + 7;			// 拡大フレーム1
	constexpr int TOTAL_STAMP_FRAME = 30;								// 合計スタンプフレーム
	constexpr int CHANGE_NEXT_TUTORIAL_FRAME = TOTAL_STAMP_FRAME + 30;	// 次のチュートリアルへの変更フレーム
	// 描画位置
	constexpr int DRAW_PERFECT_X = Game::CENTER_X;
	constexpr int DRAW_PERFECT_Y = 300;
	// 画像サイズ
	constexpr float FILE_SIZE_PERFECT = 1.0f;
	constexpr float FILE_SIZE_PERFECT_LARGE = 1.3f;
	constexpr float FILE_SIZE_PERFECT_LARGE_2 = 1.1f;
	constexpr float FILE_SIZE_PERFECT_SMALL = 0.9f;

	/* ノベルデータUI関係 */
	constexpr int NOVEL_SPACE_NUM = 2;					// 左右空白文字数
	constexpr int DRAW_NOVEL_Y = 460;					// 文字描画位置
	constexpr int DRAW_NOVEL_GRAPH_Y = 218;				// 画像描画位置Y
	constexpr int FONT_SIZE_NOVEL = 36;					// フォントサイズ
	constexpr unsigned int COLOR_NOVEL = 0xffffff;		// 文字列カラー
	constexpr float FILE_SIZE_NOVEL_FRAME_Y = 0.375f;	// フレーム画像サイズ
	constexpr float ADD_FILE_SIZE_TUTORIAL_FRAME = 1.0f;	// 画像増加サイズ
	// フレーム描画位置
	constexpr int DRAW_NOVEL_FRAME_X = 37;				
	constexpr int DRAW_NOVEL_FRAME_Y = 437;
	constexpr int DRAW_NOVEL_BUTTON_Y = 648;	// Y描画位置
	constexpr int DRAW_NOVEL_BUTTON_X = 1208;			// PadUI描画位置
	constexpr float FILE_SIZE_NOVEL_BUTTON = 0.5f;		// PadUI描画サイズ
	constexpr float SCALING_NOVEEL_BUTTON = 0.0625f;		// PadUI拡大サイズ
	constexpr float SCALING_SPEED_NOVEL_BUTTON = 0.04f;	// PadUI拡大スピード

	/* 移動先描画矢印関係 */
	constexpr float FILE_SIZE_DESTINATION_ARROW = 2.0f;				// 画像サイズ
	constexpr float ANGLE_DESTINATION_ARROW = 90 * Game::DEG_2_RAD;	// 回転度
	constexpr float SHAKE_SPEED_DESTINATION_ARROW = 0.125f;			// 揺らしスピード
	constexpr float SHAKE_SIZE_DESTINATION_ARROW = 0.5f;			// 揺らしサイズ

	// プレイヤーの対応ステート表
	const std::unordered_map<std::string, PlayerState> STATE =
	{
		{ "walk",	PlayerState::WALK },
		{ "run",	PlayerState::RUN },
		{ "jump",	PlayerState::LANDING },
	};
}

SceneTutorial::SceneTutorial() :
	SceneBase(SceneKind::TUTORIAL),
	m_updateFunc(&SceneTutorial::MainUpdate),
	m_tutorialUpdateFunc(nullptr),
	m_drawFunc(&SceneTutorial::DrawMain),
	m_cBuff(nullptr),
	m_cBuffH(-1),
	m_time(0),
	m_index(0),
	m_checkFrame(0),
	m_stampFrame(0),
	m_uiFrame1(0),
	m_uiFrame2(0),
	m_isDrawGoal(false),
	m_isDrawClear(false)
{
}

SceneTutorial::~SceneTutorial()
{
}

void SceneTutorial::AsyncInit()
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
	m_files[I_TUTORIAL_FRAME]			= fileMgr.Load(I_TUTORIAL_FRAME);
	m_files[I_OPTION_WINDOW]			= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_RANKING_ARROW]			= fileMgr.Load(I_RANKING_ARROW);
	m_files[I_COMMON_FRAME]				= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_PAD_A]					= fileMgr.Load(I_PAD_A);
	m_files[I_TUTORIAL_PERFECT]			= fileMgr.Load(I_TUTORIAL_PERFECT);
	m_files[I_DISSOLVE]					= fileMgr.Load(I_DISSOLVE);
	m_files[B_MAIN]						= fileMgr.Load(B_MAIN);
	m_files[PS_DISSOLVE]				= fileMgr.Load(PS_DISSOLVE);
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.LoadFile(E_BULLET);
	effMgr.LoadFile(E_GATE_BULLET_BLUE);
	effMgr.LoadFile(E_GATE_BULLET_ORANGE);
	effMgr.LoadFile(E_LASER_BULLET);
	// VRAMにメモリを確保
	m_cBuffH = CreateShaderConstantBuffer(sizeof(CBuff));
	m_cBuff = static_cast<CBuff*>(GetBufferShaderConstantBuffer(m_cBuffH));
	// 画面生成
	m_rtTable.resize(static_cast<int>(RTKind::MAX));
	for (auto& handle : m_rtTable) handle = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);

	// マネージャー生成
	m_stageMgr = std::make_shared<StageManager>(STAGE_NO);
	m_cameraMgr = std::make_shared<CameraManager>();
	m_gateMgr = std::make_shared<GateManager>(m_cameraMgr, STAGE_NO);
	// プレイヤー生成
	m_player = std::make_shared<Player>(std::dynamic_pointer_cast<PlayerCamera>(m_cameraMgr->GetCamera(CameraKind::PLAYER)), m_gateMgr);
	// 初期化(非同期)
	m_stageMgr->AsyncInit();
	m_player->AsyncInit();
	Load();
}

void SceneTutorial::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_MAIN)->GetHandle());
	// 初期化(同期)
	m_stageMgr->Init(m_player.get(), m_gateMgr.get());
	m_stageMgr->AppLights();
	m_gateMgr->Init(m_player);
	SoundManager::GetInstance().SetSeCenter(m_player);
	m_player->Init(m_stageMgr->GetCheckPoint(), m_stageMgr->GetCheckPointDir(), StageDataManager::GetInstance().IsOneHand(STAGE_NO));
	m_cBuff->fireRed = COLOR_FIRE_R;
	m_cBuff->fireGreen = COLOR_FIRE_G;
	m_cBuff->fireBlue = COLOR_FIRE_B;
}

void SceneTutorial::End()
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

void SceneTutorial::Update(bool isFade)
{
	m_cameraMgr->Update();
	++m_uiFrame2;
	if (isFade) return;

	if (m_scnMgr.IsFadeEndFrame()) ChangeTutorial();

	auto& novelMgr = NovelManager::GetInstance();
	if (!novelMgr.IsStop())
	{
		novelMgr.Update();
		return;
	}
	// ノベル情報を停止していなければステージの情報は更新しない
	--m_uiFrame1;
	(this->*m_updateFunc)();
	(this->*m_tutorialUpdateFunc)();
}

void SceneTutorial::Draw() const
{
	(this->*m_drawFunc)();
}

void SceneTutorial::OnRestart()
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
	m_updateFunc = &SceneTutorial::RestartUpdate;
	m_drawFunc = &SceneTutorial::DrawRestart;
}

void SceneTutorial::MainUpdate()
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::TUTORIAL, this);
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
		saveDataMgr.OnClear(STAGE_NO - 1, m_time);
		auto next = std::make_shared<SceneResult>(STAGE_NO, m_time);
		m_scnMgr.Change(next);
		return;
	}
}

void SceneTutorial::RestartUpdate()
{
	m_cBuff->frame += RESTART_SPEED;

	if (m_cBuff->frame > 1.0f)
	{
		// 関数先の変更
		m_updateFunc = &SceneTutorial::MainUpdate;
		m_drawFunc = &SceneTutorial::DrawMain;
	}
}

void SceneTutorial::Load()
{
	const auto& table = FileManager::GetInstance().LoadCsv(MASTER_PATH);
	for (const auto& item : table)
	{
		TutorialInfo info;
		// クリア演出描画フラグ取得
		info.isDrawClear = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_DRAW_CLEAR))));
		// チュートリアルの種類
		info.kind = static_cast<TutorialKind>(std::stoi(item.at(static_cast<int>(FileIndex::KIND))));
		// 種類に合わせて読み込み
		if (info.kind == TutorialKind::STATE)
		{
			info.info = CheckStateInfo();
			auto& state = std::get<CheckStateInfo>(info.info);
			// 判定ステート取得
			state.state = STATE.at(item.at(static_cast<int>(FileIndex::INFO_1)));
			// 判定フレーム取得
			state.frame = std::stoi(item.at(static_cast<int>(FileIndex::INFO_2)));
		}
		else if (info.kind == TutorialKind::INPUT)
		{
			info.info = CheckInputInfo();
			auto& input = std::get<CheckInputInfo>(info.info);
			// Triggerで判定するか判定
			input.isTrigger = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::INFO_1))));
			// Triggerで判定する場合
			if (input.isTrigger)
			{
				// Triggerの種類取得
				input.type = static_cast<CheckInputInfo::TriggerType>(std::stoi(item.at(static_cast<int>(FileIndex::INFO_2))));
				// 判定フレーム取得
				input.frame = std::stoi(item.at(static_cast<int>(FileIndex::INFO_3)));
			}
			// Triggerで判定しない場合
			else
			{
				// コマンドを取得
				input.command = item.at(static_cast<int>(FileIndex::INFO_2));
			}
		}
		else if (info.kind == TutorialKind::CP)
		{
			info.info = CheckCPInfo();
			auto& cp = std::get<CheckCPInfo>(info.info);
			// 判定チェックポイント取得
			cp.cp = std::stoi(item.at(static_cast<int>(FileIndex::INFO_1)));
		}
		// 矢印描画フラグ取得
		info.isDrawDestination = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_DRAW_ALLOW))));
		if (info.isDrawDestination)
		{
			// 矢印描画位置取得
			info.destination = StringUtility::CsvToVec3(item.at(static_cast<int>(FileIndex::ALLOW_POS)));
		}
		// 目的テキスト描画フラグ取得
		info.isDrawGoal = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_DRAW_GOAL))));
		if (info.isDrawGoal)
		{
			// 画像ID取得
			info.goalTextId = std::stoi(item.at(static_cast<int>(FileIndex::GOAL_TEXT_ID)));
			// 画像をロード
			m_files[info.goalTextId] = FileManager::GetInstance().Load(info.goalTextId);
		}
		// ノベル描画フラグ取得
		info.isDrawNovel = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_DRAW_NOVEL))));
		if (info.isDrawNovel)
		{
			// ノベルID取得
			info.novelId = std::stoi(item.at(static_cast<int>(FileIndex::NOVEL_ID)));
		}

		m_tutorialInfo.push_back(info);
	}
}

void SceneTutorial::CheckStateUpdate()
{
	const auto nowState = m_player->GetNowState();
	const auto& info = std::get<CheckStateInfo>(m_tutorialInfo.at(m_index).info);

	// 同じステートなら
	if (info.state == nowState)
	{
		// そのステートをしている時間増加
		++m_checkFrame;
		// 設定した時間を超えたら次のチュートリアルへ
		if (m_checkFrame > info.frame) OnClearTutorial();
	}
}

void SceneTutorial::CheckInputUpdate()
{
	const auto& info = std::get<CheckInputInfo>(m_tutorialInfo.at(m_index).info);
	const auto& input = Input::GetInstance();

	// トリガー入力
	if (info.isTrigger)
	{
		const auto& trigger = input.GetTriggerData();
		// 左スティック
		if (info.type == CheckInputInfo::TriggerType::STICK_LEFT)
		{
			if (trigger.LStick.SqLength() > 0) ++m_checkFrame;
		}
		// 右スティック
		else if (info.type == CheckInputInfo::TriggerType::STICK_RIGHT)
		{
			if (trigger.RStick.SqLength() > 0) ++m_checkFrame;
		}
		// トリガー
		else
		{
			if (trigger.LT != 0 || trigger.RT != 0) ++m_checkFrame;
		}

		// 設定した時間を超えたら次のチュートリアルへ
		if (m_checkFrame > info.frame) OnClearTutorial();
	}
	// ボタン入力
	else
	{
		if (input.IsTriggerd(info.command.c_str())) OnClearTutorial();
	}
}

void SceneTutorial::CheckCPUpdate()
{
	const auto nowCP = m_stageMgr->GetNowCp();
	const auto& info = std::get<CheckCPInfo>(m_tutorialInfo.at(m_index).info);

	// 現在のCPが予定のCPを超えたら次のチュートリアルへ
	if (nowCP >= info.cp)
	{
		OnClearTutorial();
	}
}

void SceneTutorial::ClearTutorialUpdate()
{
	// TODO: OK!描画終わったら次へ行くようにする
	++m_stampFrame;

	if (m_stampFrame > CHANGE_NEXT_TUTORIAL_FRAME) ChangeTutorial();
}

void SceneTutorial::OnClearTutorial()
{
	const auto& info = m_tutorialInfo.at(m_index);
	// フラグ更新
	m_isDrawGoal = false;
	m_isDrawClear = info.isDrawClear;
	// 次のチュートリアルへ
	++m_index;
	// クリア演出がある場合
	if (m_isDrawClear)
	{
		// クリア演出へ
		m_stampFrame = 0;
		m_tutorialUpdateFunc = &SceneTutorial::ClearTutorialUpdate;
	}
	// ない場合
	else
	{
		ChangeTutorial();
	}
}

void SceneTutorial::ChangeTutorial()
{
	const auto& info = m_tutorialInfo.at(m_index);
	// フラグ更新
	m_isDrawClear = false;
	m_isDrawGoal = info.isDrawGoal;
	// ノベルデータがある場合
	if (info.isDrawNovel)
	{
		// プレイヤーを停止
		m_player->OnStop();
		// ノベルを始める
		NovelManager::GetInstance().Start(info.novelId);
	}
	// フレーム初期化
	m_checkFrame = 0;
	m_uiFrame1 = UI_MOVE_FRAME + UI_STAY_FRAME;
	// 次のチュートリアルに関数を変更
	const auto kind = info.kind;
	if (kind == TutorialKind::STATE)
	{
		m_tutorialUpdateFunc = &SceneTutorial::CheckStateUpdate;
	}
	else if (kind == TutorialKind::INPUT)
	{
		m_tutorialUpdateFunc = &SceneTutorial::CheckInputUpdate;
	}
	else if (kind == TutorialKind::CP)
	{
		m_tutorialUpdateFunc = &SceneTutorial::CheckCPUpdate;
	}
}

void SceneTutorial::DrawMain() const
{
	auto func = std::bind(&SceneTutorial::DrawTutorialDestination, this);
	// ゲートの生成状況に合わせてモデルを描画
	SceneHelp::GetInstance().DrawModel(m_gateMgr, m_cameraMgr, m_stageMgr, m_player, DX_SCREEN_BACK, CameraKind::PLAYER, func);
	// UI描画
	DrawUI();
}

void SceneTutorial::DrawRestart() const
{
	int beforeRt = m_rtTable.at(static_cast<int>(RTKind::BEFORE_RESTART));
	int afterRt = m_rtTable.at(static_cast<int>(RTKind::AFTER_RESTART));
	int dissolveH = m_files.at(I_DISSOLVE)->GetHandle();
	int dissolvePS = m_files.at(PS_DISSOLVE)->GetHandle();

	// シェーダー適用して描画
	UpdateShaderConstantBuffer(m_cBuffH);
	SetShaderConstantBuffer(m_cBuffH, DX_SHADERTYPE_PIXEL, 4);
	MyEngine::DrawGraph(0, 0, dissolvePS, beforeRt, afterRt, dissolveH);
}

void SceneTutorial::DrawUI() const
{
	auto& novelMgr = NovelManager::GetInstance();
	if (novelMgr.IsStop())
	{
		// 時間描画
		DrawRotaGraphFast(DRAW_TIME_X, DRAW_TIME_Y, FILE_SIZE_TIME, 0.0f, m_files.at(I_TIME_BAR)->GetHandle(), true);
		TimeUtility::DrawTime(DRAW_TIME_STR_X, DRAW_TIME_STR_Y, m_time, TIME_SIZE, COLOR_TIME);
		// プレイヤー情報描画
		m_player->DrawPadUI();
		m_player->DrawHpUI();
		m_player->DrawDamageFillter();
		// サイト描画
		DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, FILE_SIZE_SITE, 0.0f, m_files.at(I_LOOK_SITE)->GetHandle(), true);
		// ゲートUI
		DrawGateUI();
	}
	// チュートリアルUI
	DrawTutorialClear();
	DrawTutorialGoal();
	DrawTutorialNovel();
}

void SceneTutorial::DrawGateUI() const
{
	int handle;
	// 青ゲート
	if (m_gateMgr->IsCreate(GateKind::Blue))	handle = m_files.at(I_CREATE_GATE_BLUE)->GetHandle();
	else										handle = m_files.at(I_NOT_CREATE_GATE_BLUE)->GetHandle();
	DrawRotaGraphFast(DRAW_CREATE_GATE_BLUE_X, DRAW_CREATE_GATE_BLUE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);
	// オレンジゲート
	if (m_gateMgr->IsCreate(GateKind::Orange))	handle = m_files.at(I_CREATE_GATE_ORANGE)->GetHandle();
	else										handle = m_files.at(I_NOT_CREATE_GATE_ORANGE)->GetHandle();
	DrawRotaGraphFast(DRAW_CREATE_GATE_ORANGE_X, DRAW_CREATE_GATE_ORANGE_Y, FILE_SIZE_CREATE_GATE, DRAW_CREATE_GATE_ANGLE, handle, true);

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

void SceneTutorial::DrawTutorialClear() const
{
	if (!m_isDrawClear) return;

	// 縮小
	if (m_stampFrame < STAMP_FIRST_FRAME)
	{
		const float rate = m_stampFrame / static_cast<float>(STAMP_FIRST_FRAME);
		float size = Easing::EaseIn(FILE_SIZE_PERFECT_LARGE, FILE_SIZE_PERFECT_SMALL, rate);
		int alpha = static_cast<int>(255 * rate);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRotaGraph(DRAW_PERFECT_X, DRAW_PERFECT_Y, size, 0.0f, m_files.at(I_TUTORIAL_PERFECT)->GetHandle(), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// 拡大
	else if (m_stampFrame < STAMP_SECOND_FRAME)
	{
		const float rate = (m_stampFrame - STAMP_FIRST_FRAME) / static_cast<float>(STAMP_SECOND_FRAME - STAMP_FIRST_FRAME);
		float size = Easing::EaseIn(FILE_SIZE_PERFECT_SMALL, FILE_SIZE_PERFECT_LARGE_2, rate);
		DrawRotaGraph(DRAW_PERFECT_X, DRAW_PERFECT_Y, size, 0.0f, m_files.at(I_TUTORIAL_PERFECT)->GetHandle(), true);
	}
	// 縮小
	else if (m_stampFrame < TOTAL_STAMP_FRAME)
	{
		const float rate = (m_stampFrame - STAMP_SECOND_FRAME) / static_cast<float>(TOTAL_STAMP_FRAME - STAMP_SECOND_FRAME);
		float size = Easing::EaseIn(FILE_SIZE_PERFECT_LARGE_2, FILE_SIZE_PERFECT, rate);
		DrawRotaGraph(DRAW_PERFECT_X, DRAW_PERFECT_Y, size, 0.0f, m_files.at(I_TUTORIAL_PERFECT)->GetHandle(), true);
	}
	// 通常描画
	else
	{
		DrawRotaGraph(DRAW_PERFECT_X, DRAW_PERFECT_Y, FILE_SIZE_PERFECT, 0.0f, m_files.at(I_TUTORIAL_PERFECT)->GetHandle(), true);
	}
}

void SceneTutorial::DrawTutorialGoal() const
{
	auto& novelMgr = NovelManager::GetInstance();
	// 描画しないまたは、ノベルデータを描画中は処理終了
	if (!m_isDrawGoal || !novelMgr.IsStop()) return;

	const bool isMove = m_uiFrame1 > 0;
	int frameX = DRAW_TUTORIAL_FRAME_X;
	int y = DRAW_TUTORIAL_FRAME_Y;
	float size = 1.0f;
	// 移動フレームの場合
	if (isMove)
	{
		// ステイフレームの間は大きさ、移動は最大にする
		if (m_uiFrame1 > UI_MOVE_FRAME)
		{
			frameX += MOVE_TUTORIAL_FRAME_X;
			y += MOVE_TUTORIAL_FRAME_Y;
			size += ADD_FILE_SIZE_TUTORIAL_FRAME;
		}
		// ステイフレームでないなら、割合で大きさ、移動を変更する
		else
		{
			const float rate = m_uiFrame1 / static_cast<float>(UI_MOVE_FRAME);

			frameX += static_cast<int>(MOVE_TUTORIAL_FRAME_X * rate);
			y += static_cast<int>(MOVE_TUTORIAL_FRAME_Y * rate);
			size += rate * ADD_FILE_SIZE_TUTORIAL_FRAME;
		}
	}
	DrawRotaGraphFast(frameX, y, size, 0.0f, m_files.at(I_TUTORIAL_FRAME)->GetHandle(), true);

	const auto& info = m_tutorialInfo.at(m_index);
	int textH = m_files.at(info.goalTextId)->GetHandle();
	// 画像サイズ取得
	int w, h;
	GetGraphSize(textH, &w, &h);
	// 移動完了していればテキストを動かす
	if (!isMove)
	{
		const float rate = (m_uiFrame1 * -1) % TEXT_MOVE_CIRCLE / static_cast<float>(TEXT_MOVE_CIRCLE);
		const int moveSize = static_cast<int>((w + TEXT_SPACE_W) * rate);
		// 1つ目
		int srcX = moveSize;
		DrawRectGraph(DRAW_TEXT_RECT_X, DRAW_TEXT_RECT_Y, srcX, 0, TEXT_DRAW_RANGE_W, h, textH, true);
		// 2つ目
		int x = DRAW_TEXT_RECT_X + w + TEXT_SPACE_W - moveSize;
		int srcW = std::max<int>(0, moveSize + TEXT_DRAW_RANGE_W - w - TEXT_SPACE_W);
		DrawRectGraph(x, DRAW_TEXT_RECT_Y, 0, 0, srcW, h, textH, true);
	}
	else
	{
		DrawRectRotaGraphFast(frameX, y, 0, 0, TEXT_DRAW_RANGE_W, h, size, 0.0f, textH, true);
	}
}

void SceneTutorial::DrawTutorialNovel() const
{
	// ノベルデータを停止していたら終了
	auto& novelMgr = NovelManager::GetInstance();
	if (novelMgr.IsStop()) return;
	// フレーム
	DrawRotaGraphFast3(DRAW_NOVEL_FRAME_X, DRAW_NOVEL_FRAME_Y, 0, 0, 1.0f, FILE_SIZE_NOVEL_FRAME_Y, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true, true);
	// 文字列
	novelMgr.Draw(NOVEL_SPACE_NUM, DRAW_NOVEL_Y, COLOR_NOVEL, FONT_SIZE_NOVEL);
	// 画像
	novelMgr.DrawGraph(Game::CENTER_X, DRAW_NOVEL_GRAPH_Y, 1.0f);

	float rate = std::cosf(m_uiFrame2 * SCALING_SPEED_NOVEL_BUTTON);
	float size = FILE_SIZE_NOVEL_BUTTON + rate * SCALING_NOVEEL_BUTTON;
	DrawRotaGraphFast(DRAW_NOVEL_BUTTON_X, DRAW_NOVEL_BUTTON_Y, size, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
}

void SceneTutorial::DrawTutorialDestination() const
{
	auto& info = m_tutorialInfo.at(m_index);
	if (!info.isDrawDestination) return;

	auto pos = info.destination;
	pos.y += std::sinf(m_uiFrame2 * SHAKE_SPEED_DESTINATION_ARROW) * SHAKE_SIZE_DESTINATION_ARROW;

	DrawBillboard3D(pos.VGet(), 0.5f, 0.5f, FILE_SIZE_DESTINATION_ARROW, ANGLE_DESTINATION_ARROW, m_files.at(I_RANKING_ARROW)->GetHandle(), true);
}
