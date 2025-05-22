#pragma once
#include "Object/Gimmick/GimmickSendObject.h"

namespace MyEngine
{
    struct CollideHitInfo;
}
class GimmickLinkObject;
class FileBase;

/// <summary>
/// レーザー起動装置クラス
/// </summary>
class LaserCatcher : public GimmickSendObject
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    LaserCatcher();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~LaserCatcher();

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
    virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
    // キャッチサウンドエフェクト
    std::shared_ptr<FileBase> m_catchSe;  
    // プレイ中のクリアエフェクトハンドル
	int m_playClearEffH;

    // クリアしたかどうか
    bool m_isClear;
};
