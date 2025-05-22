#include "Application.h"
#include <DxLib.h>
#include <chrono>
#include <ctime>
#include "RandomUtility.h"
#include "NumUtility.h"
#include "EffekseerManager.h"
#include "SoundManager.h"
#include "FileManager.h"
#include "FontManager.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "RankingDataManager.h"
#include "NovelManager.h"
#include "SceneManager.h"
#include "SceneHelp.h"
#include "Game.h"
#include "Input.h"
#include "MyEngine/MyEngine.h"
#include "SceneBase.h"

namespace
{
    // ウィンドウ名
    const wchar_t* const kWindowText = L"THE GATE:";
#ifdef _DEBUG
    const std::unordered_map<SceneKind, const char* const> SCENE_KIND =
    {
        { SceneKind::TITLE,         "タイトル" },
        { SceneKind::STAGE_SELECT,  "ステージ選択" },
        { SceneKind::MAIN,          "メイン" },
        { SceneKind::TUTORIAL,      "チュートリアル" },
        { SceneKind::CLEAR,         "クリア" },
        { SceneKind::OPTION,        "オプション" },
        { SceneKind::DEBUG,         "デバッグ" },
    };
#endif
}

Application::Application() :
    m_lightBuffer(nullptr),
	m_lightBufferH(-1),
    m_isEnd(false),
#ifdef _DEBUG
    m_isWindows(true),
    m_in(nullptr),
    m_out(nullptr)
#else
    m_isWindows(false)
#endif
{
}

Application& Application::GetInstance()
{
    static Application app;
    return app;
}

bool Application::Init()
{
#ifdef _DEBUG
    // コンソール画面生成
    AllocConsole();
    // stdout
    freopen_s(&m_out, "CON", "w", stdout);
    // stdin
    freopen_s(&m_in, "CON", "r", stdin);
    // デバッグコンソールがアクティブウィンドウになるのでゲーム本体のウィンドウをアクティブにする
    SetForegroundWindow(GetMainWindowHandle());
#endif
    ChangeWindowMode(m_isWindows);
    // スクリーン変更しても画像をDeleteしないように
    SetChangeScreenModeGraphicsSystemResetFlag(false);

    // 画面サイズ変更
    SetGraphMode(Game::WINDOW_W, Game::WINDOW_H, 32);
    SetWindowText(kWindowText);

	// DxLibの初期化
    if (DxLib_Init() == -1)
    {
        assert(false && "DxLibの初期化に失敗しました。");
        return false;
    }

	// 画面の描画先を裏画面にセット
    SetDrawScreen(DX_SCREEN_BACK);

    // ZBufferの使用
    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(true);

    // 各種初期化処理
    m_lightBufferH = CreateShaderConstantBuffer(sizeof(LightBuffer));
	assert(m_lightBufferH != -1);
	m_lightBuffer = static_cast<LightBuffer*>(GetBufferShaderConstantBuffer(m_lightBufferH));
	assert(m_lightBuffer != nullptr);
    InitManager();
    NumUtility::GetInstance().Init();
    Random::GetInstance().Init();
    SceneHelp::GetInstance().Init();

    return true;
}

void Application::Run()
{
    auto& scnMgr = SceneManager::GetInstance();
    auto& sndMgr = SoundManager::GetInstance();
    auto& effMgr = EffekseerManager::GetInstance();
    auto& physics = MyEngine::Physics::GetInstance();
    auto& input = Input::GetInstance();
#ifdef _DEBUG
    auto& debug = MyEngine::DebugDraw::GetInstance();
    LONGLONG updateTime;
    LONGLONG physicsTime;
    LONGLONG drawTime;
    bool isUpdate = true;
    bool isStep = false;
    int frame = 0;
    float fps = 0.0f;
#endif

    LONGLONG time;
	// メインループ
    while (ProcessMessage() != -1) 
    {
		// 時間計測
        time = GetNowHiPerformanceCount();

		// 画面クリア
        ClearDrawScreen();

		// 入力情報の更新
        input.Update();

#ifdef _DEBUG
//        debug.Gulid();
        
        if (isStep)
        {
            if (input.IsTriggerd(KEY_INPUT_P))
            {
                isUpdate = true;
            }
            if (input.IsTriggerd(KEY_INPUT_RETURN))
            {
                isStep = false;
            }
        }
        else
        {
            isUpdate = true;
            if (input.IsTriggerd(KEY_INPUT_P))
            {
                isStep = true;
            }
        }

        debug.SetStop(!isUpdate);

        if (isUpdate)
        {
            ++frame;
            printf("\n-------------------------------\n");
            ShowTime();
            printf("シーン:%s\n", SCENE_KIND.at(scnMgr.GetNowSceneKind()));
            printf("経過フレーム:%d\n", frame);
            debug.Clear();
            updateTime = GetNowHiPerformanceCount();
#endif
			// シーンの更新
            scnMgr.Update();
#ifdef _DEBUG
            physicsTime = GetNowHiPerformanceCount();
#endif
			// 物理更新
            physics.Update();
#ifdef _DEBUG
            physicsTime = GetNowHiPerformanceCount() - physicsTime;
#endif
			// サウンドの更新
            sndMgr.Update();
			// エフェクトの更新
            effMgr.Update();
#ifdef _DEBUG
            drawTime = GetNowHiPerformanceCount();
            updateTime = drawTime - updateTime - physicsTime;
        }
#endif
		// シーンの描画
        scnMgr.Draw();

#ifdef _DEBUG
#if true
//        debug.Draw();
        if (isStep) DrawString(16, 620, L"動作：ステップ中", 0x00ff00);
        else        DrawString(16, 620, L"動作：通常中", 0x00ff00);
        
        if (isUpdate)
        {
            drawTime = GetNowHiPerformanceCount() - drawTime;
            isUpdate = false;
            fps = GetFPS();
        }
        DrawFormatString(16, 636, 0x00ff00, L"FPS  : %.2f", fps);
        // 物理更新時間
        int pW = static_cast<int>(Game::WINDOW_W * (physicsTime / 16666.6f));
        DrawBox(72, 652, 72 + pW, 668, 0xff00ff, true);
        DrawFormatString(16, 652, 0x00ff00, L"物理 : %d", physicsTime);
        // 更新時間
        int uW = static_cast<int>(Game::WINDOW_W * (updateTime / 16666.6f));
        DrawBox(72, 668, 72 + uW, 684, 0xff0000, true);
        DrawFormatString(16, 668, 0x00ff00, L"更新 : %d", updateTime);
        // 描画時間
        int dW = static_cast<int>(Game::WINDOW_W * (drawTime / 16666.6f));
        DrawBox(72, 684, 72 + dW, 700, 0x0000ff, true);
        DrawFormatString(16, 684, 0x00ff00, L"描画 : %d", drawTime);
        // 合計時間
        DrawBox(72, 700, 72 + pW, 716, 0xff0000, true);
        DrawBox(72 + pW, 700, 72 + pW + uW, 716, 0xff0000, true);
        DrawBox(72 + pW + uW, 700, 72 + pW + uW + dW, 716, 0x0000ff, true);
        DrawFormatString(16, 700, 0x00ff00, L"合計 : %d", physicsTime + drawTime + updateTime);
#endif
#endif
        ScreenFlip();

        if (m_isEnd) break;
        while (Game::FPS_FRAME > GetNowHiPerformanceCount() - time);
    }

    
}

void Application::Terminate()
{
    NumUtility::GetInstance().End();
    SceneHelp::GetInstance().End();
    EndManager();
    DxLib_End();
#ifdef _DEBUG
    // コンソール画面終了処理
    fclose(m_out); 
    fclose(m_in); 
    FreeConsole();
#endif
}

void Application::SetLightBuffer(Light* ligths, int lightNum)
{
	m_lightBuffer->num = lightNum;
	for (int i = 0; i < lightNum; ++i)
	{
		m_lightBuffer->lights[i] = ligths[i];
	}
}

void Application::InitManager()
{
    auto& effMgr = EffekseerManager::GetInstance();
    auto& fileMgr = FileManager::GetInstance();
    auto& scnMgr = SceneManager::GetInstance();
    auto& fontMgr = FontManager::GetInstance();
    auto& stageDataMgr = StageDataManager::GetInstance();
    auto& saveDataMgr = SaveDataManager::GetInstance();
    auto& rankMgr = RankingDataManager::GetInstance();
    auto& novelMgr = NovelManager::GetInstance();

    effMgr.Init(8000);
    fileMgr.Init();
    fontMgr.Init();
    stageDataMgr.Load();
    saveDataMgr.Load();
    rankMgr.Load();
    novelMgr.Load();
    scnMgr.Init();
}

void Application::EndManager()
{
    auto& scnMgr = SceneManager::GetInstance();
    auto& saveDataMgr = SaveDataManager::GetInstance();
    auto& rankMgr = RankingDataManager::GetInstance();
    auto& effMgr = EffekseerManager::GetInstance();
    auto& fileMgr = FileManager::GetInstance();
    auto& fontMgr = FontManager::GetInstance();

    saveDataMgr.Save();
    rankMgr.Save();
    scnMgr.End();
    effMgr.End();
    fileMgr.End();
    fontMgr.End();
}

#ifdef _DEBUG
void Application::ShowTime() const
{
    // 現在時刻をsystem_clockを用いて取得
    auto now = std::chrono::system_clock::now();
    // 現在時刻をtime_t形式で出力
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    // 時間
    printf("%d時", static_cast<int>((t / 3600 + 9) % 24));
    //分
    printf("%d分", static_cast<int>(t / 60 % 60));
    //秒
    printf("%d秒\n", static_cast<int>(t % 60));
}
#endif