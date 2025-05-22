#pragma once
#include "Geometry/Vec3.h"
#include "Collider/ColliderBase.h"

// 衝突判定と修正を行うヘルパー
namespace MyEngine
{
    class ColliderSphere;
    class ColliderCapsule;
    class ColliderLine;
    class ColliderBox;
    class Rigidbody;

    /// <summary>
    /// 球と球の衝突判定を行う
    /// </summary>
    /// <param name="posA">球Aの位置</param>
    /// <param name="posB">球Bの位置</param>
    /// <param name="colliderA">球Aのコライダー</param>
    /// <param name="colliderB">球Bのコライダー</param>
    /// <returns>衝突情報</returns>
    CollideHitInfo IsCollideSphereAndSphere(const Vec3& posA, const Vec3& posB, ColliderSphere* colliderA, ColliderSphere* colliderB);

    /// <summary>
    /// 球とカプセルの衝突判定を行う
    /// </summary>
    /// <param name="posSphere">球の位置</param>
    /// <param name="posCapsule">カプセルの位置</param>
    /// <param name="colliderSphere">球のコライダー</param>
    /// <param name="colliderCapsule">カプセルのコライダー</param>
    /// <returns>衝突情報</returns>
    CollideHitInfo IsCollideSphereAndCapsule(const Vec3& posSphere, const Vec3& posCapsule, ColliderSphere* colliderSphere, ColliderCapsule* colliderCapsule);

    /// <summary>
    /// 球とボックスの衝突判定を行う
    /// </summary>
    /// <param name="posSphere">球の位置</param>
    /// <param name="posBox">ボックスの位置</param>
    /// <param name="colliderSphere">球のコライダー</param>
    /// <param name="colliderBox">ボックスのコライダー</param>
    /// <param name="rigidSphere">球のリジッドボディ</param>
    /// <param name="preInfo">事前衝突情報</param>
    /// <returns>衝突情報</returns>
    CollideHitInfo IsCollideSphereAndBox(const Vec3& posSphere, const Vec3& posBox, ColliderSphere* colliderSphere, ColliderBox* colliderBox, const Rigidbody& rigidSphere, const PreHitInfo& preInfo);

    /// <summary>
    /// カプセルとボックスの衝突判定を行う
    /// </summary>
    /// <param name="posCapsule">カプセルの位置</param>
    /// <param name="posBox">ボックスの位置</param>
    /// <param name="colliderCapsule">カプセルのコライダー</param>
    /// <param name="colliderBox">ボックスのコライダー</param>
    /// <param name="rigidCapsule">カプセルのリジッドボディ</param>
    /// <param name="preInfo">事前衝突情報</param>
    /// <returns>衝突情報</returns>
    CollideHitInfo IsCollideCapsuleAndBox(const Vec3& posCapsule, const Vec3& posBox, ColliderCapsule* colliderCapsule, ColliderBox* colliderBox, const Rigidbody& rigidCapsule, const PreHitInfo& preInfo);

    /// <summary>
    /// カプセルとカプセルの衝突判定を行う
    /// </summary>
    /// <param name="posA">カプセルAの位置</param>
    /// <param name="posB">カプセルBの位置</param>
    /// <param name="colliderA">カプセルAのコライダー</param>
    /// <param name="colliderB">カプセルBのコライダー</param>
    /// <returns>衝突情報</returns>
    CollideHitInfo IsCollideCapsuleAndCapsule(const Vec3& posA, const Vec3& posB, ColliderCapsule* colliderA, ColliderCapsule* colliderB);


    /// <summary>
    /// 球と球の衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主球の位置</param>
    /// <param name="secondaryPos">従球の位置</param>
    /// <param name="primaryCollider">主球のコライダー</param>
    /// <param name="secondaryCollider">従球のコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixSphereToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);

    /// <summary>
    /// カプセルとカプセルの衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主カプセルの位置</param>
    /// <param name="secondaryPos">従カプセルの位置</param>
    /// <param name="primaryCollider">主カプセルのコライダー</param>
    /// <param name="secondaryCollider">従カプセルのコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixCapsuleToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);

    /// <summary>
    /// 球とカプセルの衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主球の位置</param>
    /// <param name="secondaryPos">従カプセルの位置</param>
    /// <param name="primaryCollider">主球のコライダー</param>
    /// <param name="secondaryCollider">従カプセルのコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixSphereToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);

    /// <summary>
    /// カプセルと球の衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主カプセルの位置</param>
    /// <param name="secondaryPos">従球の位置</param>
    /// <param name="primaryCollider">主カプセルのコライダー</param>
    /// <param name="secondaryCollider">従球のコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixCapsuleToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);

    /// <summary>
    /// ボックスと球の衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主ボックスの位置</param>
    /// <param name="secondaryPos">従球の位置</param>
    /// <param name="primaryCollider">主ボックスのコライダー</param>
    /// <param name="secondaryCollider">従球のコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixBoxToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo);

    /// <summary>
    /// ボックスとカプセルの衝突修正を行う
    /// </summary>
    /// <param name="primaryPos">主ボックスの位置</param>
    /// <param name="secondaryPos">従カプセルの位置</param>
    /// <param name="primaryCollider">主ボックスのコライダー</param>
    /// <param name="secondaryCollider">従カプセルのコライダー</param>
    /// <param name="hitInfo">衝突情報</param>
    /// <returns>修正後の位置</returns>
    Vec3 FixBoxToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo);
}