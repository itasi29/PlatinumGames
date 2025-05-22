#pragma once
#include <memory>
#include <list>
#include <unordered_map>

namespace MyEngine
{
    struct CollideHitInfo;
    class Collidable;
    class ColliderCapsule;
    struct PreHitInfo;
}
class Vec3;
class Gate;
class GateBullet;
class CameraManager;
class Player;
enum class ObjectTag;

enum class GateKind
{
    Orange,
    Blue,
    MAX
};

/// <summary>
/// ゲートマネージャークラス
/// </summary>
class GateManager
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="cameraMgr">カメラマネージャー</param>
    /// <param name="stageNo">ステージ番号</param>
    GateManager(const std::shared_ptr<CameraManager>& cameraMgr, const int stageNo);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GateManager();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="player">プレイヤー</param>
    void Init(const std::weak_ptr<Player>& player);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// ゲートを描画する
    /// </summary>
    /// <param name="texA">テクスチャA</param>
    /// <param name="texB">テクスチャB</param>
    void DrawGate(int texA, int texB) const;

    /// <summary>
    /// リスタート処理
    /// </summary>
    void Restart();

    /// <summary>
    /// 弾を追加する
    /// </summary>
    /// <param name="bullet">追加する弾</param>
    void AddBullet(std::shared_ptr<GateBullet> bullet);

    /// <summary>
    /// ゲートを作成する
    /// </summary>
    /// <param name="kind">ゲートの種類</param>
    /// <param name="collider">コライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <param name="norm">法線</param>
    /// <param name="dir">方向</param>
    void CreateGate(GateKind kind, MyEngine::Collidable* collider, const MyEngine::CollideHitInfo& hitInfo, const Vec3& norm, const Vec3& dir);

    /// <summary>
    /// ペアゲートを取得する
    /// </summary>
    /// <param name="kind">ゲートの種類</param>
    /// <returns>ペアゲート</returns>
    std::shared_ptr<Gate> GetPairGate(GateKind kind) const;
    /// <summary>
    /// 最後に撃ったゲートの種類を取得する
    /// </summary>
    /// <returns>最後に撃ったゲートの種類</returns>
    const GateKind& GetLastShotKind() const { return m_lastShotKind; }
    /// <summary>
    /// 両方のゲートが作成されたかどうかを取得する
    /// </summary>
    /// <returns>true:両方作成された / false:作成されていない</returns>
    bool IsCreateBothGates() const { return m_isCreate.at(GateKind::Blue) && m_isCreate.at(GateKind::Orange); }
    /// <summary>
    /// 指定した種類のゲートが作成されたかどうかを取得する
    /// </summary>
    /// <param name="kind">ゲートの種類</param>
    /// <returns>true:作成された / false:作成されていない</returns>
    bool IsCreate(GateKind kind) const { return m_isCreate.at(kind); }

private:
    /// <summary>
    /// 地形にゲートを作成する
    /// </summary>
    void CreateGateOnTerrain();

    /// <summary>
    /// ゲートを作成する
    /// </summary>
    /// <param name="gate">ゲート</param>
    /// <param name="kind">ゲートの種類</param>
    /// <param name="hitTag">ヒットタグ</param>
    /// <param name="pos">位置</param>
    /// <param name="norm">法線</param>
    /// <param name="dir">方向</param>
    void CreateGate(std::shared_ptr<Gate>& gate, GateKind kind, ObjectTag hitTag, const Vec3& pos, const Vec3& norm, const Vec3& dir);

private:
    // ステージ名
    const int m_stageNo;

    // カメラマネージャー
    std::shared_ptr<CameraManager> m_cameraMgr;  
    // オレンジゲート
    std::shared_ptr<Gate> m_gateOrange;  
    // ブルーゲート
    std::shared_ptr<Gate> m_gateBlue;  
    // プレイヤー
    std::weak_ptr<Player> m_player;  

    // 弾リスト
    std::list<std::shared_ptr<GateBullet>> m_bulletList;  

    // ゲート作成フラグ
    std::unordered_map<GateKind, bool> m_isCreate;  

    // 最後に撃ったゲートの種類
    GateKind m_lastShotKind;  
};
