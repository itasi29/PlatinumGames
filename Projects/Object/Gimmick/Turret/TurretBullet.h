#pragma once
#include "Object/Object3DBase.h"

namespace MyEngine
{
    struct CollideHitInfo;
}
class Player;

/// <summary>
/// タレット弾クラス
/// </summary>
class TurretBullet : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    TurretBullet();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~TurretBullet();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="dir">方向</param>
    /// <param name="player">プレイヤー</param>
    void Init(const Vec3& pos, const Vec3& dir, Player* player);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 存在するかどうかを取得する
    /// </summary>
    /// <returns>true:存在する / false:存在しない</returns>
    bool IsExist() const { return m_isExist; }

    /// <summary>
    /// トリガー侵入時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    // プレイヤー
    Player* m_player;  

    // エフェクトハンドル
    int m_playEffH;  

    // 存在フラグ
    bool m_isExist;  
};
