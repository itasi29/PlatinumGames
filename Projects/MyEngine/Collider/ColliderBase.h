#pragma once
#include <unordered_map>
#include <memory>
#include "Geometry/Vec3.h"

namespace MyEngine
{
    class Collidable;
	class Rigidbody;

    /// <summary>
    /// 衝突判定情報を格納する構造体
    /// </summary>
    struct CollideHitInfo
    {
        bool isHit = false;  // 衝突したかどうか
        Vec3 hitPos;         // 衝突位置
        Vec3 fixDir;         // 修正方向
        // ビット判定
        uint8_t state = 0;
        static const uint8_t STATE_FIRST_FLAG = 0b01000000;     // 今回が初かの判定
        static const uint8_t STATE_NOW_WALL_FLAG = 0b00100000;  // 今回が壁の場合　:1
        static const uint8_t STATE_NOW_ROOF_FLAG = 0b00010000;  // 今回が天所の場合:1
        static const uint8_t STATE_NOW_FLOOR_FLAG = 0b00001000; // 今回が地面の場合:1
        static const uint8_t STATE_WALL_FLAG = 0b00000100;      // 壁ヒットフラグ
        static const uint8_t STATE_ROOF_FLAG = 0b00000010;      // 天井ヒットフラグ
        static const uint8_t STATE_FLOOR_FLAG = 0b00000001;     // 地面ヒットフラグ
    };

    /// <summary>
    /// 事前衝突情報を格納する構造体
    /// </summary>
    struct PreHitInfo
    {
        CollideHitInfo info;         // 衝突情報
        bool isCreate = false;       // 作成されたかどうか
        bool isCheckPallel = false;  // 平行チェックを行うかどうか
    };

    // 当たり判定種別
    enum class ColKind : int
    {
        SPHERE,
        CAPSULE,
        BOX,
    };

    /// <summary>
    /// コライダーベースクラス
    /// </summary>
    class ColliderBase abstract
    {
    public:
        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="kind">当たり判定種別</param>
        ColliderBase(ColKind kind);
        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="kind">当たり判定種別</param>
        /// <param name="center">中心</param>
        /// <param name="isTrigger">トリガーかどうか</param>
        ColliderBase(ColKind kind, const Vec3& center, bool isTrigger);
        virtual ~ColliderBase() {}

        /// <summary>
        /// 当たり判定種別取得
        /// </summary>
        /// <returns>当たり判定種別</returns>
        ColKind GetKind() const { return m_kind; }

        /// <summary>
        /// 判定
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>判定結果(当たったか, 当たった座標)</returns>
        CollideHitInfo IsCollide(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo);

        /// <summary>
        /// 座標修正
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="hitInfo">判定結果情報</param>
        /// <returns>修正後の座標</returns>
        const Vec3& FixNextPos(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo);

    protected:
        /// <summary>
        /// 自身と球体との判定
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>判定結果</returns>
        virtual CollideHitInfo IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo);

        /// <summary>
        /// 自身とカプセルとの判定
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>判定結果</returns>
        virtual CollideHitInfo IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo);

        /// <summary>
        /// 自身とボックスとの判定
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="rigid">自身のリジッドボディ</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="colliderRigid">対象のリジッドボディ</param>
        /// <param name="preInfo">事前衝突情報</param>
        /// <returns>判定結果</returns>
        virtual CollideHitInfo IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo);

        /// <summary>
        /// 球体を押して修正する
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="hitInfo">判定結果情報</param>
        /// <returns>修正後の座標</returns>
        virtual Vec3 FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo);

        /// <summary>
        /// カプセルを押して修正する
        /// </summary>
        /// <param name="pos">自身の座標</param>
        /// <param name="colliderCol">対象の判定情報</param>
        /// <param name="colliderPos">対象の座標</param>
        /// <param name="hitInfo">判定結果情報</param>
        /// <returns>修正後の座標</returns>
        virtual Vec3 FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo);

    public:
        // MEMO: 色々なところで使うためColliderBase関係のpublic変数はメンバ変数のコーディング規約無視している
        
        // 判定チェックのみを行うか(true:座標修正を行わない/ false:座標修正を行う)
        bool isTrigger;  
        // 判定の中心座標
        Vec3 center;     

    private:
        // 判定の種類
        ColKind m_kind;  

        using CollideFunc_t = CollideHitInfo(ColliderBase::*)(const Vec3&, const Rigidbody&, ColliderBase*, const Vec3&, const Rigidbody&, const PreHitInfo&);
        std::unordered_map<ColKind, CollideFunc_t> m_collideFunc;
        using FixFunc_t = Vec3(ColliderBase::*)(const Vec3&, ColliderBase*, const Vec3&, const CollideHitInfo&);
        std::unordered_map<ColKind, FixFunc_t> m_fixFunc;
    };
}