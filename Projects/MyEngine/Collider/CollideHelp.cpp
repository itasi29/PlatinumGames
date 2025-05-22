#include "CollideHelp.h"
#include <cassert>
#include "Game.h"
#include "Geometry/MathHelp.h"
#include "Rigidbody.h"
#include "Collidable.h"
#include "Colliders.h"
#include "DebugDraw.h"

namespace
{
    constexpr float WALL_POLYGON_RADAR = 0.4f;
}

using namespace MyEngine;

/// <summary>
/// 球と球の判定
/// </summary>
/// <param name="pt1">座標1</param>
/// <param name="pt2">座標2</param>
/// <param name="radius1">半径1</param>
/// <param name="radius2">半径2</param>
/// <returns>判定情報</returns>
CollideHitInfo IsCollideSphereAndSphere(const Vec3& pt1, const Vec3& pt2, float radius1, float radius2)
{
    auto pt1Topt2 = (pt2 - pt1);
    auto sqLen = pt1Topt2.SqLength();
    auto sumRadius = radius1 + radius2;
    CollideHitInfo info;
    if (sqLen < sumRadius * sumRadius)
    {
        info.isHit = true;
        float len = pt1Topt2.Length();
        info.hitPos = pt1 + pt1Topt2.GetNormalized() * len * (radius1 / sumRadius);
    }
    return info;
}

/// <summary>
/// 平行判定
/// </summary>
/// <param name="axis">軸</param>
/// <param name="pre">判定1つ目</param>
/// <param name="now">判定2つ目</param>
/// <returns></returns>
bool IsPreParallelAndNowNotParallel(const Vec3& axis, const Vec3& pre, const Vec3& now)
{
    float dot = Vec3::Dot(pre, axis);
    return (-0.9f < dot && dot < 0.9f);
    // 一つ前が平行でないならfalse
    if (-0.9f < dot && dot < 0.9f) return false;

    dot = Vec3::Dot(now, axis);
    // 今回が平行ならばfalse、平行でなければtrue
    return -1.0f < dot && dot < 1.0f;
}

/// <summary>
/// ステート確認
/// </summary>
/// <param name="info">情報</param>
/// <param name="preInfo">前回情報</param>
void CheckState(CollideHitInfo& info, const PreHitInfo& preInfo, const Rigidbody& rigid)
{
    uint8_t state = 0;
    if (preInfo.isCreate)
    {
        const auto preState = preInfo.info.state;
        state = preState & 0b00000111;
    }
    const auto& velDir = rigid.GetDir();
    // 壁判定
    if (-WALL_POLYGON_RADAR < info.fixDir.y && info.fixDir.y < WALL_POLYGON_RADAR)
    {
        if ((state & CollideHitInfo::STATE_WALL_FLAG) == 0)
        {
            state += CollideHitInfo::STATE_FIRST_FLAG;
        }
        state += CollideHitInfo::STATE_NOW_WALL_FLAG;
        state = state | CollideHitInfo::STATE_WALL_FLAG;
    }
    // 天井判定
    else if (velDir.y > 0)
    {
        if ((state & CollideHitInfo::STATE_ROOF_FLAG) == 0)
        {
            state += CollideHitInfo::STATE_FIRST_FLAG;
        }
        state += CollideHitInfo::STATE_NOW_ROOF_FLAG;
        state = state | CollideHitInfo::STATE_ROOF_FLAG;
        info.fixDir = Vec3::Down();
    }
    // 地面判定
    else
    {
        if ((state & CollideHitInfo::STATE_FLOOR_FLAG) == 0)
        {
            state += CollideHitInfo::STATE_FIRST_FLAG;
        }
        state += CollideHitInfo::STATE_NOW_FLOOR_FLAG;
        state = state | CollideHitInfo::STATE_FLOOR_FLAG;
        info.fixDir = Vec3::Up();
    }

    info.state = state;
}

CollideHitInfo MyEngine::IsCollideSphereAndSphere(const Vec3& posA, const Vec3& posB, ColliderSphere* colliderA, ColliderSphere* colliderB)
{
    auto sphereAPos = posA + colliderA->center;
    auto sphereBPos = posB + colliderB->center;

    return IsCollideSphereAndSphere(sphereAPos, sphereBPos, colliderA->radius, colliderB->radius);
}

CollideHitInfo MyEngine::IsCollideSphereAndCapsule(const Vec3& posSphere, const Vec3& posCapsule, ColliderSphere* colliderSphere, ColliderCapsule* colliderCapsule)
{
    auto spherePos = posSphere + colliderSphere->center;
    auto capsulePos = posCapsule + colliderCapsule->center;

    auto halfDir = colliderCapsule->dir * colliderCapsule->size * 0.5f;
    auto capsuleStart = capsulePos + halfDir;
    auto capsuleEnd = capsulePos - halfDir;

    auto nearPt = GetNearestPtOnLine(spherePos, capsuleStart, capsuleEnd);

    return IsCollideSphereAndSphere(spherePos, nearPt, colliderCapsule->radius, colliderSphere->radius);
}

CollideHitInfo MyEngine::IsCollideSphereAndBox(const Vec3& posSphere, const Vec3& posBox, ColliderSphere* colliderSphere, ColliderBox* colliderBox, const Rigidbody& rigidSphere, const PreHitInfo& preInfo)
{
    auto spherePos = posSphere + colliderSphere->center;
    auto boxPos = posBox + colliderBox->center;

    // 球に近い場所を求める
    bool isIn;
    auto nearPos = GetNearestPtOnBox(spherePos, boxPos, colliderBox->size, colliderBox->rotation, isIn);
    // 球が中にある場合
    if (isIn)
    {
        CollideHitInfo info;
        info.isHit = true;
        info.hitPos = nearPos;
        info.fixDir = (nearPos - spherePos).GetNormalized();

        return info;
    }
    // 違う場合、最近接点と球の中心との長さで判定
    auto radius = colliderSphere->radius;
    auto nearToSphere = spherePos - nearPos;
    if (nearToSphere.SqLength() < radius * radius)
    {
        CollideHitInfo info;
        info.isHit = true;
        info.hitPos = nearPos;
        info.fixDir = nearToSphere.GetNormalized();
        CheckState(info, preInfo, rigidSphere);

        return info;
    }
    else
    {
        return CollideHitInfo{ false };
    }
}

CollideHitInfo MyEngine::IsCollideCapsuleAndBox(const Vec3& posCapsule, const Vec3& posBox, ColliderCapsule* colliderCapsule, ColliderBox* colliderBox, const Rigidbody& rigidCapsule, const PreHitInfo& preInfo)
{
    auto capsulePos = posCapsule + colliderCapsule->center;
    auto boxPos = posBox + colliderBox->center;

    auto dirH = colliderCapsule->dir * colliderCapsule->size * 0.5f;
    auto cS = capsulePos + dirH;
    auto cE = capsulePos - dirH;

    auto cNearPos = GetNearestPtOnLine(boxPos, cS, cE);

    CollideHitInfo info;
    bool isIn;
    auto bNearPos = GetNearestPtOnBox(cNearPos, boxPos, colliderBox->size, colliderBox->rotation, isIn);
    // ボックス内に近接点がなければ
    if (!isIn)
    {
        // 最近接点と球の中心との長さで判定
        auto radius = colliderCapsule->radius;
        auto bNearToCNear = cNearPos - bNearPos;
        if (bNearToCNear.SqLength() > radius * radius) return CollideHitInfo{ false };
    }
    info.isHit = true;
    info.hitPos = bNearPos;
    info.fixDir = cNearPos - bNearPos;
    info.fixDir.Normalize();
    
    if (preInfo.isCheckPallel)
    {
        if (IsPreParallelAndNowNotParallel(Vec3::Right(), preInfo.info.fixDir, info.fixDir)) return CollideHitInfo{ false };
        if (IsPreParallelAndNowNotParallel(Vec3::Up(),    preInfo.info.fixDir, info.fixDir)) return CollideHitInfo{ false };
        if (IsPreParallelAndNowNotParallel(Vec3::Front(), preInfo.info.fixDir, info.fixDir)) return CollideHitInfo{ false };
    }

    CheckState(info, preInfo, rigidCapsule);

    return info;
}

CollideHitInfo MyEngine::IsCollideCapsuleAndCapsule(const Vec3& posA, const Vec3& posB, ColliderCapsule* colliderA, ColliderCapsule* colliderB)
{
    auto capsuleAPos = posA + colliderA->center;
    auto capsuleBPos = posB + colliderB->center;

    // 自身の向いている方向に伸びているベクトルを作成
    Vec3 dirA = colliderA->dir.GetNormalized() * colliderA->size * 0.5f;
    // 対象の向いている方向に伸びているベクトルを作成
    Vec3 dirB = colliderB->dir.GetNormalized() * colliderB->size * 0.5f;
    // 結果保持
    Vec3 minPosA, minPosB;
    GetNearestPtOnLine(capsuleAPos, dirA, capsuleBPos, dirB, minPosA, minPosB);

    return IsCollideSphereAndSphere(minPosA, minPosB, colliderA->radius, colliderB->radius);
}

Vec3 MyEngine::FixSphereToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo)
{
    auto priPos = primaryPos + primaryCollider->center;
    auto secPos = secondaryPos + secondaryCollider->center;

    // primaryからsecondaryへのベクトルを作成
    auto primaryToSecondary = secPos - priPos;
    // そのままだとちょうど当たる位置になるので少し余分に離す
    float  awayDist = primaryCollider->radius + secondaryCollider->radius + 0.0001f;
    // 長さを調整
    primaryToSecondary = primaryToSecondary.GetNormalized() * awayDist;

    // primaryからベクトル方向に押す
    return priPos + primaryToSecondary - secondaryCollider->center;
}

Vec3 MyEngine::FixCapsuleToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo)
{
    auto priPos = primaryPos + primaryCollider->center;
    auto secPos = secondaryPos + secondaryCollider->center;

    // 自身の向いている方向に伸びているベクトルを作成
    Vec3 dirPrimary = primaryCollider->dir.GetNormalized() * primaryCollider->size * 0.5f;
    // 対象の向いている方向に伸びているベクトルを作成
    Vec3 dirSecondary = secondaryCollider->dir.GetNormalized() * secondaryCollider->size * 0.5f;
    // 結果保持
    Vec3 minPosPrimary, minPosSecondary;
    // 線分同士の最近接点を取得する
    GetNearestPtOnLine(priPos, dirPrimary, secPos, dirSecondary, minPosPrimary, minPosSecondary);

    // Primaryの最近接点からSecondaryの最近接点までのベクトル
    auto pToS = minPosSecondary - minPosPrimary;
    // 長さを取得
    auto len = pToS.Length();
    // それぞれの半径＋極小値から長さを引いたぶんだけずらす
    float awayDis = primaryCollider->radius + secondaryCollider->radius + 0.0001f - len;

    return secPos - secondaryCollider->center + pToS.GetNormalized() * awayDis;
}

Vec3 MyEngine::FixSphereToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderSphere* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo)
{
    auto spherePos = primaryPos + primaryCollider->center;
    auto capsulePos = secondaryPos + secondaryCollider->center;

    auto dirH = secondaryCollider->dir * secondaryCollider->size * 0.5f;
    auto cS = capsulePos + dirH;
    auto cE = capsulePos - dirH;

    auto cNearPos = GetNearestPtOnLine(spherePos, cS, cE);

    auto dir = cNearPos - spherePos;
    auto fixLen = (primaryCollider->radius + secondaryCollider->radius) - dir.Length() + 0.0001f;

    return capsulePos + dir.GetNormalized() * fixLen - secondaryCollider->center;
}

Vec3 MyEngine::FixCapsuleToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderCapsule* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo)
{
    auto capsulePos = primaryPos + primaryCollider->center;
    auto spherePos = secondaryPos + secondaryCollider->center;

    auto dirH = primaryCollider->dir * primaryCollider->size * 0.5f;
    auto cS = capsulePos + dirH;
    auto cE = capsulePos - dirH;

    auto cNearPos = GetNearestPtOnLine(spherePos, cS, cE);

    auto dir = spherePos - cNearPos;
    dir.Normalize();

    return cNearPos + dir * (primaryCollider->radius + secondaryCollider->radius + 0.0001f) - secondaryCollider->center;
}

Vec3 MyEngine::FixBoxToSphere(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderSphere* secondaryCollider, const CollideHitInfo& hitInfo)
{
    Vec3 fixPos;
    if (hitInfo.state & CollideHitInfo::STATE_FIRST_FLAG)
    {
        fixPos = hitInfo.hitPos + hitInfo.fixDir * (secondaryCollider->radius + 0.0001f) - secondaryCollider->center;
    }
    else
    {
        fixPos = secondaryPos + hitInfo.fixDir * 0.05f;
    }
    return fixPos;
}

Vec3 MyEngine::FixBoxToCapsule(const Vec3& primaryPos, const Vec3& secondaryPos, ColliderBox* primaryCollider, ColliderCapsule* secondaryCollider, const CollideHitInfo& hitInfo)
{
    Vec3 fixPos;
    if (hitInfo.state & CollideHitInfo::STATE_FIRST_FLAG)
    {
        auto capsulePos = secondaryPos + secondaryCollider->center;

        auto dirH = secondaryCollider->dir * secondaryCollider->size * 0.5f;
        auto cS = capsulePos + dirH;
        auto cE = capsulePos - dirH;

        auto cNearPos = GetNearestPtOnLine(hitInfo.hitPos, cS, cE);

        auto dir = cNearPos - hitInfo.hitPos;
        auto fixLen = secondaryCollider->radius - dir.Length();

        fixPos = capsulePos + hitInfo.fixDir * (fixLen + 0.0001f) - secondaryCollider->center;
    }
    else
    {
        fixPos = secondaryPos + hitInfo.fixDir * 0.05f;
    }
    return fixPos;
}
