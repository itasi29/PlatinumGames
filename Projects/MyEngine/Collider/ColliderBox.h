#pragma once
#include "ColliderBase.h"
#include "Geometry/Vec3.h"
#include "Geometry/Quaternion.h"

namespace MyEngine
{
    /// <summary>
    /// ボックスコライダークラス
    /// </summary>
    class ColliderBox : public ColliderBase
    {
    public:
        /// <summary>
        /// コンストラクタ
        /// </summary>
        ColliderBox();

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="center">中心</param>
		/// <param name="size">サイズ</param>
		/// <param name="rot">回転</param>
		/// <param name="norm">法線</param>
		/// <param name="isTrigger">トリガーかどうか</param>
		ColliderBox(const Vec3& center, const Vec3& size, const Quaternion& rot, const Vec3& norm, bool isTrigger);

        /// <summary>
        /// 球との衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">球のコライダー</param>
        /// <param name="colliderPos">球の位置</param>
        /// <param name="colliderRigid">球のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// カプセルとの衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">カプセルのコライダー</param>
        /// <param name="colliderPos">カプセルの位置</param>
        /// <param name="colliderRigid">カプセルのリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// 球との衝突修正
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="colliderCol">球のコライダー</param>
        /// <param name="colliderPos">球の位置</param>
        /// <param name="hitInfo">衝突情報</param>
        /// <returns>修正後の位置</returns>
        Vec3 FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo) override;

        /// <summary>
        /// カプセルとの衝突修正
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="colliderCol">カプセルのコライダー</param>
        /// <param name="colliderPos">カプセルの位置</param>
        /// <param name="hitInfo">衝突情報</param>
        /// <returns>修正後の位置</returns>
        Vec3 FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo) override;

    public:
        // サイズ
        Vec3 size;            
        // 回転
        Quaternion rotation;  
        // 法線
        Vec3 norm;            
    };
}