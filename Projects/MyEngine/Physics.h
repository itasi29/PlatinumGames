#pragma once
#include <memory>
#include <list>
#include <vector>
#include "ObjectTag.h"
#include "Utility.h"
#include "Vec3.h"
#include "Collider/ColliderBase.h"

namespace MyEngine
{
    class Rigidbody;
    class Collidable;

    /// <summary>
    /// 物理管理クラス
    /// </summary>
    class Physics final
    {
    public:
        /// <summary>
        /// 衝突情報の種類
        /// </summary>
        enum class OnCollideInfoKind
        {
            CollideEnter,
            CollideStay,
            CollideExit,
            TriggerEnter,
            TriggerStay,
            TriggerExit
        };

        /// <summary>
		/// 判定チェックリスト情報
        /// </summary>
        struct CheckList
        {
            std::shared_ptr<Collidable> primary;
            std::shared_ptr<Collidable> secondary;
            bool isSamePriority = false;
        };

        /// <summary>
        /// 判定リスト情報
        /// </summary>
        struct CheckData
        {
            std::shared_ptr<Collidable> primary;
            std::shared_ptr<Collidable> secondary;
            int primaryIndex;
            int secondaryIndex;
        };

        /// <summary>
        /// 通知情報
        /// </summary>
        struct SendInfo
        {
            std::weak_ptr<Collidable> own;
            std::weak_ptr<Collidable> send;
            int ownColIndex = -1;
            int sendColIndex = -1;
            CollideHitInfo hitInfo;
        };

        /// <summary>
        /// 通知選択情報
        /// </summary>
        struct OnCollideInfoData
        {
            std::weak_ptr<Collidable> own;
            std::weak_ptr<Collidable> send;
            int ownColIndex = -1;
            int sendColIndex = -1;
            CollideHitInfo hitInfo;
            OnCollideInfoKind kind;
        };

        /// <summary>
        /// 当たった情報
        /// </summary>
        struct HitObjectInfo
        {
            Collidable* col;
            CollideHitInfo hitInfo;
            int colIndex = -1;
        };

        using SendCollideInfo = std::list<SendInfo>;
        using PreHitInfoList_t = std::unordered_map<ObjectTag, PreHitInfo>;
        using PreHitInfoTable_t = std::unordered_map<std::shared_ptr<Collidable>, PreHitInfoList_t>;

    private:
        /// <summary>
        /// コンストラクタ
        /// </summary>
        Physics();

		Physics(const Physics&) = delete;
		Physics(const Physics&&) = delete;
		Physics& operator= (const Physics&) = delete;
		Physics& operator= (const Physics&&) = delete;

    public:
        /// <summary>
        /// インスタンスを取得する
        /// </summary>
        /// <returns>Physicsのインスタンス</returns>
        static Physics& GetInstance();

        /// <summary>
        /// Physicsに登録する
        /// </summary>
        /// <param name="collidable">登録する情報</param>
        void Entry(std::shared_ptr<Collidable> collidable);

        /// <summary>
        /// Physicsから削除する
        /// </summary>
        /// <param name="collidable">削除する情報</param>
        void Exit(std::shared_ptr<Collidable> collidable);

        /// <summary>
        /// 更新処理
        /// </summary>
        void Update();

        /// <summary>
        /// 判定
        /// </summary>
        /// <param name="pos">座標</param>
        /// <param name="rigid">リジッドボディ</param>
        /// <param name="collider">判定</param>
        /// <param name="count">カウント</param>
        /// <param name="checkNum">判定回数</param>
        /// <param name="isFix">true: 位置修正する /false: 位置修正しない</param>
        /// <param name="preInfoList">事前判定情報リスト</param>
        /// <param name="checkTags">判定タグリスト(emptyなら全タグチェック)</param>
        /// <param name="throughObj">スルーオブジェクトリスト(emptyなら全オブジェクトチェック)</param>
        /// <returns>当たった情報群</returns>
        std::vector<HitObjectInfo> CheckObject(Vec3& pos, Rigidbody rigid, ColliderBase* collider, int& count, int checkNum = 1, bool isFix = false, PreHitInfoList_t preInfoList = {}, std::list<ObjectTag> checkTags = {}, std::list<Collidable*> throughObj = { nullptr });

    private:
        /// <summary>
        /// オブジェクトが持つ移動量から次の移動座標を決定する
        /// </summary>
        void MoveNextPos() const;

        /// <summary>
        /// 判定確認
        /// </summary>
        void CheckCollide();

        /// <summary>
        /// 判定リストの取得
        /// </summary>
        /// <returns>判定リスト</returns>
        std::list<CheckData> GetCollisionList() const;

        /// <summary>
        /// チェックリストを作成
        /// </summary>
        void MakeCheckList();

        /// <summary>
        /// 判定通知リストに追加する
        /// </summary>
        /// <param name="objA">オブジェクトA</param>
        /// <param name="objB">オブジェクトB</param>
        /// <param name="info">通知リスト</param>
        /// <param name="hitInfo">当たった情報</param>
        void AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, int colIndexA, int colIndexB, SendCollideInfo& info, const CollideHitInfo& hitInfo);

        /// <summary>
        /// 通知を種類(当たった時・当たっている状態・離れた時)に分ける
        /// </summary>
        /// <param name="preSendInfo">一つ前の送った通知リスト</param>
        /// <param name="newSendInfo">今回送る通知リスト</param>
        /// <param name="isTrigger">true:OnCollide～に送る /false: OnTrigger～に送る</param>
        void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);

        /// <summary>
        /// 最終的な判定通知リストに追加する
        /// </summary>
        /// <param name="info">情報</param>
        /// <param name="kind">通知の種類</param>
        void AddOnCollideInfo(const SendInfo& info, OnCollideInfoKind kind);

        /// <summary>
        /// 通知の種類に合わせて通知関数を呼ぶ
        /// </summary>
        /// <param name="own">送る側</param>
        /// <param name="send">通知を受け取る側</param>
        /// <param name="ownIndex">判定データの要素番号(自身)</param>
        /// <param name="sendIndex">判定データの要素番号(対象)</param>
        /// <param name="hitInfo">当たった情報</param>
        /// <param name="kind">通知の種類</param>
        void OnCollideInfo(const std::weak_ptr<Collidable>& own, const std::weak_ptr<Collidable>& send, int ownIndex, int sendIndex, const CollideHitInfo& hitInfo, OnCollideInfoKind kind);

        /// <summary>
        /// 座標を確定させる
        /// </summary>
        void FixPos() const;

#ifdef _DEBUG
        /// <summary>
        /// デバック情報の追加
        /// </summary>
        /// <param name="pos">座標</param>
        /// <param name="colliders">当たり判定データ</param>
        /// <param name="color">色</param>
        void AddDebugInfo(const Vec3& pos, const std::vector<std::shared_ptr<ColliderBase>>& colliders, unsigned int color) const;
#endif

    private:
        // 登録されたオブジェクト
        std::vector<std::shared_ptr<Collidable>> m_objects;  
        // チェックリスト
        std::list<CheckList> m_checkList;  

        // 衝突通知情報リスト
        std::list<OnCollideInfoData> m_onCollideInfo;  
        // 新しい衝突通知情報
        SendCollideInfo m_newCollideInfo;  
        // 前回の衝突通知情報
        SendCollideInfo m_preCollideInfo;  
        // 新しいトリガー通知情報
        SendCollideInfo m_newTirrigerInfo; 
        // 前回のトリガー通知情報
        SendCollideInfo m_preTirrigerInfo; 

        // チェックリストを作成しないかどうか
        bool m_isNoMake;  
    };
}