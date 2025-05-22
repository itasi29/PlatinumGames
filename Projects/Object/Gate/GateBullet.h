#pragma once
#include "Collidable.h"
#include <memory>

enum class GateKind;
class GateManager;

/// <summary>
/// ゲート弾クラス
/// </summary>
class GateBullet final : public MyEngine::Collidable
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="kind">ゲートの種類</param>
    GateBullet(const std::shared_ptr<GateManager>& gateMgr, GateKind kind);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GateBullet();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="dir">方向</param>
    void Init(const Vec3& pos, const Vec3& dir);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 弾が壊れたかどうかを取得する
    /// </summary>
    /// <returns>true:壊れた / false:壊れていない</returns>
    bool IsBreak() const { return m_isBreak; }
    /// <summary>
    /// ゲートの種類を取得する
    /// </summary>
    /// <returns>ゲートの種類</returns>
    const GateKind& GetKind() const { return m_kind; }

    /// <summary>
    /// 衝突時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
    /// <summary>
    /// トリガー侵入時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    // ゲートマネージャー
    std::shared_ptr<GateManager> m_gateMgr;  

    // ゲートの種類
    GateKind m_kind;  

    // エフェクトハンドル
    int m_effHandle;  

    // 弾が壊れたかどうか
    bool m_isBreak;  
};
