#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>
#include <variant>
#include "Vec3.h"

class GateManager;
class StageManager;
class CameraManager;
class Player;
enum class PlayerState;

/// <summary>
/// チュートリアルの種類
/// </summary>
enum class TutorialKind
{
    STATE,  // プレイヤーステートでのチェック
    INPUT,  // 入力情報でのチェック
    CP,     // チェックポイントでのチェック
};

/// <summary>
/// プレイヤーステートでのチェック情報
/// </summary>
struct CheckStateInfo
{
    PlayerState state;
    int frame = 0;
};

/// <summary>
/// 入力情報でのチェック情報
/// </summary>
struct CheckInputInfo
{
    /// <summary>
    /// トリガーの種類
    /// </summary>
    enum class TriggerType
    {
        STICK_LEFT,
        STICK_RIGHT,
        TRIGGER,
    };
    bool isTrigger = false;
    TriggerType type = TriggerType::STICK_RIGHT;
    std::string command;
    int frame = 0;
};

/// <summary>
/// チェックポイントでのチェック情報
/// </summary>
struct CheckCPInfo
{
    int cp = -1;
};

// 共用体に
using CheckInfo = std::variant<CheckStateInfo, CheckInputInfo, CheckCPInfo>;

/// <summary>
/// チュートリアルで使う情報
/// </summary>
struct TutorialInfo
{
    TutorialKind kind = TutorialKind::STATE;
    CheckInfo info;
    bool isDrawClear = false;
    bool isDrawDestination = false;
    Vec3 destination;
    bool isDrawGoal = false;
    int goalTextId = -1;
    bool isDrawNovel = false;
    int novelId = -1;
};

/// <summary>
/// チュートリアルシーンクラス
/// </summary>
class SceneTutorial : public SceneBase
{
private:
    /// <summary>
    /// シェーダー用の定数バッファ
    /// </summary>
    struct CBuff
    {
        float frame;
        float fireRed;
        float fireGreen;
        float fireBlue;
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneTutorial();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneTutorial();

    /// <summary>
    /// 初期化処理(非同期)
    /// </summary>
    void AsyncInit() override;

    /// <summary>
    /// 初期化処理(同期)
    /// </summary>
    void Init() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理(同期)
    /// </summary>
    /// <param name="isFade">フェード中かどうか</param>
    void Update(bool isFade) override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const override;

    /// <summary>
    /// 再スタート
    /// </summary>
    void OnRestart();

private:
    /// <summary>
    /// チュートリアル情報の読み込み
    /// </summary>
    void Load();

    /// <summary>
    /// メインの更新処理
    /// </summary>
    void MainUpdate();

    /// <summary>
    /// 再スタート時の更新処理
    /// </summary>
    void RestartUpdate();

    /// <summary>
    /// プレイヤーステートのチェック更新処理
    /// </summary>
    void CheckStateUpdate();

    /// <summary>
    /// 入力情報のチェック更新処理
    /// </summary>
    void CheckInputUpdate();

    /// <summary>
    /// チェックポイントのチェック更新処理
    /// </summary>
    void CheckCPUpdate();

    /// <summary>
    /// チュートリアルクリア時の更新処理
    /// </summary>
    void ClearTutorialUpdate();

    /// <summary>
    /// チュートリアルクリア時の処理
    /// </summary>
    void OnClearTutorial();

    /// <summary>
    /// チュートリアルの変更
    /// </summary>
    void ChangeTutorial();

    /// <summary>
    /// メインの描画処理
    /// </summary>
    void DrawMain() const;

    /// <summary>
    /// 再スタート時の描画処理
    /// </summary>
    void DrawRestart() const;

    /// <summary>
    /// UIの描画処理
    /// </summary>
    void DrawUI() const;

    /// <summary>
    /// ゲートUIの描画処理
    /// </summary>
    void DrawGateUI() const;

    /// <summary>
    /// チュートリアルクリア時の描画処理
    /// </summary>
    void DrawTutorialClear() const;

    /// <summary>
    /// チュートリアルゴール時の描画処理
    /// </summary>
    void DrawTutorialGoal() const;

    /// <summary>
    /// チュートリアルノベルの描画処理
    /// </summary>
    void DrawTutorialNovel() const;

    /// <summary>
    /// チュートリアル目的地の描画処理
    /// </summary>
    void DrawTutorialDestination() const;

private:
    using UpdateFunc_t = void(SceneTutorial::*)();
    using DrawFunc_t = void(SceneTutorial::*)() const;
    UpdateFunc_t m_updateFunc;          // 更新関数ポインタ
    UpdateFunc_t m_tutorialUpdateFunc;  // チュートリアル更新関数ポインタ
    DrawFunc_t m_drawFunc;              // 描画関数ポインタ

    std::shared_ptr<GateManager> m_gateMgr;     // ゲートマネージャー
    std::shared_ptr<StageManager> m_stageMgr;   // ステージマネージャー
    std::shared_ptr<CameraManager> m_cameraMgr; // カメラマネージャー
    std::shared_ptr<Player> m_player;           // プレイヤー

    std::vector<int> m_rtTable; // レンダーターゲットのテーブル
    std::vector<TutorialInfo> m_tutorialInfo; // チュートリアル情報のリスト

    CBuff* m_cBuff; // シェーダー用の定数バッファ
    int m_cBuffH;   // 定数バッファのハンドル

    // 時間
    int m_time; 
    // インデックス
    int m_index; 
    int m_checkFrame; // チェックフレーム
    int m_stampFrame; // スタンプフレーム
    int m_uiFrame1; // UIフレーム1
    int m_uiFrame2; // UIフレーム2

    bool m_isDrawGoal;  // ゴール描画フラグ
    bool m_isDrawClear; // クリア描画フラグ
};