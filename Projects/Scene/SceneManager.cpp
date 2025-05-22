#include "SceneManager.h"
#include <DxLib.h>
#include "SceneTitle.h"
#include "SceneOption.h"
#include "Game.h"
#include "Application.h"
#include "Files.h"
#include "Easing.h"
#include "SoundManager.h"
#ifdef _DEBUG
#include "SceneDebug.h"
#include "CommandList.h"
#endif

namespace
{
	// オプションで使うファイルID
	const wchar_t* const FILE_WINDOW = L"I_OptionWindow";
	const wchar_t* const FILE_COMMON_FRAME = L"I_CommonFrame";
	const wchar_t* const FILE_COMMON_SELECT_FRAME = L"I_CommonSelectFrame";
	const wchar_t* const FILE_SOUND_BAR = L"I_MainSoundBar";
	const wchar_t* const FILE_VOLUME_POINT = L"I_VolumePoint";
	const wchar_t* const FILE_CANCEL = L"S_Cancel";
	const wchar_t* const FILE_DECIDE = L"S_Decide";
	const wchar_t* const FILE_MOVE = L"S_CursorMove";
	// NowLoadingで使うファイルID
	const wchar_t* const FILE_LOADING = L"I_Loading";
	const wchar_t* const FILE_LOADING_FADE = L"I_LoadingFade";
	const wchar_t* const FILE_LOADING_UNDER = L"I_LoadingUnder";

	constexpr int FADE_FRAME_SAVE_NO = 30;
	constexpr float FADE_SPEED = 1.0f / FADE_FRAME_SAVE_NO;

	constexpr float NOW_LOADING_SPEED = 0.01f;
	constexpr float NOW_LOADING_ANGLE_SPEED = 0.05f;
	constexpr float NOW_LOADING_ANGLE_1 = 29.0f * Game::DEG_2_RAD;
	constexpr float NOW_LOADING_ANGLE_2 = -7.0f * Game::DEG_2_RAD;
}

SceneManager::SceneManager() :
	m_scene(nullptr),
	m_updateFunc(&SceneManager::NormalUpdate),
	m_drawFunc(&SceneManager::DrawNormal),
	m_bgmH(-1),
	m_count(0.0f),
	m_angleSpeed(0.0f),
	m_isFadeEndFrame(false)
{
}

SceneManager::~SceneManager()
{
}

SceneManager& SceneManager::GetInstance()
{
	static SceneManager mgr;
	return mgr;
}

void SceneManager::Init()
{
	// 最初のシーンのメモリ確保
#ifdef _DEBUG
	m_scene = std::make_shared<SceneDebug>();
#else
	m_scene = std::make_shared<SceneTitle>();
#endif

	// 常駐リソース読み込み
	auto& fileMgr = FileManager::GetInstance();
	fileMgr.Load(I_PAD_A, true);
	fileMgr.Load(I_PAD_B, true);
	fileMgr.Load(I_PAD_X, true);
	fileMgr.Load(I_PAD_Y, true);
	fileMgr.Load(I_PAD_LT, true);
	fileMgr.Load(I_PAD_RT, true);
	fileMgr.Load(I_PAD_RB, true);
	fileMgr.Load(I_OPTION_WINDOW, true);
	fileMgr.Load(I_FRAME, true);
	fileMgr.Load(I_COMMON_FRAME, true);
	fileMgr.Load(I_COMMON_SELECT_FRAME, true);
	fileMgr.Load(I_MAIN_BAR, true);
	fileMgr.Load(I_VOLUME_POINT, true);
	fileMgr.Load(S_CANCEL, true);
	fileMgr.Load(S_DECIDE, true);
	fileMgr.Load(S_CURSOR_MOVE, true);
	m_files[I_LOADING]		= fileMgr.Load(I_LOADING, true);
	m_files[I_LOADING_FADE]	= fileMgr.Load(I_LOADING_FADE, true);
	m_files[I_LOADING_UNDER]= fileMgr.Load(I_LOADING_UNDER, true);
	m_scene->AsyncInit();
	// 非同期ロードの開始
	m_updateFunc = &SceneManager::FileLoadingUpdate;
	m_drawFunc = &SceneManager::FileLoadingDraw;

	// オリジナルシェーダーの使用
	MV1SetUseOrigShader(true);
}

void SceneManager::Update()
{
	if (m_option)
	{
		m_option->Update(false);
		return;
	}
	(this->*m_updateFunc)();
}

void SceneManager::Draw() const
{
	(this->*m_drawFunc)();
	if (m_option)
	{
		m_option->Draw();
	}
}

void SceneManager::End()
{
	m_scene->End();
}

void SceneManager::Change(const std::shared_ptr<SceneBase>& next)
{
#ifdef _DEBUG
	printf("シーンの変更\n");
#endif
	m_count = 0.0f;
	// フェードアウトへ
	m_updateFunc = &SceneManager::FadeOut;
	m_drawFunc = &SceneManager::DrawFade;

	m_nextScene = next;
}

void SceneManager::ChangeBgmH(int handle)
{
	m_bgmH = handle;
}

void SceneManager::OpenOption(SceneKind openScene, SceneBase* scene)
{
	// 開いていれば終了
	if (m_option) return;

	auto next = std::make_shared<SceneOption>();
	next->Init();
	if (scene) next->Init(scene);
	next->CheckOpenScene(openScene);
	m_option = next;
}

void SceneManager::CloseOption()
{
	if (!m_option) return;

	m_option->End();
	m_option = nullptr;
}

SceneKind SceneManager::GetNowSceneKind() const
{
	return m_scene->GetNowKind();
}

void SceneManager::NormalUpdate()
{
#ifdef _DEBUG
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(KEY_INPUT_D))
	{
		auto next = std::make_shared<SceneDebug>();
		Change(next);
		return;
	}
#endif
	// BGMが切れたら再度再生
	auto& sndMgr = SoundManager::GetInstance();
	if (!sndMgr.IsNowPlay(m_bgmH)) sndMgr.PlayBgm(m_bgmH);
	// シーンの更新
	m_scene->Update(false);
	m_isFadeEndFrame = false;
}

void SceneManager::FadeOut()
{
	m_scene->Update(true);
	m_count += FADE_SPEED;

	// フェードアウト完了したら
	if (m_count > 1.0f)
	{
		m_count = 0.0f;
		m_angleSpeed = 0.0f;
		// 終了処理
		m_scene->End();
		// シーン変更
		m_scene = m_nextScene;
		// 初期化処理(非同期)
		m_scene->AsyncInit();
		// 非同期ロードの開始
		m_updateFunc = &SceneManager::FileLoadingUpdate;
		m_drawFunc = &SceneManager::FileLoadingDraw;
		return;
	}

	// ハンドルが入っていな場合は流さない
	if (m_bgmH > -1)
	{
		auto& snd = SoundManager::GetInstance();
		snd.PlayFadeBgm(m_bgmH, 1.0f - m_count);
	}
}

void SceneManager::FileLoadingUpdate()
{
	m_count = std::fmod(m_count + NOW_LOADING_SPEED, 1.0f);
	m_angleSpeed += NOW_LOADING_ANGLE_SPEED;
	
	auto num = GetASyncLoadNum();
	if (num <= 0)
	{
		m_count = 1.0f;
		m_isFadeEndFrame = false;
		// 初期化処理(同期)
		m_scene->Init();
		// 通常処理へ
		m_updateFunc = &SceneManager::FadeIn;
		m_drawFunc = &SceneManager::DrawFade;
	}
	printf("残りロード数：%d\n", num);
}

void SceneManager::FadeIn()
{
	m_scene->Update(true);
	m_count -= FADE_SPEED;

	// フェードイン完了したら
	if (m_count < 0.0f)
	{
		m_isFadeEndFrame = true;
		m_count = 0.0f;
		// シーンの処理へ
		m_updateFunc = &SceneManager::NormalUpdate;
		m_drawFunc = &SceneManager::DrawNormal;
	}

	// ハンドルが入っていな場合は流さない
	if (m_bgmH > -1)
	{
		auto& snd = SoundManager::GetInstance();
		snd.PlayFadeBgm(m_bgmH, 1.0f - m_count);
	}
}

void SceneManager::DrawNormal() const
{
	m_scene->Draw();
}

void SceneManager::DrawFade() const
{
	m_scene->Draw();
	// フェード処理
	const int alpha = static_cast<int>(Game::ALPHA_VAL * m_count);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::WINDOW_W, Game::WINDOW_H, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneManager::FileLoadingDraw() const
{
	int alpha = static_cast<int>(std::abs(Game::ALPHA_VAL * std::cosf(m_count * 2 - 1)));
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, 0.0f, m_files.at(I_LOADING_UNDER)->GetHandle(), true);
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, NOW_LOADING_ANGLE_1 * m_angleSpeed, m_files.at(I_LOADING_FADE)->GetHandle(), true);
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.25f, NOW_LOADING_ANGLE_2 * m_angleSpeed, m_files.at(I_LOADING_FADE)->GetHandle(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawRotaGraphFast(Game::CENTER_X, Game::CENTER_Y, 1.0f, 0.0f, m_files.at(I_LOADING)->GetHandle(), true);
}
