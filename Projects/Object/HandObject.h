#pragma once
#include "Object/Object3DBase.h"
#include <memory>
#include <unordered_map>

class Player;
class GateManager;
class Gate;

/// <summary>
/// ハンドオブジェクトクラス
/// </summary>
class HandObject : public Object3DBase
{
private:
    // MEMO: もしfriendを使わずに済むならそちらの処理に変更する
    friend Player;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    HandObject(GateManager& gateMgr);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~HandObject();

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
    /// リスタート処理
    /// </summary>
    void Restart() override;

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

    /// <summary>
    /// トリガー中の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

    /// <summary>
    /// トリガー退出時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    /// <summary>
    /// モデル情報を適用する
    /// </summary>
    void AppModelInfo() const override;

    /// <summary>
    /// ハンド処理
    /// </summary>
    void OnHand();

    /// <summary>
    /// ハンド終了処理
    /// </summary>
    void EndHand();

    /// <summary>
    /// 位置を設定する
    /// </summary>
    /// <param name="pos">位置</param>
    void SetPos(const Vec3& pos);

    /// <summary>
    /// プレイヤーワープ処理
    /// </summary>
    void OnPlayerWarp();

private:
    // ゲートマネージャー
    GateManager& m_gateMgr;  
    // ゲート
    Gate* m_gate;  

    // 初期位置
    Vec3 m_initPos;  
    // 移動前の位置
    Vec3 m_prePos;  
    // ワープ時の位置
    Vec3 m_warpPos;  

    // 判定サイズ
    float m_colSize;  

    // 持っているフラグ
    bool m_isHold;  
    // ワープフラグ
    bool m_isWarp;  
    // ワープ可能フラグ
    bool m_isCanWarp;  
    // タグ追加フラグ
    std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;  
};
