#pragma once
#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include "ObjectTag.h"
#include "Rigidbody.h"

namespace MyEngine
{
	class Physics;
	class ColliderBase;
	struct CollideHitInfo;
	enum class ColKind;

	/// <summary>
	/// コライダブルクラス
	/// </summary>
	class Collidable abstract : public std::enable_shared_from_this<Collidable>
	{
		friend Physics;
	public:
		// 優先度判別(小さいほうが動かされる)
		enum class Priority : int
		{
			LOW,		// 低
			MIDDLE,		// 中
			HIGH,		// 高
			STATIC,		// 動かない（最高）
		};

	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="priority">優先順位</param>
		/// <param name="tag">オブジェクトタグ</param>
		Collidable(Priority priority, ObjectTag tag);

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Collidable();

		/// <summary>
		/// 終了処理
		/// </summary>
		virtual void End();

		/// <summary>
		/// 判定情報を取得
		/// </summary>
		/// <returns>判定情報</returns>
		/// <param name="index">取得したい判定情報の番号</param>
		ColliderBase* GetColliderData(int index) const;

		/// <summary>
		/// 物体同士が当たった時の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="selfIndex">判定の要素番号(自身)</param>
		/// <param name="sendIndex">判定の要素番号(対象)</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnCollideEnter(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);
		/// <summary>
		/// 物体同士が当たっている状態の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="selfIndex">判定の要素番号(自身)</param>
		/// <param name="sendIndex">判定の要素番号(対象)</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnCollideStay(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);
		/// <summary>
		/// 物体同士が離れた時の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="selfIndex">判定の要素番号(自身)</param>
		/// <param name="sendIndex">判定の要素番号(対象)</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnCollideExit(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);
		/// <summary>
		/// 物体が侵入した時の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="colIndex">判定の要素番号</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnTriggerEnter(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);
		/// <summary>
		/// 物体が通過中の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="selfIndex">判定の要素番号(自身)</param>
		/// <param name="sendIndex">判定の要素番号(対象)</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnTriggerStay(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);
		/// <summary>
		/// 物体が通過しきった時の通知処理
		/// </summary>
		/// <param name="colider">当たった対象の情報</param>
		/// <param name="selfIndex">判定の要素番号(自身)</param>
		/// <param name="sendIndex">判定の要素番号(対象)</param>
		/// <param name="hitInfo">当たった場所</param>
		virtual void OnTriggerExit(Collidable* colider, int selfIndex, int sendIndex, const CollideHitInfo& hitInfo);

		/// <summary>
		/// タグの取得
		/// </summary>
		/// <returns>タグ</returns>
		ObjectTag GetTag() const { return m_tag; }
		/// <summary>
		/// 優先順位の取得
		/// </summary>
		/// <returns>優先順位</returns>
		Priority GetPriority() const { return m_priority; }
		/// <summary>
		/// 座標の取得
		/// </summary>
		/// <returns>座標</returns>
		const Vec3& GetPos() const;
		/// <summary>
		/// 速度の取得
		/// </summary>
		/// <returns>速度</returns>
		const Vec3& GetVelocity() const;

		/// <summary>
		/// ある方向に力を加える
		/// </summary>
		/// <param name="dir">力の方向</param>
		/// <param name="power">力の強さ</param>
		void AddForce(const Vec3& dir, float power);

	protected:
		/// <summary>
		/// Physicsに登録する
		/// </summary>
		void OnEntryPhysics();
		/// <summary>
		/// Physicsから削除する
		/// </summary>
		void OnExistPhysics();
		/// <summary>
		/// 当たり判定データの生成
		/// </summary>
		/// <param name="kind">当たり判定の種類</param>
		/// <returns>作成した判定データ/returns>
		std::shared_ptr<ColliderBase> CreateCollider(const ColKind& kind);

	private:
#ifdef _DEBUG
		/// <summary>
		/// 当たった文字列情報を出す
		/// </summary>
		/// <param name="start">{ Collide, Trigger }を入れる</param>
		/// <param name="end">{ 当たった, 当たっている, 離れた }を入れる</param>
		/// <param name="collider">対象の情報</param>
		/// <param name="selfIndex">判定インデックス番号(自身)</parm>
		/// <param name="sendIndex">判定インデックス番号(対象)</parm>
		void SendCollideInfo(const char* start, const char* const end, Collidable* collider, int selfIndex, int sendIndex);
#endif

	protected:
		// 物理データ
		Rigidbody m_rigid;
		// 当たり判定データ
		std::vector<std::shared_ptr<ColliderBase>> m_collider;
		// Trigger判定だけを取るオブジェクトタグ
		std::list<ObjectTag> m_throughTag;
		// 地面判定タグ
		std::list<ObjectTag> m_groundTag;
		// 壁判定タグ
		std::list<ObjectTag> m_wallTag;

	private:
		// オブジェクトタグ
		ObjectTag m_tag;
		// 優先順位
		Priority m_priority;
		// PhysicsにEntryしているか
		bool m_isEntry;
	};
}

