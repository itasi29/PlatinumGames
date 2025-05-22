#pragma once
#include "Object/Object3DBase.h"

namespace MyEngine
{
    struct CollideHitInfo;
}
class GateManager;
class LaserLaunchPad;

/// <summary>
/// レーザー弾クラス
/// </summary>
class LaserBullet : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="launchPad">レーザー発射台</param>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="reflectNum">反射回数</param>
    LaserBullet(LaserLaunchPad* launchPad, const GateManager* gateMgr, const int reflectNum);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~LaserBullet();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="dir">方向</param>
    void Init(const Vec3& pos, const Vec3& dir);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 衝突時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

    /// <summary>
    /// 衝突中の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
    /// <summary>
    /// トリガー侵入時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
    /// <summary>
    /// トリガー中の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
    /// <summary>
    /// トリガー退出時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    // ゲートマネージャー
    const GateManager* m_gateMgr;  
    // 最大反射回数
    const int m_maxReflectNum;  

    // レーザー発射台
    LaserLaunchPad* m_launchPad;  

    // エフェクトハンドル
    int m_effHandle;  
    // 反射回数
    int m_reflectNum;  

    // ワープ可能かどうか
    bool m_isCanWarp;  
    // ワープ中かどうか
    bool m_isWarp;  
    // 前回ワープ中かどうか
    bool m_isPreWarp;  
    // タグ追加フラグ
    std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;  
};
