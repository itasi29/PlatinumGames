#include "SceneOption.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "StringUtility.h"
#include "NumUtility.h"
#include "Game.h"
#include "Files.h"
#include "SceneManager.h"
#include "SceneStageSelect.h"
#include "SceneMain.h"
#include "SceneTutorial.h"
#include "SoundManager.h"
#include "SaveDataManager.h"

namespace
{
	/* 列挙型 */
	// タイトルでのオプション
	enum class SelectTitleCurrent : int
	{
		SOUND,
		CLOSE,
		MAX,
	};
	// ステージ選択でのオプション
	enum class SelectStageSelectCurrent : int
	{
		SOUND,
		CAMERA,
		CLOSE,
		MAX,
	};;
	// メインでのオプション
	enum class SelectMainCurrent : int
	{
		SOUND,
		CAMERA,
		RESTART,
		RETURN_STAGE_SELECT,
		CLOSE,
		MAX
	};
	// 音変更
	enum class SoundCurrent
	{
		BGM,
		SE,
		MAX
	};
	// カメラ感度変更
	enum class CameraCurrent
	{
		MAX
	};
	// 確認変更
	enum class CheckCurrent
	{
		CANCEl,
		OK,
		MAX
	};

	// タイトルから来た時の文字列
	const std::vector<std::wstring> STRS_TITLE =
	{
		L"音量調整",
		L"閉じる"
	};
	// ステージ選択から来た時の文字列
	const std::vector<std::wstring> STRS_STAGE_SELECT =
	{
		L"音量調整",
		L"カメラ感度調整",
		L"閉じる"
	};
	// メイン・チュートリアルからの来た時の文字列
	const std::vector<std::wstring> STRS_MAIN =
	{
		L"音量調整",
		L"カメラ感度調整",
		L"リスタート",
		L"ステージ選択へ",
		L"閉じる"
	};
	// サウンド調整文字列
	const std::vector<std::wstring> STRS_SOUND =
	{
		L"BGM",
		L"SE"
	};
	// カメラ感度調整文字
	const std::wstring STR_CAMERA = L"カメラ感度調整";
	// リスタート文字列
	const std::wstring STR_RESTART_NOT_SELECT = L"チェックポイントからやり直します";
	const std::wstring STR_RESTART_SELECT = L"チェックポイントまで戻りますか？";
	// リスタートフレーム文字列
	const std::vector<std::wstring> STRS_RESTART =
	{
		L"戻らない",
		L"戻る",
	};
	// リターン文字列
	const std::wstring STR_RETURN_NOT_SELECT = L"ステージ選択に戻ります";
	const std::wstring STR_RETURN_SELECT = L"ステージ選択に戻りますか？";
	// リターンフレーム文字列
	const std::vector<std::wstring> STRS_RETURN =
	{
		L"戻らない",
		L"戻る"
	};
	// オプション閉じる文字
	const std::wstring STR_CLOSE = L"オプション画面を閉じます";
	
	/* メイン*/
	constexpr int DRAW_MAIN_Y = 160;	// 描画位置(y)
	constexpr int DRAW_MAIN_FRAME_X = 200;	// フレーム描画位置(x)
	constexpr int DRAW_MAIN_STR_X = 96;	// 文字列描画位置(x)
	constexpr int DRAW_MAIN_ARM_X = 320;	// アーム描画位置(x)
	constexpr int DRAW_MAIN_ARM_FRAME_X = 392;	// アームフレーム描画位置(x)
	constexpr int DRAW_MAIN_ARM_FRAME_Y = 360;	// アームフレーム描画位置(y)
	constexpr int DRAW_MAIN_ARM_WINDOW_X = 376;	// アームウィンドウ描画位置(x)
	constexpr int DRAW_MAIN_ARM_WINDOW_Y = 110;	// アームウィンドウ描画位置(y)
	constexpr int DRAW_MAIN_Y_INTERVAL = 64;	// 描画間隔(y)
	constexpr int FONT_SIZE_MAIN = 32;		// 文字列フォントサイズ
	constexpr float FILE_SIZEE_MAIN_FRAME = 1.0f;	// フレーム画像サイズ
	constexpr float FILE_SIZE_MAIN_ARM = 1.0f;	// アーム画像サイズ
	constexpr float FILE_SIZE_MAIN_ARM_FRAME = 2.0f;	// アームフレーム画像サイズ
	constexpr float FILE_SIZE_MAIN_ARM_WINDOW_W = 3.0f;	// アームウィンドウ画像サイズ(w)
	constexpr float FILE_SIZE_MAIN_ARM_WINDOW_H = 2.0f;	// アームウィンドウ画像サイズ(h)
	constexpr unsigned int COLOR_SELECT = 0xffffff;		// 選択文字列カラー
	constexpr unsigned int COLOR_NOT_SELECT = 0x888888;	// 未選択文字列カラー

	/* サブ */
	constexpr int DRAW_SUB_Y = 160;			// 描画位置(y)
	constexpr int DRAW_SUB_FRAME_X = 500;	// フレーム描画位置(x)
	constexpr int DRAW_SUB_STR_X = 400;		// 文字列描画位置(x)

	/* サウンド調整 */
	constexpr int DRAW_SOUND_BAR_X = 408;	// バー描画位置(x)
	constexpr int DRAW_SOUND_BAR_Y = 256;	// バー描画位置(y)
	constexpr int DRAW_SOUND_POINT_X = 424;		// ポイント描画位置(x)
	constexpr int DRAW_SUB_SOUND_POINT_Y = 4;	// ポイント差分描画位置(y)
	constexpr int DRAW_SOUND_VOLUME_X = 504;		// 音量部分描画位置(x)
	constexpr int DRAW_SUB_SOUND_VOLUME_Y = -48;	// 音量差分描画位置(y)
	constexpr int DRAW_SOUND_Y_INTERVAL = 160;		 // 描画間隔(y)
	constexpr int DRAW_SOUND_POINT_X_INTERVAL = 648; // ポイント描画間隔(y)
	constexpr int FONT_SIZE_SOUND = 32;		// フォントサイズ
	constexpr float FILE_SIZE_VOLUME = 0.5f;// 音量画像サイズ
	constexpr unsigned int COLOR_SOUND = 0xffffff;	// 文字カラー

	/* カメラ感度系 */
	constexpr int DRAW_CAMERA_SENS_X = 504;	// カメラ感度描画位置(x)
	constexpr int DRAW_CAMERA_SENS_Y = 228;	// カメラ感度描画位置(y)
	constexpr int DRAW_CAMERA_POINT_X = 424;	// ポイント描画位置(x)
	constexpr int DRAW_CAMERA_POINT_Y = 280;	// ポイント描画位置(y)
	constexpr int DRAW_CAMERA_BAR_X = 408;	// バー描画位置(x)
	constexpr int DRAW_CAMERA_BAR_Y = 276;	// バー描画位置(y)
	constexpr int DRAW_CAMERA_POINT_X_INTERVAL = 648;	// ポイント描画間隔(x)
	constexpr int DRAW_CAMERA_BAR_X_INTERVAL = 340;	// バー描画間隔(x)
	constexpr int DRAW_CAMERA_BAR_X_CENTER = DRAW_CAMERA_BAR_X + DRAW_CAMERA_BAR_X_INTERVAL;	// バー中心描画位置(x)
	constexpr int CAMERA_SENS_CENTER = 50;	// カメラ感度中心値
	constexpr int FONT_SIZE_CAMERA = 32;	// フォントサイズ

	/* リスタート系 */
	constexpr int DRAW_RESTART_STR_X = 750;
	constexpr int DRAW_RESTART_STR_Y_NOT_SELECT = 360;
	constexpr int DRAW_RESTART_STR_Y_SELECT = 260;
	constexpr int DRAW_RESTART_FRAME_X = 580;
	constexpr int DRAW_RESTART_FRAME_Y = 440;
	constexpr int DRAW_RESTART_FRAME_X_INTERVAL = 340;
	constexpr int DRAW_RESTART_FRAME_STR_X = 480;
	constexpr int FONT_SIZE_RESTART = 48;
	constexpr int FONT_SIZE_RESTART_FRAME = 32;
	constexpr unsigned int COLOR_RESTART = 0xffffff;

	/* ステージ選択系 */
	constexpr int DRAW_RETURN_STR_X = 750;
	constexpr int DRAW_RETURN_STR_Y_NOT_SELECT = 360;
	constexpr int DRAW_RETURN_STR_Y_SELECT = 260;
	constexpr int DRAW_RETURN_FRAME_X = 580;
	constexpr int DRAW_RETURN_FRAME_Y = 440;
	constexpr int DRAW_RETURN_FRAME_X_INTERVAL = 340;
	constexpr int DRAW_RETURN_FRAME_STR_X = 480;
	constexpr int FONT_SIZE_RETURN = 48;
	constexpr int FONT_SIZE_RETURN_FRAME = 32;
	constexpr unsigned int COLOR_RETURN = 0xffffff;

	/* 閉じる */
	constexpr int DRAW_CLOSE_STR_X = 750;
	constexpr int DRAW_CLOSE_STR_Y = 360;
	constexpr int FONT_SIZE_CLOSE = 48;
	constexpr unsigned int COLOR_CLOSE = 0xffffff;

	/* フレーム点滅関係 */
	constexpr float FADE_SPEED_FRAME = 0.02f;	// 点滅速度
	constexpr int ALPHA_SIZE_FRAME = 48;		// 点滅サイズ

	/* PadUi描画 */
	constexpr int DRAW_PAD_Y = 650;			// 描画位置
	constexpr int DRAW_PAD_X = 904;			// 描画位置
	constexpr int DRAW_PAD_STR_X = 924;	// 文字列描画位置
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int DRAW_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー
	constexpr int DRAW_PAD_X_INTERVAL = 128;	// 描画間隔

	constexpr int REPEATE_INTERVAL = 12;	// リピート間隔
	constexpr float MOVE_SPEED = 1 / 5.0f;	// UIの移動速度

	// 変更音量
	constexpr int CHANGE_VOLUME = 1;
	// サウンド更新を行う番号のマックス値
	constexpr int SOUND_CHECK_CURRENT_MAX = 2;

	// 文字のウェーブサイズ
	constexpr int WAVE_SIZE = 4;
	// ウェーブスピード
	constexpr float WAVE_SPEED = 2.0f;
	// リピート間隔
	constexpr int SOUND_REPEATE_INTERVAL = 1;
	// スタートリピート間隔
	constexpr int START_REPEATE_INTERVAL = 10;
	// フェードスピード
	constexpr float FADE_SPEED = 0.1f;
	// フェード時の画面外の値
	constexpr int DRAW_OUT_VAL = 360;
}

SceneOption::SceneOption() :
	SceneBase(SceneKind::OPTION),
	m_updateFunc(&SceneOption::FadeInUpdate),
	m_drawFunc(&SceneOption::DrawFade),
	m_subDrawFunc(&SceneOption::DrawSound),
	m_scene(nullptr),
	m_openScene(SceneKind::TITLE),
	m_preRepeatKind(RepeatKind::NONE),
	m_mainCurrent(0),
	m_subCurrent(0),
	m_uiCount(0),
	m_fadeCount(0.0f),
	m_isNowMenu(true)
{
}

void SceneOption::AsyncInit()
{
}

void SceneOption::Init()
{
	// ファイルロード
	auto& fileMgr = FileManager::GetInstance();
	m_files[I_OPTION_WINDOW]	= fileMgr.Load(I_OPTION_WINDOW);
	m_files[I_FRAME]			= fileMgr.Load(I_FRAME);
	m_files[I_BASE_BAR]			= fileMgr.Load(I_BASE_BAR);
	m_files[I_MAIN_BAR]			= fileMgr.Load(I_MAIN_BAR);
	m_files[I_PLUS_BAR]			= fileMgr.Load(I_PLUS_BAR);
	m_files[I_VOLUME_POINT]		= fileMgr.Load(I_VOLUME_POINT);
	m_files[I_PAD_A]			= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]			= fileMgr.Load(I_PAD_B);
	m_files[I_SAVE_ARM]			= fileMgr.Load(I_SAVE_ARM);
	m_files[I_SAVE_ARM_FRAME]	= fileMgr.Load(I_SAVE_ARM_FRAME);
	m_files[I_SAVE_WINDOW]		= fileMgr.Load(I_SAVE_WINDOW);
	m_files[S_CANCEL]			= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]			= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]		= fileMgr.Load(S_CURSOR_MOVE);
	// 文字列登録
	m_selectStrList[SceneKind::TITLE]			= STRS_TITLE;
	m_selectStrList[SceneKind::STAGE_SELECT]	= STRS_STAGE_SELECT;
	m_selectStrList[SceneKind::MAIN]			= STRS_MAIN;
	m_selectStrList[SceneKind::TUTORIAL]		= STRS_MAIN;
	// UIMoveData作成
	m_mainUI = UIMoveData::Make(DRAW_MAIN_FRAME_X, DRAW_MAIN_Y);
	m_subUI = UIMoveData::Make(DRAW_SUB_FRAME_X, DRAW_SUB_Y);
	// サウンドの一時停止
	SoundManager::GetInstance().StopNowPlaySe();
}

void SceneOption::End()
{
	// 停止していたサウンドを再生
	SoundManager::GetInstance().RestartStopedSe();
}

void SceneOption::Update(bool isFade)
{
	m_uiCount++;
	m_mainUI->Update(MOVE_SPEED, EasingType::LERP);
	m_subUI->Update(MOVE_SPEED, EasingType::LERP);
	if (isFade) return;
	(this->*m_updateFunc)();
}

void SceneOption::Draw() const
{
	(this->*m_drawFunc)();
	
}

void SceneOption::CheckOpenScene(SceneKind openScene)
{
	m_openScene = openScene;
}

void SceneOption::FadeInUpdate()
{
	// フェード更新
	m_fadeCount += FADE_SPEED;
	// フェード中は処理終了
	if (m_fadeCount < 1.0f) return;
	
	m_drawFunc = &SceneOption::DrawCommon;
	m_subDrawFunc = &SceneOption::DrawSound;
	OnMenu();
}

void SceneOption::FadeOutUpdate()
{
	// フェード更新
	m_fadeCount -= FADE_SPEED;
	// フェード終了したらオプションを閉じる
	if (m_fadeCount < 0) m_scnMgr.CloseOption();
}

void SceneOption::MainTitleUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	bool isMove = false;
	if (CursorUtility::CursorUp<SelectTitleCurrent>(m_mainCurrent, SelectTitleCurrent::MAX, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<SelectTitleCurrent>(m_mainCurrent, SelectTitleCurrent::MAX, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_mainUI->ChangeVertical(DRAW_MAIN_Y + DRAW_MAIN_Y_INTERVAL * m_mainCurrent);
		ChangeSubDraw();
		sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}

	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// 音量調整
		if (m_mainCurrent == static_cast<int>(SelectTitleCurrent::SOUND))
		{
			OnSound();
		}
		// 閉じる
		if (m_mainCurrent == static_cast<int>(SelectTitleCurrent::CLOSE))
		{
			OnFadeOut();
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// 閉じる
		OnFadeOut();
	}
}

void SceneOption::MainStageSelectUpdate()
{
	bool isMove = false;
	if (CursorUtility::CursorUp<SelectStageSelectCurrent>(m_mainCurrent, SelectStageSelectCurrent::MAX, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<SelectStageSelectCurrent>(m_mainCurrent, SelectStageSelectCurrent::MAX, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_mainUI->ChangeVertical(DRAW_MAIN_Y + DRAW_MAIN_Y_INTERVAL * m_mainCurrent);
		ChangeSubDraw();
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}

	auto& input = Input::GetInstance();
	auto& sndMgr = SoundManager::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// 音量調整
		if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::SOUND))
		{
			OnSound();
		}
		// カメラ感度調整
		else if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::CAMERA))
		{
			OnCamera();
		}
		// 閉じる
		else if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::CLOSE))
		{
			OnFadeOut();
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// 閉じる
		OnFadeOut();
	}
}

void SceneOption::MainMainUpdate()
{
	bool isMove = false;
	if (CursorUtility::CursorUp<SelectMainCurrent>(m_mainCurrent, SelectMainCurrent::MAX, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<SelectMainCurrent>(m_mainCurrent, SelectMainCurrent::MAX, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_mainUI->ChangeVertical(DRAW_MAIN_Y + DRAW_MAIN_Y_INTERVAL * m_mainCurrent);
		ChangeSubDraw();
		SoundManager::GetInstance().PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}

	auto& sndMgr = SoundManager::GetInstance();
	auto& input = Input::GetInstance();
	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// サウンド
		if (m_mainCurrent == static_cast<int>(SelectMainCurrent::SOUND))
		{
			OnSound();
		}
		// カメラ感度
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::CAMERA))
		{
			OnCamera();
		}
		// リスタート
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::RESTART))
		{
			OnRestart();
		}
		// ステージ選択に戻る
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::RETURN_STAGE_SELECT))
		{
			OnReturnStageSelect();
		}
		// 閉じる
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::CLOSE))
		{
			OnFadeOut();
		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// 閉じる
		OnFadeOut();
	}
}

void SceneOption::SoundUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	auto& input = Input::GetInstance();
	bool isMove = false;
	if (CursorUtility::CursorUp<SoundCurrent>(m_subCurrent, SoundCurrent::MAX, Command::BTN_UP, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<SoundCurrent>(m_subCurrent, SoundCurrent::MAX, Command::BTN_DOWN, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_subUI->ChangeVertical(DRAW_SUB_Y + DRAW_SOUND_Y_INTERVAL * m_subCurrent);
		sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}
	if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		OnMenu();
		return;
	}

	auto& saveDataMgr = SaveDataManager::GetInstance();
	bool isChange = false;
	RepeatKind kind;
	// 音量減少
	if (input.IsRepeat(Command::BTN_LEFT, kind, SOUND_REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		if (m_subCurrent == static_cast<int>(SoundCurrent::BGM))	 saveDataMgr.ChangeBgmRate(-CHANGE_VOLUME);
		else if (m_subCurrent == static_cast<int>(SoundCurrent::SE)) saveDataMgr.ChangeSeRate(-CHANGE_VOLUME);
		isChange = true;
	}
	// 上をリピートしていない場合、音量増加
	else if (kind == RepeatKind::NONE && input.IsRepeat(Command::BTN_RIGHT, kind, SOUND_REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		if (m_subCurrent == static_cast<int>(SoundCurrent::BGM))	 saveDataMgr.ChangeBgmRate(CHANGE_VOLUME);
		else if (m_subCurrent == static_cast<int>(SoundCurrent::SE)) saveDataMgr.ChangeSeRate(CHANGE_VOLUME);
		isChange = true;
	}

	// 変更していない時
	if (!isChange)
	{
		// ボリューム変更を行っているのがSEの時
		if (m_subCurrent == static_cast<int>(SoundCurrent::SE))
		{
			// リピートから離した時なら音を鳴らす
			const bool isReverse = m_preRepeatKind == RepeatKind::COOL_DOWN && kind == RepeatKind::NONE;
			if (isReverse) sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
		}
	}
	m_preRepeatKind = kind;
}

void SceneOption::CameraUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		OnMenu();
		return;
	}

	auto& saveDataMgr = SaveDataManager::GetInstance();
	bool isChange = false;
	RepeatKind kind;
	// カメラ感度変更
	if (input.IsRepeat(Command::BTN_LEFT, kind, SOUND_REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		saveDataMgr.ChangeCameraSens(-CHANGE_VOLUME);
		isChange = true;
	}
	else if (kind == RepeatKind::NONE && input.IsRepeat(Command::BTN_RIGHT, kind, SOUND_REPEATE_INTERVAL, START_REPEATE_INTERVAL))
	{
		saveDataMgr.ChangeCameraSens(CHANGE_VOLUME);
		isChange = true;
	}

	// 変更していない時
	if (!isChange)
	{
		// リピートから離した時なら音を鳴らす
		const bool isReverse = m_preRepeatKind == RepeatKind::COOL_DOWN && kind == RepeatKind::NONE;
		if (isReverse) sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}
}

void SceneOption::RestartUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	auto& input = Input::GetInstance();
	bool isMove = false;
	if (CursorUtility::CursorUp<CheckCurrent>(m_subCurrent, CheckCurrent::MAX, Command::BTN_RIGHT, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<CheckCurrent>(m_subCurrent, CheckCurrent::MAX, Command::BTN_LEFT, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_subUI->ChangeHorizontal(DRAW_RESTART_FRAME_X + DRAW_RESTART_FRAME_X_INTERVAL * m_subCurrent);
		sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}

	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// リスタート
		if (m_subCurrent == static_cast<int>(CheckCurrent::OK))
		{
			const auto nowKind = m_scene->GetNowKind();
			if (nowKind == SceneKind::MAIN)			 dynamic_cast<SceneMain*>(m_scene)->OnRestart();
			else if (nowKind == SceneKind::TUTORIAL) dynamic_cast<SceneTutorial*>(m_scene)->OnRestart();
			OnFadeOut();
		}
		// キャンセル
		else if (m_subCurrent == static_cast<int>(CheckCurrent::CANCEl))
		{
			// メニューに戻る
			OnMenu();
		}
	}
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// メニューに戻る
		OnMenu();
	}
}

void SceneOption::ReturnStageSelectUpdate()
{
	auto& sndMgr = SoundManager::GetInstance();
	auto& input = Input::GetInstance();
	bool isMove = false;
	if (CursorUtility::CursorUp<CheckCurrent>(m_subCurrent, CheckCurrent::MAX, Command::BTN_RIGHT, true, REPEATE_INTERVAL)) isMove = true;
	if (CursorUtility::CursorDown<CheckCurrent>(m_subCurrent, CheckCurrent::MAX, Command::BTN_LEFT, true, REPEATE_INTERVAL)) isMove = true;
	if (isMove)
	{
		m_uiCount = 0;
		m_subUI->ChangeHorizontal(DRAW_RETURN_FRAME_X + DRAW_RETURN_FRAME_X_INTERVAL * m_subCurrent);
		sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle(), true);
	}

	if (input.IsTriggerd(Command::BTN_OK))
	{
		sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle(), true);
		// ステージ選択に戻る
		if (m_subCurrent == static_cast<int>(CheckCurrent::OK))
		{
			auto next = std::make_shared<SceneStageSelect>();
			m_scnMgr.Change(next);
			OnFadeOut();
		}
		// キャンセル
		else if (m_subCurrent == static_cast<int>(CheckCurrent::CANCEl))
		{
			// メニューに戻る
			OnMenu();
		}
	}
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle(), true);
		// メニューに戻る
		OnMenu();
	}
}

void SceneOption::ChangeSubDraw()
{
	// タイトルの時のアップデート
	if (m_openScene == SceneKind::TITLE)
	{
		if (m_mainCurrent == static_cast<int>(SelectTitleCurrent::SOUND))		m_subDrawFunc = &SceneOption::DrawSound;
		else if (m_mainCurrent == static_cast<int>(SelectTitleCurrent::CLOSE))	m_subDrawFunc = &SceneOption::DrawClose;
	}
	// ステージ選択の時のアップデート
	else if (m_openScene == SceneKind::STAGE_SELECT)
	{
		if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::SOUND))			m_subDrawFunc = &SceneOption::DrawSound;
		else if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::CAMERA))	m_subDrawFunc = &SceneOption::DrawCamera;
		else if (m_mainCurrent == static_cast<int>(SelectStageSelectCurrent::CLOSE))	m_subDrawFunc = &SceneOption::DrawClose;
	}
	// メインの時のアップデート
	else if (m_openScene == SceneKind::MAIN || m_openScene == SceneKind::TUTORIAL)
	{
		if (m_mainCurrent == static_cast<int>(SelectMainCurrent::SOUND))					m_subDrawFunc = &SceneOption::DrawSound;
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::CAMERA))				m_subDrawFunc = &SceneOption::DrawCamera;
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::RESTART))				m_subDrawFunc = &SceneOption::DrawRestart;
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::RETURN_STAGE_SELECT)) m_subDrawFunc = &SceneOption::DrawReturnStageSelect;
		else if (m_mainCurrent == static_cast<int>(SelectMainCurrent::CLOSE))				m_subDrawFunc = &SceneOption::DrawClose;
	}
}

void SceneOption::OnMenu()
{
	m_isNowMenu = true;
	// タイトルの時のアップデート
	if (m_openScene == SceneKind::TITLE)
	{
		m_updateFunc = &SceneOption::MainTitleUpdate;
	}
	// ステージ選択の時のアップデート
	else if (m_openScene == SceneKind::STAGE_SELECT)
	{
		m_updateFunc = &SceneOption::MainStageSelectUpdate;
	}
	// メインの時のアップデート
	else if (m_openScene == SceneKind::MAIN || m_openScene == SceneKind::TUTORIAL)
	{
		m_updateFunc = &SceneOption::MainMainUpdate;
	}
	// それ以外の時
	else
	{
		assert(false && "開くシーンがおかしいです");
	}
}

void SceneOption::OnFadeOut()
{
	m_fadeCount = 1.0f;
	m_updateFunc = &SceneOption::FadeOutUpdate;
	m_drawFunc = &SceneOption::DrawFade;
}

void SceneOption::OnSound()
{
	m_isNowMenu = false;
	m_subCurrent = 0;
	m_subUI->ChangeVerticalImmediate(DRAW_SUB_Y);
	m_subUI->ChangeHorizontalImmediate(DRAW_SUB_FRAME_X);
	m_updateFunc = &SceneOption::SoundUpdate;
}

void SceneOption::OnCamera()
{
	m_isNowMenu = false;
	m_subCurrent = 0;
	m_subUI->ChangeVerticalImmediate(DRAW_SUB_Y);
	m_subUI->ChangeHorizontalImmediate(DRAW_SUB_FRAME_X);
	m_updateFunc = &SceneOption::CameraUpdate;
}

void SceneOption::OnRestart()
{
	m_isNowMenu = false;
	m_subCurrent = 0;
	m_subUI->ChangeHorizontalImmediate(DRAW_RESTART_FRAME_X);
	m_subUI->ChangeVerticalImmediate(DRAW_RESTART_FRAME_Y);
	m_updateFunc = &SceneOption::RestartUpdate;
}

void SceneOption::OnReturnStageSelect()
{
	m_isNowMenu = false;
	m_subCurrent = 0;
	m_subUI->ChangeHorizontalImmediate(DRAW_RETURN_FRAME_X);
	m_subUI->ChangeVerticalImmediate(DRAW_RETURN_FRAME_Y);
	m_updateFunc = &SceneOption::ReturnStageSelectUpdate;
}

void SceneOption::DrawFade() const
{
	int y = Easing::Lerp(-DRAW_OUT_VAL, Game::CENTER_Y, m_fadeCount);
	DrawRotaGraphFast(Game::CENTER_X, y, 1.0f, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
}

void SceneOption::DrawCommon() const
{
	// ウィンドウ描画
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, 0.0f, m_files.at(I_OPTION_WINDOW)->GetHandle(), true);
	// アーム＆ウィンドウ
	DrawRotaGraphFast(DRAW_MAIN_ARM_X,			m_mainUI->y,				FILE_SIZE_MAIN_ARM,			0.0f, m_files.at(I_SAVE_ARM)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_MAIN_ARM_FRAME_X,	DRAW_MAIN_ARM_FRAME_Y,	FILE_SIZE_MAIN_ARM_FRAME,	0.0f, m_files.at(I_SAVE_ARM_FRAME)->GetHandle(), true);
	DrawRotaGraphFast3(DRAW_MAIN_ARM_WINDOW_X,  DRAW_MAIN_ARM_WINDOW_Y, 0, 0, FILE_SIZE_MAIN_ARM_WINDOW_W, FILE_SIZE_MAIN_ARM_WINDOW_H, 0.0f, m_files.at(I_SAVE_WINDOW)->GetHandle(), true);
	// フレーム描画
	if (m_isNowMenu)
	{
		const float rate = std::cosf(m_uiCount * FADE_SPEED_FRAME);
		const float halfSize = ALPHA_SIZE_FRAME * 0.5f;
		const int alpha = static_cast<int>(Game::ALPHA_VAL - halfSize + halfSize * rate);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	}
	DrawRotaGraphFast(m_mainUI->x, m_mainUI->y, FILE_SIZEE_MAIN_FRAME, 0.0f, m_files.at(I_FRAME)->GetHandle(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// メインとなる文字列
	const auto& selectStr = m_selectStrList.at(m_openScene);
	for (int i = 0; i <  selectStr.size(); ++i)
	{
		const auto& str = selectStr.at(i);
		const bool isSelect = i == m_mainCurrent;
		int y = DRAW_MAIN_Y + DRAW_MAIN_Y_INTERVAL * i;
		if (isSelect && m_isNowMenu)	UIUtility::DrawWaveStrLeft(DRAW_MAIN_STR_X, y, COLOR_SELECT, str, FONT_SIZE_MAIN, m_uiCount * WAVE_SPEED, WAVE_SIZE, true);
		else if (isSelect)				UIUtility::DrawShadowStrLeft(DRAW_MAIN_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_SELECT, str, FONT_SIZE_MAIN);
		else							UIUtility::DrawShadowStrLeft(DRAW_MAIN_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_NOT_SELECT, str, FONT_SIZE_MAIN);
	}
	// それぞれの描画処理
	(this->*m_subDrawFunc)();
	// PadUIの描画
	DrawPadUI();
	return;
}

void SceneOption::DrawSound() const
{
	DrawSubFrame();
	for (int i = 0; i < static_cast<int>(SoundCurrent::MAX); ++i)
	{
		int y = DRAW_SUB_Y + DRAW_SOUND_Y_INTERVAL * i;
		if (i == m_subCurrent && !m_isNowMenu) UIUtility::DrawWaveStrLeft(DRAW_SUB_STR_X, y, COLOR_SELECT, STRS_SOUND.at(i), FONT_SIZE_SOUND, m_uiCount * WAVE_SPEED, WAVE_SIZE, true);
		else									UIUtility::DrawShadowStrLeft(DRAW_SUB_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_NOT_SELECT, STRS_SOUND.at(i), FONT_SIZE_SOUND);
	}
	auto& sndMgr = SoundManager::GetInstance();
	auto& saveDataMgr = SaveDataManager::GetInstance();
	DrawSoundBar(DRAW_SOUND_BAR_Y, saveDataMgr.GetBgmVolume());
	DrawSoundBar(DRAW_SOUND_BAR_Y + DRAW_SOUND_Y_INTERVAL, saveDataMgr.GetSeVolume());
}

void SceneOption::DrawCamera() const
{
	DrawSubFrame();
	if (!m_isNowMenu)	UIUtility::DrawWaveStrLeft(DRAW_SUB_STR_X, DRAW_SUB_Y, COLOR_SELECT, STR_CAMERA, FONT_SIZE_CAMERA, m_uiCount * WAVE_SPEED, WAVE_SIZE, true);
	else				UIUtility::DrawShadowStrLeft(DRAW_SUB_STR_X, DRAW_SUB_Y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_SELECT, STR_CAMERA, FONT_SIZE_CAMERA);

	auto& saveDataMgr = SaveDataManager::GetInstance();
	const int sens = saveDataMgr.GetCameraSens();
	const float rate = sens * 0.01f;

	int w, h;
	GetGraphSize(m_files.at(I_MAIN_BAR)->GetHandle(), &w, &h);
	// バー描画
	DrawGraph(DRAW_CAMERA_BAR_X, DRAW_CAMERA_BAR_Y, m_files.at(I_BASE_BAR)->GetHandle(), true);
	int barX = 0;
	int srcW = 0;
	int srcX = 0;
	int handle = -1;
	if (sens < 50)
	{
		barX = DRAW_CAMERA_BAR_X + static_cast<int>(DRAW_CAMERA_BAR_X_INTERVAL * rate * 2);
		srcX = 0;
		srcW = static_cast<int>(w * (0.5f - rate));
		while (barX + srcW > DRAW_CAMERA_BAR_X_CENTER) --srcW;
		while (barX + srcW < DRAW_CAMERA_BAR_X_CENTER) ++srcW;
		handle = m_files.at(I_MAIN_BAR)->GetHandle();
	}
	else
	{
		barX = DRAW_CAMERA_BAR_X + DRAW_CAMERA_BAR_X_INTERVAL;
		srcX = static_cast<int>(w * 0.5f);
		srcW = static_cast<int>(w * (rate - 0.5f));
		handle = m_files.at(I_PLUS_BAR)->GetHandle();
	}
	DrawRectGraph(barX, DRAW_CAMERA_BAR_Y, srcX, 0, srcW, h, handle, true);
	// ポイント描画
	int x = DRAW_CAMERA_POINT_X + static_cast<int>(DRAW_CAMERA_POINT_X_INTERVAL * rate);
	DrawRotaGraphFast(x, DRAW_CAMERA_POINT_Y, 1.0f, 0.0f, m_files.at(I_VOLUME_POINT)->GetHandle(), true);

	// 割合描画
	auto& util = NumUtility::GetInstance();
	const int drawSens = sens - CAMERA_SENS_CENTER;
	util.DrawNumberMore(DRAW_CAMERA_SENS_X, DRAW_CAMERA_SENS_Y, FILE_SIZE_VOLUME, COLOR_SOUND, drawSens, 2, true);
}

void SceneOption::DrawRestart() const
{
	std::wstring str = STR_RESTART_NOT_SELECT;
	int y = DRAW_RESTART_STR_Y_NOT_SELECT;
	if (!m_isNowMenu)
	{
		str = STR_RESTART_SELECT;
		y = DRAW_RESTART_STR_Y_SELECT;
	}
	UIUtility::DrawShadowStr(DRAW_RESTART_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_RESTART, str, FONT_SIZE_RESTART);

	if (m_isNowMenu) return;
	DrawSubFrame();
	for (int i = 0; i < static_cast<int>(CheckCurrent::MAX); ++i)
	{
		int x = DRAW_RESTART_FRAME_STR_X + DRAW_RESTART_FRAME_X_INTERVAL * i;
		if (i == m_subCurrent && !m_isNowMenu)	UIUtility::DrawWaveStrLeft(x, DRAW_RESTART_FRAME_Y, COLOR_SELECT, STRS_RESTART.at(i), FONT_SIZE_RESTART_FRAME, m_uiCount * WAVE_SPEED, WAVE_SIZE, true);
		else									UIUtility::DrawShadowStrLeft(x, DRAW_RESTART_FRAME_Y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_NOT_SELECT, STRS_RESTART.at(i), FONT_SIZE_RESTART_FRAME);
	}

}

void SceneOption::DrawReturnStageSelect() const
{
	std::wstring str = STR_RETURN_NOT_SELECT;
	int y = DRAW_RETURN_STR_Y_NOT_SELECT;
	if (!m_isNowMenu)
	{
		str = STR_RETURN_SELECT;
		y = DRAW_RETURN_STR_Y_SELECT;
	}
	UIUtility::DrawShadowStr(DRAW_RETURN_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_RETURN, str, FONT_SIZE_RETURN);

	if (m_isNowMenu) return;
	DrawSubFrame();
	for (int i = 0; i < static_cast<int>(CheckCurrent::MAX); ++i)
	{
		int x = DRAW_RETURN_FRAME_STR_X + DRAW_RETURN_FRAME_X_INTERVAL * i;
		if (i == m_subCurrent && !m_isNowMenu)	UIUtility::DrawWaveStrLeft(x, DRAW_RETURN_FRAME_Y, COLOR_SELECT, STRS_RETURN.at(i), FONT_SIZE_RETURN_FRAME, m_uiCount * WAVE_SPEED, WAVE_SIZE, true);
		else									UIUtility::DrawShadowStrLeft(x, DRAW_RETURN_FRAME_Y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_NOT_SELECT, STRS_RETURN.at(i), FONT_SIZE_RETURN_FRAME);
	}
}

void SceneOption::DrawClose() const
{
	UIUtility::DrawShadowStr(DRAW_CLOSE_STR_X, DRAW_CLOSE_STR_Y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_CLOSE, STR_CLOSE, FONT_SIZE_CLOSE);
}

void SceneOption::DrawSoundBar(int y, int volume) const
{
	float rate = volume * 0.01f;
	int w, h;
	GetGraphSize(m_files.at(I_MAIN_BAR)->GetHandle(), &w, &h);
	// バー描画
	DrawGraph(DRAW_SOUND_BAR_X, y, m_files.at(I_BASE_BAR)->GetHandle(), true);
	DrawRectGraph(DRAW_SOUND_BAR_X, y, 0, 0, static_cast<int>(w * rate), h, m_files.at(I_MAIN_BAR)->GetHandle(), true);
	// ポイント描画
	int x = DRAW_SOUND_POINT_X + static_cast<int>(DRAW_SOUND_POINT_X_INTERVAL * rate);
	DrawRotaGraphFast(x, y + DRAW_SUB_SOUND_POINT_Y, 1.0f, 0.0f, m_files.at(I_VOLUME_POINT)->GetHandle(), true);
	// 音量描画
	auto& util = NumUtility::GetInstance();
	util.DrawNumberMore(DRAW_SOUND_VOLUME_X, y + DRAW_SUB_SOUND_VOLUME_Y, FILE_SIZE_VOLUME, COLOR_SOUND, volume, 3, true);
}

void SceneOption::DrawSubFrame() const
{
	if (m_isNowMenu) return;

	const float rate = std::cosf(m_uiCount * FADE_SPEED_FRAME);
	const float halfSize = ALPHA_SIZE_FRAME * 0.5f;
	const int alpha = static_cast<int>(Game::ALPHA_VAL - halfSize + halfSize * rate);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawRotaGraphFast(m_subUI->x, m_subUI->y, FILE_SIZEE_MAIN_FRAME, 0.0f, m_files.at(I_FRAME)->GetHandle(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneOption::DrawPadUI() const
{
	int y = DRAW_PAD_Y;
	// A:決定
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
	// B:閉じるor戻る
	std::wstring str = L"閉じる";
	if (!m_isNowMenu) str = L"戻る";
	DrawRotaGraphFast(DRAW_PAD_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X + DRAW_PAD_X_INTERVAL, y, DRAW_STR_SHADOW_POS, DRAW_STR_SHADOW_POS, COLOR_PAD, str, FONT_SIZE_PAD);
}
