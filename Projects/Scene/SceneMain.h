#pragma once
#include "SceneBase.h"
#include <vector>
#include <list>
#include <memory>

class GateManager;
class StageManager;
class CameraManager;
class Player;
class UIMoveData;

/// <summary>
/// メインシーンクラス
/// </summary>
class SceneMain final : public SceneBase
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
    /// <param name="stageNo">ステージ番号</param>
    SceneMain(const int stageNo);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneMain() {}

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
    /// メインの更新処理
    /// </summary>
    void MainUpdate();

    /// <summary>
    /// ゲームオーバー時の更新処理
    /// </summary>
    void GameOverUpdate();

    /// <summary>
    /// 再スタート時の更新処理
    /// </summary>
    void RestartUpdate();

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
    /// 共通UIの描画処理
    /// </summary>
    void DrawCommonUI() const;

    /// <summary>
    /// 存在するUIの描画処理
    /// </summary>
    void DrawExistUI() const;

    /// <summary>
    /// 死亡時のUIの描画処理
    /// </summary>
    void DrawDeathUI() const;

private:
    using UpdateFunc_t = void(SceneMain::*)();
    using DrawFunc_t = void(SceneMain::*)() const;
    UpdateFunc_t m_updateFunc; // 更新関数ポインタ
    DrawFunc_t m_drawFunc;     // 描画関数ポインタ

    std::shared_ptr<GateManager> m_gateMgr;     // ゲートマネージャー
    std::shared_ptr<StageManager> m_stageMgr;   // ステージマネージャー
    std::shared_ptr<CameraManager> m_cameraMgr; // カメラマネージャー
    std::shared_ptr<Player> m_player;           // プレイヤー

    /* UI */
    std::shared_ptr<UIMoveData> m_deathUIWindow;            // 死亡時のUIウィンドウ
    std::vector<std::shared_ptr<UIMoveData>> m_deathUIList; // 死亡時のUIリスト

    // レンダーターゲットのテーブル
    std::vector<int> m_rtTable; 
    // ステージ番号
    const int m_stageNo;
    // シェーダー用の定数バッファ
    CBuff* m_cBuff; 

    int m_cBuffH;   // 定数バッファのハンドル
    int m_time;     // 時間
    int m_current;  // 現在の状態
    int m_count;    // カウント
};