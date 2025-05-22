#pragma once
#include "ColliderBase.h"
#include "Vec3.h"

namespace MyEngine
{
    /// <summary>
    /// カプセルコライダークラス
    /// </summary>
    class ColliderCapsule : public ColliderBase
    {
    public:
        /// <summary>
        /// コンストラクタ
        /// </summary>
        ColliderCapsule();
        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="center">中心</param>
        /// <param name="dir">方向</param>
        /// <param name="radius">半径</param>
        /// <param name="size">サイズ</param>
        /// <param name="isTrigger">トリガーかどうか</param>
        ColliderCapsule(const Vec3& center, const Vec3& dir, float radius, float size, bool isTrigger);

        /// <summary>
        /// カプセルコライダーの初期化
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="dir">方向ベクトル</param>
        /// <param name="radius">半径</param>
        /// <param name="size">サイズ</param>
        /// <param name="isTrigger">トリガーかどうか</param>
        void Init(const Vec3& center, const Vec3& dir, float radius, float size, bool isTrigger);

    protected:
        /// <summary>
        /// 球との衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">球のコライダー</param>
        /// <param name="colliderPos">球の位置</param>
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
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

        /// <summary>
        /// ボックスとの衝突判定
        /// </summary>
        /// <param name="pos">自身の位置</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">ボックスのコライダー</param>
        /// <param name="colliderPos">ボックスの位置</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>衝突情報</returns>
        virtual CollideHitInfo IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo) override;

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
        // カプセルの方向ベクトル
        Vec3 dir;     
        // カプセルの半径
        float radius; 
        // カプセルのサイズ
        float size;   
    };
}