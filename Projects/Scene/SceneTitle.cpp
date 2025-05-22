#include "SceneTitle.h"
#include <DxLib.h>
#include <vector>
#include <sstream>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "TimeUtility.h"
#include "Game.h"
#include "Files.h"
#include "Application.h"
#include "FontManager.h"
#include "SceneManager.h"
#include "SceneHelp.h"
#include "SceneStageSelect.h"
#include "SceneTutorial.h"
#include "SaveDataManager.h"
#include "StageDataManager.h"
#include "RankingDataManager.h"
#include "SoundManager.h"
#include "EffekseerManager.h"
#include "Camera/PlayerCamera.h"
#include "Camera/CameraManager.h"
#include "Object/Player.h"
#include "Object/Gate/GateManager.h"
#include "Object/Stage/StageManager.h"
#include "Object/Gate/GateBullet.h"
#include "Object/Gimmick/Turret/Turret.h"
#include "Collider/ColliderCapsule.h"

namespace
{
	/* 列挙型 */
	// メニュー選択
	enum class MenuCurrent : int
	{
		NEW_GAME,
		CONTINUE,
		RANKING,
		OPOTION,
		END,
		MAX
	};
	// データ確認
	enum class DecideCurrent : int
	{
		CONTINU,
		DELETE_DATA,
		MAX
	};

	/* ロゴ */
	constexpr int DRAW_LOGO_X = 160;		// 描画位置(x)
	constexpr int DRAW_LOGO_Y = 244;				// 描画位置(y)
	constexpr float FILE_SIZE_LOGO = 1.0f;			// 画像サイズ

	/* メニュー */
	// 描画位置
	constexpr int DRAW_MENU_STR_X = 80;				// 文字描画位置(x)
	constexpr int DRAW_MENU_FRAME_X = 180;			// フレーム描画位置(x)
	constexpr int DRAW_MENU_Y = 386;				// 描画位置(y)
	constexpr int DRAW_MENU_Y_INTERVAL = 64;		// 描画間隔(y)
	constexpr float FILE_SIZE_MENU_FRAME = 1.0f;	// フレーム描画サイズ
	constexpr int FONT_SIZE_MENU = 36;				// 文字フォントサイズ
	// 文字列
	const std::vector<std::wstring> STR_MENU =
	{
		L"ニューゲーム",
		L"ロード",
		L"ランキング",
		L"オプション",
		L"エンド",
	};

	/* セーブデータ選択 */
	// 文字列
	const std::vector<std::wstring> STR_SAVE =
	{
		L"セーブデータ1",
		L"セーブデータ2",
		L"セーブデータ3",
	};
	constexpr int DRAW_SAVE_X = 450;	// 基準描画位置(x)
	constexpr int DRAW_SAVE_Y = 530;	// 基準描画位置(y)
	constexpr int DRAW_SAVE_FRAME_X = 450;	// フレーム描画位置
	constexpr int DRAW_SAVE_FRAME_Y = 466;	// フレーム描画位置
	constexpr int DRAW_SAVE_Y_INTERVAL = 64;	// 描画間隔(y)
	constexpr int DRAW_SUB_SAVE_ARM_X = -142;
	constexpr int DRAW_SUB_SAVE_ARM_Y = -80;
	constexpr int DRAW_SUB_SAVE_ARM_FRAME_X = -100;
	constexpr int DRAW_SUB_SAVE_STR_X = -92;
	constexpr int DRAW_SUB_SAVE_STR_Y = -64;
	constexpr int FONT_SIZE_SAVE = 32;					// 文字フォントサイズ
	constexpr float FILE_SIZE_SAVE_ARM = 0.875f;
	constexpr float FILE_SIZE_SAVE_ARM_FRAME = 0.875f;
	constexpr float FILE_SIZE_SAVE_WINDOW = 0.875f;
	constexpr float FILE_SIZE_SAVE_FRAME = 0.875f;
	constexpr int FADE_FRAME_SAVE_INFO = 10;

	/* セーブデータ情報 */
	constexpr int DRAW_SAVE_INFO_X = 718;	// 基準描画位置
	constexpr int DRAW_SAVE_INFO_Y = 498;	// 基準描画位置 
	constexpr int DRAW_SAVE_INFO_Y_INTERVAL = DRAW_SAVE_Y_INTERVAL;	// 描画間隔
	constexpr int DRAW_SAVE_INFO_TIME_Y = -32;		// 時間情報描画間隔
	constexpr int DRAW_SAVE_INFO_CLEAR_NUM_Y = 24;	// クリア数情報描画間隔
	constexpr int DRAW_SUB_SAVE_INFO_ARM_X = -152;
	constexpr int DRAW_SUB_SAVE_INFO_ARM_Y = -32;
	constexpr int DRAW_SUB_SAVE_INFO_ARM_FRAME_X = -102;
	constexpr int DRAW_SUB_SAVE_INFO_STR_X = -110;
	constexpr float FILE_SIZE_SAVE_INFO_ARM = 1.0f;
	constexpr float FILE_SIZE_SAVE_INFO_ARM_FRAME = 1.0f;
	constexpr float FILE_SIZE_SAVE_INFO_WINDOW = 1.0f;
	constexpr int FONT_SIZE_SAVE_INFO = 28;		// 文字フォントサイズ

	/* ランキング */
	constexpr int DRAW_SUB_RANKING_ARROW_X = 256;		// 矢印差分描画位置(x)
	constexpr int DRAW_SUB_RANKING_ARROW_Y = 240;		// 矢印差分描画位置(y)
	constexpr int DRAW_SUB_RANKING_NAME_Y = 240;		// ステージ名差分描画位置(y)
	constexpr int DRAW_SUB_RANKING_RANKING_X = -240;	// 順位差分描画位置(x)
	constexpr int DRAW_SUB_RANKING_RANK_X = -112;		// ランク差分描画位置(x)
	constexpr int DRAW_SUB_RANKING_TIME_X = 160;		// タイム差分描画位置(x)
	constexpr int DRAW_SUB_RANKING_INFO_Y = -134;		// ランキング情報差分描画位置(y)
	constexpr int DRAW_SUB_RANKING_INFO_Y_INTERVAL = 96;	// ランキング情報描画間隔(y)
	constexpr unsigned int COLOR_RANKING_NAME = 0xffffff;	// ステージ名文字カラー
	constexpr unsigned int COLOR_RANKING_DATA = 0xffffff;	// 情報文字カラー
	constexpr int DRAW_RANKING_OUT_VAL = -720;	// 画面外までの大きさ
	constexpr int FONT_SIZE_RANKING_NAME = 48;	// ステージ名フォントサイズ
	constexpr int FONT_SIZE_RANKING_INFO = 48;	// 情報フォントサイズ
	constexpr float FILE_SIZE_RANKING_INFO = 0.5f;		// 順位画像サイズ
	constexpr float FILE_SIZE_RANKING_ARROW = 1.0f;		// 矢印画像サイズ
	constexpr float FILE_SIZE_RANKING_RANK = 0.125f;	// ランク画像サイズ
	constexpr int WAVE_SIZE_RANKING_NAME = 8;		// ステージ名ウェーブサイズ
	constexpr float WAVE_SPEED_RANKING_NAME = 2.0f;	// ステージ名ウェーブスピード
	constexpr int DRAW_RANKING_ARROW_X_SWIPE_SIZE = 16;	// 矢印スワイプサイズ
	constexpr float SWIPE_SPEED_ARROW = 1.3f;			// 矢印スワイプスピード
	// ランク毎のファイルパス
	const std::unordered_map<RankKind, int> RANK_FILE_PATH =
	{
		{ RankKind::S, I_RANK_S },
		{ RankKind::A, I_RANK_A },
		{ RankKind::B, I_RANK_B },
		{ RankKind::C, I_RANK_C },
	};

	/* PadUi描画 */
	constexpr int DRAW_PAD_X = 1130;		// 基準Y描画位置
	constexpr int DRAW_PAD_Y = 700;			// 画像描画位置
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int DRAW_PAD_STR_X = 1150;	// 文字列描画位置
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int PAD_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー
	constexpr int DRAW_PAD_Y_INTERVAL = 36;		// 描画間隔

	/* 続けるエラー系 */
	constexpr int DRAW_ERROR_Y = 450;
	constexpr int DRAW_ERROR_WINDOW_X = 468;
	constexpr int DRAW_ERROR_ARM_X = 316;
	constexpr int DRAW_ERROR_ARM_FRAME_X = 366;
	constexpr int DRAW_ERROR_STR_X = 358;

	/* その他 */
	constexpr float SPEED_FRAME_FADE = 0.02f;	// フレームの点滅速度
	constexpr int ALPHA_SIZE_FRAME = 48;		// フレームの点滅サイズ
	// 選択文字カラー
	constexpr unsigned int COLOR_SELECT = 0xffffff;
	// 未選択文字カラー
	constexpr unsigned int COLOR_NOT_SELECT = 0x888888;
	// エラー文字カラー
	constexpr unsigned int COLOR_ERROR = 0xff0000;
	// 文字ウェーブの大きさ
	constexpr int STR_WAVE_SIZE = 4;
	// エラー揺れるサイズ
	constexpr int SHAKE_SIZE_ERROR = 4;
	// ウェーブスピード
	constexpr float STR_WAVE_SPEED = 2.0f;
	// フェードにかかるフレーム
	constexpr int FADE_FRAME_SAVE_NO = 20;
	// UIの移動速度
	constexpr float MOVE_SPEED = 1 / 5.0f;
	// 時間のフェード速度
	constexpr int TIME_FADE_SPEED = 2;
	// リピート間隔
	constexpr int REPEATE_INTERVAL = 17;

	// カメラの向き
	const std::vector<Vec3> CAMERA_DIRS =
	{
		Vec3(-1.0f, -0.8f, 1.0f),
		Vec3(1.0f, -0.8f, -1.0f),
		Vec3(0.0f, -0.8f, -1.0f),
	};
	// カメラの位置
	const std::vector<Vec3> CAMERA_POSS =
	{
		Vec3(-2.0f, 26.0f, 324.0f),
		Vec3(156.0f, 16.0f, 42.0f),
		Vec3(294.3f, 16.0f,  36.0f),
	};
	// カメラの回転度度合い
	constexpr float CAMERA_ROT_ANGLE = 8.0f;
	// カメラの回転速度
	constexpr float CAMERA_ROT_SPEED = 0.0025f;

	// ステージ数
	constexpr int STAGE_NUM = 3;
	// ステージ番号
	std::vector<int> STAGE_NO = 
	{ 
		0, 
		5, 
		9
	};
	// ステージ変更までのフレーム
	constexpr int STAGE_CHANGE_FRAME = 5000;
	// ステージ変更フェードスピード
	constexpr float STAGE_FADE_SPEED = 1 / 16.0f;
	// ステージ変更時のカラーDissolveの色
	constexpr float COLOR_FIRE_R = 0.09f;	// 赤
	constexpr float COLOR_FIRE_G = 0.59f;	// 緑
	constexpr float COLOR_FIRE_B = 0.59f;	// 青
}

SceneTitle::SceneTitle() :
	SceneBase(SceneKind::TITLE),
	m_updateFunc(&SceneTitle::SelectMenuUpdate),
	m_drawFunc(&SceneTitle::DrawSelectMenu),
	m_state(State::MENU),
	m_cBuff(nullptr),
	m_cBuffH(-1),
	m_cameraRt1(-1),
	m_cameraRt2(-1),
	m_menuCurrent(0),
	m_saveDataNo(0),
	m_rankingCurrent(0),
	m_fadeSaveInfoFrame(0),
	m_fadeSaveNoFrame(0),
	m_uiCount(0),
	m_cameraFrame(0),
	m_stageChangeFrame(0),
	m_stageIndex(0),
	m_erroeCount(0),
	m_stageFade(0.0f),
	m_isFadeSaveNo(false),
	m_isNowSelectMenu(true),
	m_isNowSelectSaveData(false),
	m_isNewData(false),
	m_isError(false),
	m_isErrorShake(false),
	m_isStageChangeFade(false)
{
}

void SceneTitle::AsyncInit()
{
	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[M_TURRET]				= fileMgr.Load(M_TURRET);
	m_files[I_LOGO]					= fileMgr.Load(I_LOGO);
	m_files[I_FRAME]				= fileMgr.Load(I_FRAME);
	m_files[I_COMMON_FRAME]			= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_COMMON_SELECT_FRAME]	= fileMgr.Load(I_COMMON_SELECT_FRAME);
	m_files[I_SAVE_ARM]				= fileMgr.Load(I_SAVE_ARM);
	m_files[I_SAVE_ARM_FRAME]		= fileMgr.Load(I_SAVE_ARM_FRAME);
	m_files[I_SAVE_WINDOW]			= fileMgr.Load(I_SAVE_WINDOW);
	m_files[I_DECIDE_ARM]			= fileMgr.Load(I_DECIDE_ARM);
	m_files[I_DECIDE_ARM_FRAME]		= fileMgr.Load(I_DECIDE_ARM_FRAME);
	m_files[I_DECIDE_WINDOW]		= fileMgr.Load(I_DECIDE_WINDOW);
	m_files[I_OPTION_WINDOW]		= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_RANKING_FRAME]		= fileMgr.Load(I_RANKING_FRAME);
	m_files[I_RANKING_ARROW]		= fileMgr.Load(I_RANKING_ARROW);
	m_files[I_RANK_S]				= fileMgr.Load(I_RANK_S);
	m_files[I_RANK_A]				= fileMgr.Load(I_RANK_A);
	m_files[I_RANK_B]				= fileMgr.Load(I_RANK_B);
	m_files[I_RANK_C]				= fileMgr.Load(I_RANK_C);
	m_files[I_PAD_A]				= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]				= fileMgr.Load(I_PAD_B);
	m_files[I_DISSOLVE]				= fileMgr.Load(I_DISSOLVE);
	m_files[S_CANCEL]				= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]				= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]			= fileMgr.Load(S_CURSOR_MOVE);
	m_files[S_SHOT_PLAYER]			= fileMgr.Load(S_SHOT_PLAYER);
	m_files[B_TITLE]				= fileMgr.Load(B_TITLE);
	m_files[PS_DISSOLVE]			= fileMgr.Load(PS_DISSOLVE);
	for (int i = 0; i < RankingDataManager::RANKING_DATA_NUM; ++i) m_files[I_RANKING_FIRST + i] = fileMgr.Load(I_RANKING_FIRST + i);
	auto& effMgr = EffekseerManager::GetInstance();
	effMgr.LoadFile(E_LASER_BULLET);
	effMgr.LoadFile(E_DEATH_TURRET);
	effMgr.LoadFile(E_BULLET);
	effMgr.LoadFile(E_MUZZLE_FLASH);

	SetUseASyncLoadFlag(false);
	// 画面生成
	m_cameraRt1 = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
	m_cameraRt2 = MakeScreen(Game::WINDOW_W, Game::WINDOW_H, true);
	// VRAMにメモリを確保
	m_cBuffH = CreateShaderConstantBuffer(sizeof(CBuff));
	m_cBuff = static_cast<CBuff*>(GetBufferShaderConstantBuffer(m_cBuffH));
	SetUseASyncLoadFlag(true);

	// マネージャー生成
	for (int i = 0; i < STAGE_NUM; ++i)
	{
		m_stageMgrs.push_back(std::make_shared<StageManager>(STAGE_NO.at(i)));
	}
	m_cameraMgr = std::make_shared<CameraManager>();
	// 初期化(非同期)
	for (auto& mgr : m_stageMgrs) mgr->AsyncInit();
}

void SceneTitle::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_TITLE)->GetHandle());
	// UIデータ生成
	m_mainUI			= UIMoveData::Make(DRAW_MENU_FRAME_X, DRAW_MENU_Y);
	m_saveUI			= UIMoveData::Make(DRAW_SAVE_FRAME_X, DRAW_SAVE_FRAME_Y);
	m_saveInfoUI		= UIMoveData::Make(DRAW_SAVE_INFO_X, DRAW_SAVE_INFO_Y);
	m_rankingUI			= UIMoveData::Make(Game::CENTER_X, Game::CENTER_Y + DRAW_RANKING_OUT_VAL);
	for (auto& mgr : m_stageMgrs) mgr->Init(nullptr, nullptr);
	m_stageMgrs.at(m_stageIndex)->AppLights();
	// 1つ目のカメラを設定
	m_nowCamera = m_cameraMgr->GetCamera(CameraKind::TITLE_1);
	m_nowCamera->SetFront(CAMERA_DIRS.at(0), 0.0f);
	m_nowCamera->SetTargetPos(CAMERA_POSS.at(0));
	m_nowCamera->SetTps(false);
	m_nowCamera->SetUpFixed(true);
	// 2つ目のカメラを設定
	m_nextCamera = m_cameraMgr->GetCamera(CameraKind::TITLE_2);
	m_nextCamera->SetFront(CAMERA_DIRS.at(1), 0.0f);
	m_nextCamera->SetTargetPos(CAMERA_POSS.at(1));
	m_nextCamera->SetTps(false);
	m_nextCamera->SetUpFixed(true);
	// シェーダの設定
	m_cBuff->fireRed = COLOR_FIRE_R;
	m_cBuff->fireGreen = COLOR_FIRE_G;
	m_cBuff->fireBlue = COLOR_FIRE_B;
}

void SceneTitle::End()
{
	for (auto& mgr : m_stageMgrs) mgr->End();
	m_cameraMgr->End();

	EffekseerManager::GetInstance().ReleaseFile();
}

void SceneTitle::Update(bool isFade)
{
	// ウェーブ更新
	++m_uiCount;
	// エラー処理
	if (m_isErrorShake)
	{
		// エラー描画フレーム
		constexpr int ERROR_COUNT = 30;
		++m_erroeCount;
		if (m_erroeCount > ERROR_COUNT)
		{
			m_isErrorShake = false;
		}
	}
	// UI更新
	m_mainUI->Update(MOVE_SPEED, EasingType::LERP);
	m_saveUI->Update(MOVE_SPEED, EasingType::LERP);
	m_saveInfoUI->Update(MOVE_SPEED, EasingType::LERP);
	m_rankingUI->Update(MOVE_SPEED, EasingType::LERP);
	// ステージ更新
	StageUpdate();

	if (isFade) return;
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::TITLE);
		return;
	}
	(this->*m_updateFunc)();
}

void SceneTitle::Draw() const
{
	auto& fontMgr = FontManager::GetInstance();

	/* 共通で描画する部分 */
	if (m_isStageChangeFade)
	{
		// フェード中は2つのカメラで描画
		m_stageMgrs.at(m_stageIndex)->AppLights();
		SceneHelp::GetInstance().DrawModel(nullptr, m_cameraMgr, m_stageMgrs.at(m_stageIndex), nullptr, m_cameraRt1, CameraKind::TITLE_1);
		int nextIndex = (m_stageIndex + 1) % STAGE_NUM;
		m_stageMgrs.at(nextIndex)->AppLights();
		SceneHelp::GetInstance().DrawModel(nullptr, m_cameraMgr, m_stageMgrs.at(nextIndex), nullptr, m_cameraRt2, CameraKind::TITLE_2);
		// 合成
		SetDrawScreen(DX_SCREEN_BACK);
		int dissolveH = m_files.at(I_DISSOLVE)->GetHandle();
		int dissolvePS = m_files.at(PS_DISSOLVE)->GetHandle();
		UpdateShaderConstantBuffer(m_cBuffH);
		SetShaderConstantBuffer(m_cBuffH, DX_SHADERTYPE_PIXEL, 4);
		MyEngine::DrawGraph(0, 0, dissolvePS, m_cameraRt1, m_cameraRt2, dissolveH);
	}
	else
	{
		SceneHelp::GetInstance().DrawModel(nullptr, m_cameraMgr, m_stageMgrs.at(m_stageIndex), nullptr, DX_SCREEN_BACK, CameraKind::TITLE_1);
	}

	// 画像ハンドル
	int frameH = m_files.at(I_FRAME)->GetHandle();
	// フレームの点滅具合を計算
	const float rate = std::cosf(m_uiCount * SPEED_FRAME_FADE);
	const float halfSize = ALPHA_SIZE_FRAME * 0.5f;
	const int alpha = static_cast<int>(Game::ALPHA_VAL - halfSize + halfSize * rate);

	/* ロゴ */
	DrawRotaGraphFast(DRAW_LOGO_X, DRAW_LOGO_Y, FILE_SIZE_LOGO, 0.0f, m_files.at(I_LOGO)->GetHandle(), true);

	/* メニュー */
	// フレーム
	if (m_isNowSelectMenu) SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawRotaGraphFast(m_mainUI->x, m_mainUI->y, FILE_SIZE_MENU_FRAME, 0.0f, frameH, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// 文字列
	for (int i = 0; i < static_cast<int>(MenuCurrent::MAX); ++i)
	{
		int y = DRAW_MENU_Y + DRAW_MENU_Y_INTERVAL * i;
		// 選択中の場合、ウェーブ描画
		if (m_menuCurrent == i && m_isNowSelectMenu)  UIUtility::DrawWaveStrLeft(DRAW_MENU_STR_X, y, COLOR_SELECT, STR_MENU.at(i), FONT_SIZE_MENU, m_uiCount * STR_WAVE_SPEED, STR_WAVE_SIZE, true);
		// 未選択の場合、通常描画
		else									UIUtility::DrawShadowStrLeft(DRAW_MENU_STR_X, y, 2, 2, COLOR_NOT_SELECT, STR_MENU.at(i), FONT_SIZE_MENU);
	}

	/* セーブデータ選択 */
	if (m_fadeSaveInfoFrame > 0)
	{
		const bool isFadeEnd = m_fadeSaveInfoFrame > FADE_FRAME_SAVE_INFO;
		if (!isFadeEnd)
		{
			const float rate = m_fadeSaveInfoFrame / static_cast<float>(FADE_FRAME_SAVE_INFO);
			const int alpha = static_cast<int>(Game::ALPHA_VAL * rate);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		}
		// アーム＆ウィンドウ
		int subY = 0;
		if (m_isNewData) subY = DRAW_SAVE_Y_INTERVAL;
		DrawRotaGraphFast(DRAW_SAVE_X + DRAW_SUB_SAVE_ARM_X,		DRAW_SAVE_Y + DRAW_SUB_SAVE_ARM_Y - subY, FILE_SIZE_SAVE_ARM, 0.0f, m_files.at(I_SAVE_ARM)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_SAVE_X + DRAW_SUB_SAVE_ARM_FRAME_X,  DRAW_SAVE_Y - subY, FILE_SIZE_SAVE_ARM_FRAME, 0.0f, m_files.at(I_SAVE_ARM_FRAME)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_SAVE_X,								DRAW_SAVE_Y - subY, FILE_SIZE_SAVE_WINDOW, 0.0f, m_files.at(I_SAVE_WINDOW)->GetHandle(), true);
		// フレーム
		if (m_isNowSelectSaveData && isFadeEnd) SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRotaGraphFast(m_saveUI->x, m_saveUI->y - subY, FILE_SIZE_SAVE_FRAME, 0.0f, frameH, true);
		if (isFadeEnd) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		// 文字列
		for (int i = 0; i < SaveDataManager::GetInstance().GetSaveDataNum(); ++i)
		{
			int y = DRAW_SAVE_Y + DRAW_SUB_SAVE_STR_Y + DRAW_SAVE_Y_INTERVAL * i - subY;
			// 選択中の場合、ウェーブ描画
			if (m_saveDataNo == i && m_isNowSelectSaveData)	UIUtility::DrawWaveStrLeft(DRAW_SAVE_X + DRAW_SUB_SAVE_STR_X, y, COLOR_SELECT, STR_SAVE.at(i), FONT_SIZE_SAVE, m_uiCount * STR_WAVE_SPEED, STR_WAVE_SIZE, true);
			// 未選択の場合、通常描画
			else											UIUtility::DrawShadowStrLeft(DRAW_SAVE_X + DRAW_SUB_SAVE_STR_X, y, 2, 2, COLOR_NOT_SELECT, STR_SAVE.at(i), FONT_SIZE_SAVE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// それぞれでの描画処理
	(this->*m_drawFunc)();
	// PadUI
	DrawPadUI();
}

void SceneTitle::StageUpdate()
{
	// カメラの回転
	++m_cameraFrame;
	float rate = sinf(m_cameraFrame * CAMERA_ROT_SPEED);
	float angle = CAMERA_ROT_ANGLE * rate;
	const auto& rot = Quaternion::AngleAxis(angle, Vec3::Up());
	m_nowCamera->SetFront(rot * CAMERA_DIRS.at(m_stageIndex), 0.0f);
	m_nowCamera->Update();
	m_nextCamera->SetFront(rot * CAMERA_DIRS.at((m_stageIndex + 1) % STAGE_NUM), 0.0f);
	m_nextCamera->Update();

	// ステージフェードをしてる場合
	if (m_isStageChangeFade)
	{
		// フェード更新
		m_stageFade += STAGE_FADE_SPEED;
		// フェード終了したら
		if (m_stageFade > 1.0f)
		{
			m_isStageChangeFade = false;
			m_stageFade = 1.0f;
			// ステージ変更
			m_stageChangeFrame = 0;
			m_stageIndex = (m_stageIndex + 1) % STAGE_NUM;
			// カメラの位置を変更
			m_nowCamera->SetTargetPos(CAMERA_POSS.at(m_stageIndex));
			m_nextCamera->SetTargetPos(CAMERA_POSS.at((m_stageIndex + 1) % STAGE_NUM));
			// カメラの向きを変更
			m_nowCamera->SetFront(rot * CAMERA_DIRS.at(m_stageIndex), 0.0f);
			m_nextCamera->SetFront(rot * CAMERA_DIRS.at((m_stageIndex + 1) % STAGE_NUM), 0.0f);
			// ライトの適用
			m_stageMgrs.at(m_stageIndex)->AppLights();
		}
		m_cBuff->frame = m_stageFade;
	}
	// していない場合
	else
	{
		// 待機
		++m_stageChangeFrame;
		// 一定フレーム経過したらフェード開始
		if (m_stageChangeFrame > STAGE_CHANGE_FRAME)
		{
			m_stageFade = 0.0f;
			m_isStageChangeFade = true;
			m_cBuff->frame = 0.0f;
		}
	}
	m_stageMgrs.at(m_stageIndex)->Update();
}

void SceneTitle::SelectMenuUpdate()
{
	bool isMove = false;
	if (CursorUtility::CursorUp<MenuCurrent>(m_menuCurrent, MenuCurrent::MAX, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<MenuCurrent>(m_menuCurrent, MenuCurrent::MAX, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_mainUI->ChangeVertical(DRAW_MENU_Y + DRAW_MENU_Y_INTERVAL * m_menuCurrent);
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		m_uiCount = 0;
		// ステージの変更
		m_stageFade = 0.0f;
		m_isStageChangeFade = true;
		m_cBuff->frame = 0.0f;
		// エラーしていないことに
		m_isError = false;
	}
	--m_fadeSaveInfoFrame;

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		bool isPlaySe = true;
		// ニューゲーム
		if (m_menuCurrent == static_cast<int>(MenuCurrent::NEW_GAME))
		{
			// 1つもセーブデータが存在しない場合
			auto& saveDataMgr = SaveDataManager::GetInstance();
			if (!saveDataMgr.IsExist(0))
			{
				// ゲームスタート
				m_saveDataNo = 0;
				OnStart();
			}
			else
			{
				// セーブデータ選択に移動
				OnSelectSaveData();
				m_isNewData = true;
				// Window関係のものを不透明に
				m_fadeSaveInfoFrame = std::max<int>(m_fadeSaveInfoFrame, 0);
			}
		}
		// 続きから
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::CONTINUE))
		{
			auto& saveDataMgr = SaveDataManager::GetInstance();
			// 1つもセーブデータが存在する場合に限り変更
			if (saveDataMgr.IsExist(0))
			{
				// セーブデータ選択に移動
				OnSelectSaveData();
				m_isNewData = false;
				// Window関係のものを不透明に
				m_fadeSaveInfoFrame = std::max<int>(m_fadeSaveInfoFrame, 0);
			}
			// ない場合はエラーに
			else
			{
				m_erroeCount = 0;
				m_isError = true;
				m_isErrorShake = true;
				SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
			}
		}
		// ランキングを開く
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::RANKING))
		{
			OnRanking();
		}
		// オプションを開く
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::OPOTION))
		{
			m_scnMgr.OpenOption(SceneKind::TITLE);
		}
		// ゲーム終了
		else if (m_menuCurrent == static_cast<int>(MenuCurrent::END))
		{
			auto& app = Application::GetInstance();
			app.OnEnd();
		}

		if (isPlaySe) SoundManager::GetInstance().PlaySe(m_files.at(S_DECIDE)->GetHandle());
	}
}

void SceneTitle::RankingUpdate()
{
	bool isMove = false;
	auto stageNum = StageDataManager::GetInstance().GetStageNum() - 2;
	if (CursorUtility::CursorUp(m_rankingCurrent, stageNum, Command::BTN_LEFT, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown(m_rankingCurrent, stageNum, Command::BTN_RIGHT, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
	}

	auto& input = Input::GetInstance();
	// キャンセル
	if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		OnSelectMenu();
		m_rankingUI->ChangeVertical(Game::CENTER_Y + DRAW_RANKING_OUT_VAL);
	}
}

void SceneTitle::SelectSaveDataUpdate()
{
	// カーソルが移動したら
	const auto saveDataNum = SaveDataManager::GetInstance().GetSaveDataNum();
	bool isMove = false;
	if (CursorUtility::CursorUp(m_saveDataNo, saveDataNum, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown(m_saveDataNo, saveDataNum, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());
		// カウント初期化
		m_uiCount = 0;
		m_fadeSaveNoFrame = 0;
		m_isFadeSaveNo = true;
		// エラーしていないことに
		m_isError = false;
		// 位置の変更
		m_saveUI->ChangeVertical(DRAW_SAVE_FRAME_Y + DRAW_SAVE_Y_INTERVAL * m_saveDataNo, -1, true);
		m_saveInfoUI->ChangeVertical(DRAW_SAVE_INFO_Y + DRAW_SAVE_INFO_Y_INTERVAL * m_saveDataNo, -1, true);
	}
	// フェード更新
	++m_fadeSaveInfoFrame;
	if (m_isFadeSaveNo)
	{
		++m_fadeSaveNoFrame;
		m_isFadeSaveNo = m_fadeSaveNoFrame < FADE_FRAME_SAVE_NO;
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		auto& sndMgr = SoundManager::GetInstance();
		auto& saveDataMgr = SaveDataManager::GetInstance();
		// ニューゲームの場合
		if (m_isNewData)
		{
			// 新しくデータを作成
			saveDataMgr.Initialized(m_saveDataNo);
			// ゲームスタート
			OnStart();
			sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle());
		}
		// ロードの場合
		else
		{
			// セーブデータが存在する場合ハスタート
			if (saveDataMgr.IsExist(m_saveDataNo))
			{
				// ゲームスタート
				OnStart();
				sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle());
			}
			// 存在しない場合はエラー
			else
			{
				m_erroeCount = 0;
				m_isError = true;
				m_isErrorShake = true;
				sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle());
			}
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		SoundManager::GetInstance().PlaySe(m_files.at(S_CANCEL)->GetHandle());
		m_isError = false;
		// メニュー選択に戻る
		OnSelectMenu();
		// Window関係のものを透明に
		m_fadeSaveInfoFrame = std::min<int>(m_fadeSaveInfoFrame, FADE_FRAME_SAVE_INFO);
	}
}

void SceneTitle::OnSelectMenu()
{
	m_state = State::MENU;
	// メンバ関数変更
	m_updateFunc = &SceneTitle::SelectMenuUpdate;
	m_drawFunc = &SceneTitle::DrawSelectMenu;
	// カウント初期化
	m_uiCount = 0;
	m_isNowSelectMenu = true;
	m_isNowSelectSaveData = false;
}

void SceneTitle::OnRanking()
{
	m_state = State::RANKING;
	m_rankingUI->ChangeVertical(Game::CENTER_Y);
	// メンバ関数変更
	m_updateFunc = &SceneTitle::RankingUpdate;
	m_drawFunc = &SceneTitle::DrawRanking;
}

void SceneTitle::OnSelectSaveData()
{
	m_state = State::SELECT_SAVE_DATA;
	// メンバ関数変更
	m_updateFunc = &SceneTitle::SelectSaveDataUpdate;
	m_drawFunc = &SceneTitle::DrawSelectSaveData;
	// カウント初期化
	m_uiCount = 0;
	m_isNowSelectMenu = false;
	m_isNowSelectSaveData = true;
}

void SceneTitle::DrawSelectMenu() const
{
	if (m_fadeSaveInfoFrame < 0)
	{
		const float rate = m_fadeSaveInfoFrame / static_cast<float>(FADE_FRAME_SAVE_INFO);
		const int alpha = static_cast<int>(Game::ALPHA_VAL * rate);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// アーム＆ウィンドウ
		int subY = 0;
		if (m_isNewData) subY = DRAW_SUB_SAVE_ARM_Y;
		DrawRotaGraphFast(DRAW_SAVE_INFO_X + DRAW_SUB_SAVE_INFO_ARM_X,		 m_saveInfoUI->y + DRAW_SUB_SAVE_INFO_ARM_Y - subY, FILE_SIZE_SAVE_INFO_ARM,		0.0f, m_files.at(I_DECIDE_ARM)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_SAVE_INFO_X + DRAW_SUB_SAVE_INFO_ARM_FRAME_X, m_saveInfoUI->y - subY,							FILE_SIZE_SAVE_INFO_ARM_FRAME,  0.0f, m_files.at(I_DECIDE_ARM_FRAME)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_SAVE_INFO_X,									 m_saveInfoUI->y - subY,							FILE_SIZE_SAVE_INFO_WINDOW,		0.0f, m_files.at(I_DECIDE_WINDOW)->GetHandle(), true);
		// セーブデータ情報描画
		DrawSaveInfo(m_saveDataNo);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// エラー描画
	if (m_isError)
	{
		DrawRotaGraphFast(DRAW_ERROR_WINDOW_X, DRAW_ERROR_Y, FILE_SIZE_SAVE_INFO_WINDOW, 0.0f, m_files.at(I_DECIDE_WINDOW)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_ERROR_ARM_FRAME_X, DRAW_ERROR_Y, FILE_SIZE_SAVE_INFO_ARM_FRAME, 0.0f, m_files.at(I_DECIDE_ARM_FRAME)->GetHandle(), true);
		DrawRotaGraphFast(DRAW_ERROR_ARM_X, DRAW_ERROR_Y, FILE_SIZE_SAVE_INFO_ARM, 0.0f, m_files.at(I_DECIDE_ARM)->GetHandle(), true);

		if (m_isErrorShake) UIUtility::DrawShakeStrLeft(DRAW_ERROR_STR_X, DRAW_ERROR_Y, COLOR_ERROR, L"データが存在しません", FONT_SIZE_SAVE_INFO, SHAKE_SIZE_ERROR, true);
		else				UIUtility::DrawShadowStrLeft(DRAW_ERROR_STR_X, DRAW_ERROR_Y, 2, 2, COLOR_ERROR, L"データが存在しません", FONT_SIZE_SAVE_INFO);
	}
}

void SceneTitle::DrawRanking() const
{
	auto& fontMgr = FontManager::GetInstance();
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageName = stageDataMgr.GetStageName(m_rankingCurrent + 1);
	auto& rnkMgr = RankingDataManager::GetInstance();
	// ウィンドウ描画
	DrawRotaGraphFast(m_rankingUI->x, m_rankingUI->y, 1.0f, 0.0f, m_files.at(I_RANKING_FRAME)->GetHandle(), true);
	// 矢印描画
	int addArrowX = static_cast<int>(std::sin(m_uiCount * Game::DEG_2_RAD * SWIPE_SPEED_ARROW) * DRAW_RANKING_ARROW_X_SWIPE_SIZE);
	DrawRotaGraphFast(m_rankingUI->x - DRAW_SUB_RANKING_ARROW_X + addArrowX, m_rankingUI->y - DRAW_SUB_RANKING_ARROW_Y, FILE_SIZE_RANKING_ARROW, 0.0f, m_files.at(I_RANKING_ARROW)->GetHandle(), true);
	DrawRotaGraphFast(m_rankingUI->x + DRAW_SUB_RANKING_ARROW_X - addArrowX, m_rankingUI->y - DRAW_SUB_RANKING_ARROW_Y, FILE_SIZE_RANKING_ARROW, 0.0f, m_files.at(I_RANKING_ARROW)->GetHandle(), true, true);
	UIUtility::DrawWaveStr(m_rankingUI->x, m_rankingUI->y - DRAW_SUB_RANKING_NAME_Y, COLOR_RANKING_NAME, stageName, FONT_SIZE_RANKING_NAME, m_uiCount * WAVE_SPEED_RANKING_NAME, WAVE_SIZE_RANKING_NAME, true);

	// 順位、タイム描画
	int startY = m_rankingUI->y + DRAW_SUB_RANKING_INFO_Y;
	for (int i = 0; i < RankingDataManager::RANKING_DATA_NUM; ++i)
	{
		int y = startY + DRAW_SUB_RANKING_INFO_Y_INTERVAL * i;
		// N位描画
		DrawRotaGraphFast(m_rankingUI->x + DRAW_SUB_RANKING_RANKING_X, y, 1.0f, 0.0f, m_files.at(I_RANKING_FIRST + i)->GetHandle(), true);
		auto timeFrame = rnkMgr.GetTime(m_rankingCurrent, i);
		// 記録がない場合
		if (timeFrame < 0)
		{
			UIUtility::DrawShadowStrLeft(m_rankingUI->x, y, 2, 2, COLOR_RANKING_DATA, L"記録がありません", FONT_SIZE_RANKING_INFO);
		}
		// 記録がある場合
		else
		{
			const auto& rank = stageDataMgr.GetRank(m_rankingCurrent + 1, timeFrame);
			const auto& path = RANK_FILE_PATH.at(rank);
			DrawRotaGraphFast(m_rankingUI->x + DRAW_SUB_RANKING_RANK_X, y, FILE_SIZE_RANKING_RANK, 0.0f, m_files.at(path)->GetHandle(), true);
			TimeUtility::DrawTime(m_rankingUI->x + DRAW_SUB_RANKING_TIME_X, y, timeFrame, FILE_SIZE_RANKING_INFO, COLOR_RANKING_DATA, true);
		}
	}
}

void SceneTitle::DrawSelectSaveData() const
{
	// フェード中ならα描画
	if (m_fadeSaveInfoFrame < FADE_FRAME_SAVE_INFO)
	{
		const float rate = m_fadeSaveInfoFrame / static_cast<float>(FADE_FRAME_SAVE_INFO);
		const int alpha = static_cast<int>(Game::ALPHA_VAL * rate);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	}
	// フェード中でない場合、もう一つがフェード中ならα描画
	else if (m_isFadeSaveNo)
	{
		const float rate = m_fadeSaveNoFrame / static_cast<float>(FADE_FRAME_SAVE_NO);
		const int alpha = static_cast<int>(Game::ALPHA_VAL * rate);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	}
	// アーム＆ウィンドウ
	int subY = 0;
	if (m_isNewData) subY = DRAW_SAVE_Y_INTERVAL;
	DrawRotaGraphFast(DRAW_SAVE_INFO_X + DRAW_SUB_SAVE_INFO_ARM_X,		 m_saveInfoUI->y + DRAW_SUB_SAVE_INFO_ARM_Y - subY, FILE_SIZE_SAVE_INFO_ARM,		0.0f, m_files.at(I_DECIDE_ARM)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_SAVE_INFO_X + DRAW_SUB_SAVE_INFO_ARM_FRAME_X, m_saveInfoUI->y - subY,							FILE_SIZE_SAVE_INFO_ARM_FRAME, 0.0f, m_files.at(I_DECIDE_ARM_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_SAVE_INFO_X,									 m_saveInfoUI->y - subY,							FILE_SIZE_SAVE_INFO_WINDOW,	0.0f, m_files.at(I_DECIDE_WINDOW)->GetHandle(), true);
	// セーブデータ情報描画
	DrawSaveInfo(m_saveDataNo);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::DrawSaveInfo(int saveNo) const
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	const auto fontH = FontManager::GetInstance().GetHandle(FONT_KAISOTAI, FONT_SIZE_SAVE_INFO);

	int subY = 0;
	if (m_isNewData) subY = DRAW_SAVE_Y_INTERVAL;
	// セーブデータが存在する場合
	if (saveDataMgr.IsExist(saveNo))
	{
		// 情報の書き込み
		auto& stageDataMgr = StageDataManager::GetInstance();
		const auto time = saveDataMgr.GetTotalTime(saveNo);
		const auto clearNo = saveDataMgr.GetClearStageNum(saveNo);
		const auto stageNum = stageDataMgr.GetStageNum() - 2;
		TimeUtility::DrawTimeLeft(m_saveInfoUI->x + DRAW_SUB_SAVE_INFO_STR_X, m_saveInfoUI->y + DRAW_SAVE_INFO_TIME_Y - subY, time, FONT_SIZE_SAVE_INFO, COLOR_SELECT, L"プレイ時間：", nullptr, true);
		std::wostringstream oss;
		oss << L"クリア数　：" << clearNo << L" / " << stageNum;
		UIUtility::DrawShadowStrLeft(m_saveInfoUI->x + DRAW_SUB_SAVE_INFO_STR_X, m_saveInfoUI->y + DRAW_SAVE_INFO_CLEAR_NUM_Y - subY, 2, 2, COLOR_SELECT, oss.str(), FONT_SIZE_SAVE_INFO);
	}
	// 存在しない場合
	else
	{
		// エラー時
		if (m_isError)
		{
			if (m_isErrorShake) UIUtility::DrawShakeStrLeft(m_saveInfoUI->x + DRAW_SUB_SAVE_INFO_STR_X, m_saveInfoUI->y - subY, COLOR_ERROR, L"データが存在しません", FONT_SIZE_SAVE_INFO, SHAKE_SIZE_ERROR, true);
			else				UIUtility::DrawShadowStrLeft(m_saveInfoUI->x + DRAW_SUB_SAVE_INFO_STR_X, m_saveInfoUI->y - subY, 2, 2, COLOR_ERROR, L"データが存在しません", FONT_SIZE_SAVE_INFO);
		}
		// 通常時
		else
		{
			UIUtility::DrawShadowStrLeft(m_saveInfoUI->x + DRAW_SUB_SAVE_INFO_STR_X, m_saveInfoUI->y - subY, 2, 2, COLOR_SELECT, L"データが存在しません", FONT_SIZE_SAVE_INFO);
		}
	}
}

void SceneTitle::DrawPadUI() const
{
	// オプションなら描画しない
	if (m_scnMgr.IsOpenOption()) return;

	const auto fontPadH = FontManager::GetInstance().GetHandle(FONT_KAISOTAI, FONT_SIZE_PAD);
	if (m_state == State::RANKING)
	{
		// B:キャンセル
		DrawRotaGraphFast(DRAW_PAD_X, DRAW_PAD_Y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, DRAW_PAD_Y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"キャンセル", FONT_SIZE_PAD);
	}
	else
	{
		int y = DRAW_PAD_Y;
		// A:決定
		DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
		if (m_state != State::MENU)
		{
			// B:キャンセル
			y -= DRAW_PAD_Y_INTERVAL;
			DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
			UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"キャンセル", FONT_SIZE_PAD);
		}
	}
}

void SceneTitle::OnStart()
{
	auto& saveDataMgr = SaveDataManager::GetInstance();
	// 使用セーブデータの決定
	saveDataMgr.DecideUseSaveNo(m_saveDataNo);

	// セーブデータが存在しない場合
	if (!saveDataMgr.IsExist(m_saveDataNo))
	{
		// チュートリアルシーンに遷移
		auto next = std::make_shared<SceneTutorial>();
		m_scnMgr.Change(next);
	}
	else
	{
		// ステージ選択シーンに遷移
		auto next = std::make_shared<SceneStageSelect>();
		m_scnMgr.Change(next);
	}
}
