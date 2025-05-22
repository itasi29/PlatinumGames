#pragma once
#include "Object/Object3DBase.h"

class GateManager;

/// <summary>
/// ゲート削除クラス
/// </summary>
class GateDelete : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    GateDelete(GateManager& gateMgr);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GateDelete();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="scale">スケール</param>
    /// <param name="rot">回転</param>
    /// <param name="list">コライダーリスト</param>
    /// <param name="isGravity">重力を使用するかどうか</param>
    void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity) override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// トリガー侵入時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    /// <summary>
    /// エフェクトを再生する
    /// </summary>
    /// <param name="handle">エフェクトハンドル</param>
    void PlayEff(int& handle);

private:
    // ゲートマネージャー
    GateManager& m_gateMgr;  

    // エフェクトハンドル1
    int m_effH1;  
    // エフェクトハンドル2
    int m_effH2;  
    // 遅延フレーム
    int m_delayFrame;  

    // 遅延フラグ
    bool m_isDelay;  
};
