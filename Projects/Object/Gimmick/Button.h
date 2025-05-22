#pragma once
#include "GimmickSendObject.h"
#include "StateMachine.h"

namespace MyEngine
{
    struct CollideHitInfo;
}
class GimmickLinkObject;
class FileBase;

/// <summary>
/// ボタンクラス
/// </summary>
class Button : public GimmickSendObject
{
private:
    /// <summary>
    /// ボタンの状態を表す列挙型
    /// </summary>
    enum class State
    {
        DOWN,  // 押された状態
        UP     // 離された状態
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Button();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~Button();

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
    /// 衝突中の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
    /// <summary>
    /// 衝突終了時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnCollideExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    /// <summary>
    /// 押された状態の更新処理
    /// </summary>
    void UpdateDown();
    /// <summary>
    /// 離された状態の更新処理
    /// </summary>
    void UpdateUp();

    /// <summary>
    /// 押された状態に遷移する
    /// </summary>
    void OnDown();
    /// <summary>
    /// 離された状態に遷移する
    /// </summary>
    void OnUp();

private:
    // ステートマシン
    StateMachine<State> m_stateMachine;  

    // 押された時のサウンドエフェクト
    std::shared_ptr<FileBase> m_pushSe;  
    // 異なる状態のサウンドエフェクト
    std::shared_ptr<FileBase> m_difPs;  
    // 通常状態のサウンドエフェクト
    std::shared_ptr<FileBase> m_norPs;  

    // 初期位置
    Vec3 m_startPos;  

    // カウント
    int m_count;  

    // 離れたかどうか
    bool m_isRelease;  
    // 下がっているかどうか
    bool m_isDown;  
    // 上がっているかどうか
    bool m_isUp;  
};
