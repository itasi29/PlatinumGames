#pragma once
#include "ColliderBase.h"

namespace MyEngine
{
    /// <summary>
    /// スフィアコライダークラス
    /// </summary>
    class ColliderSphere : public ColliderBase
    {
    public:
        /// <summary>
        /// コンストラクタ
        /// </summary>
        ColliderSphere();

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="center">中心</param>
        /// <param name="radius">半径</param>
        /// <param name="isTrigger">トリガーかどうか</param>
        ColliderSphere(const Vec3& center, float radius, bool isTrigger);

    protected:
        /// <summary>
        /// 球との衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象のコライダー</param>
        /// <param name="colliderPos">対象の位置</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// カプセルとの衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象のコライダー</param>
        /// <param name="colliderPos">対象の位置</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// ボックスとの衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象のコライダー</param>
        /// <param name="colliderPos">対象の位置</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// 球との衝突修正
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="colliderCol">対象のコライダー</param>
        /// <param name="colliderPos">対象の位置</param>
        /// <param name="hitInfo">衝突情報</param>
        /// <returns>修正後の位置</returns>
        Vec3 FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo) override;

        /// <summary>
        /// カプセルとの衝突修正
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="colliderCol">対象のコライダー</param>
        /// <param name="colliderPos">対象の位置</param>
        /// <param name="hitInfo">衝突情報</param>
        /// <returns>修正後の位置</returns>
        Vec3 FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo) override;

    public:
        // 球の半径
        float radius;  
    };
}