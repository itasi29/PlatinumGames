#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "CameraManager.h"

class GateManager;
class StageManager;
class Player;
enum class CameraKind;

/// <summary>
/// シーンのヘルパークラス
/// </summary>
class SceneHelp
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneHelp();

    /// <summary>
    /// コピーコンストラクタを削除
    /// </summary>
    SceneHelp(const SceneHelp& mgr) = delete;

    /// <summary>
    /// 代入演算子を削除
    /// </summary>
    void operator= (const SceneHelp& mgr) = delete;

public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~SceneHelp();

    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>SceneHelpのインスタンス</returns>
    static SceneHelp& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// モデルの描画
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="cameraMgr">カメラマネージャー</param>
    /// <param name="stageMgr">ステージマネージャー</param>
    /// <param name="player">プレイヤー</param>
    /// <param name="drawRt">描画ターゲット</param>
    /// <param name="mainCamera">メインカメラの種類</param>
    /// <param name="func">追加の描画関数</param>
    void DrawModel(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int drawRt = -1, CameraKind mainCamera = CameraKind::PLAYER, std::function<void()> func = nullptr) const;

private:
    /// <summary>
    /// ゲートのブレンド描画
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="cameraMgr">カメラマネージャー</param>
    /// <param name="stageMgr">ステージマネージャー</param>
    /// <param name="player">プレイヤー</param>
    /// <param name="rt">描画ターゲット</param>
    /// <param name="gate">ゲートカメラの種類</param>
    /// <param name="gateFrom">ゲートからのカメラの種類</param>
    /// <param name="func">追加の描画関数</param>
    void DrawGateBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, CameraKind gate, CameraKind gateFrom, std::function<void()> func) const;

    /// <summary>
    /// モデルのブレンド描画
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="cameraMgr">カメラマネージャー</param>
    /// <param name="stageMgr">ステージマネージャー</param>
    /// <param name="player">プレイヤー</param>
    /// <param name="rt">描画ターゲット</param>
    /// <param name="tex1">テクスチャ1</param>
    /// <param name="tex2">テクスチャ2</param>
    /// <param name="kind">カメラの種類</param>
    /// <param name="isPlayerDraw">プレイヤーを描画するかどうか</param>
    /// <param name="func">追加の描画関数</param>
    void DrawModelBlend(const std::shared_ptr<GateManager>& gateMgr, const std::shared_ptr<CameraManager>& cameraMgr, const std::shared_ptr<StageManager>& stageMgr, const std::shared_ptr<Player>& player, int rt, int tex1, int tex2, CameraKind kind, bool isPlayerDraw, bool isEffDraw, std::function<void()> func) const;

private:
    // レンダーターゲットのテーブル
    std::vector<int> m_rtTable; 
};