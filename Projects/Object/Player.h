#pragma once
#include <unordered_map>
#include <memory>
#include "Object/Object3DBase.h"

class FileBase;
class Camera;
class PlayerCamera;
class GateManager;
class HandObject;
class AnimController;
namespace MyEngine
{
	class ColliderSphere;
	class ColliderCapsule;
	struct CollideHitInfo;
}

// プレイヤーの状態
enum class PlayerState
{
	IDLE,
	WALK,
	RUN,
	JUMP,
	AERIAL,
	LANDING,
	HIT,
	DEATH,
};

class Player : public Object3DBase
{
public:
	Player(const std::shared_ptr<PlayerCamera>& camera, const std::shared_ptr<GateManager>& gateMgr);
	virtual ~Player();

	/// <summary>
	/// 非同期初期化処理
	/// </summary>
	void AsyncInit();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pos">初期位置</param>
	/// <param name="dir">初期向き</param>
	/// <param name="isOneHand">片手ステージかどうか</param>
	void Init(const Vec3& pos, const Vec3& dir, bool isOneHand);

	/// <summary>
	/// リスタート処理
	/// </summary>
	/// <param name="pos">スポーン位置</param>
	/// <param name="dir">向いている方向</param>
	void Restart(const Vec3& pos, const Vec3& dir);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 銃の描画
	/// </summary>
	void DrawGun() const;
	/// <summary>
	/// HP描画
	/// </summary>
	void DrawHpUI() const;
	/// <summary>
	/// Pad情報描画
	/// </summary>
	void DrawPadUI() const;
	/// <summary>
	/// ダメージを分かりやすく描画
	/// </summary>
	void DrawDamageFillter() const;

	/// <summary>
	/// ダメージ
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="hitDir">ヒットバック方向</param>
	void OnDamage(int damage, const Vec3& hitDir);
	/// <summary>
	/// プレイヤーを停止させる(速度を0にする)
	/// </summary>
	void OnStop();
	/// <summary>
	/// プレイヤーを指定の座標までワープさせる
	/// </summary>
	/// <param name="pos">座標</param>
	void OnWarpPos(const Vec3& pos);

	/// <summary>
	/// 死亡判定
	/// </summary>
	/// <returns>true: 死亡/ false: 生存</returns>
	bool IsDeath() const { return m_isDeath; }
	/// <summary>
	/// カメラを取得
	/// </summary>
	/// <returns>プレイヤーカメラ</returns>
	std::shared_ptr<Camera> GetCamera() const;
	/// <summary>
	/// 現在のステートを取得する
	/// </summary>
	/// <returns>現在のステート</returns>
	PlayerState GetNowState() const { return m_nowState; }

	/// <summary>
	/// 衝突開始時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnCollideEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// 衝突中の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnCollideStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー侵入時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー中の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerStay(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;
	/// <summary>
	/// トリガー退出時の処理
	/// </summary>
	/// <param name="colider">衝突した対象</param>
	/// <param name="selfIndex">自身の判定インデックス</param>
	/// <param name="sendIndex">対象の判定インデックス</param>
	/// <param name="hitInfo">衝突情報</param>
	virtual void OnTriggerExit(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

private:
	/// <summary>
	/// 手で持つ処理
	/// </summary>
	void HandUpdate();
	/// <summary>
	/// 銃処理
	/// </summary>
	void GunUpdate();
	/// <summary>
	/// 回復処理
	/// </summary>
	void RecoverHpUpdate();
	/// <summary>
	/// HPバー処理
	/// </summary>
	void HpBarUpdate();
	/// <summary>
	/// モデル回転処理
	/// </summary>
	void RotationUpdate();
	/// <summary>
	/// アニメーション処理
	/// </summary>
	void AnimUpdate();
	/// <summary>
	/// カメラ処理
	/// </summary>
	void CameraUpdate();

	/// <summary>
	/// 待機状態処理
	/// </summary>
	void IdleUpdate();
	/// <summary>
	/// 歩き状態処理
	/// </summary>
	void WalkUpdate();
	/// <summary>
	/// 走り状態処理
	/// </summary>
	void RunUpdate();
	/// <summary>
	/// ジャンプ状態処理
	/// </summary>
	void JumpUpdate();
	/// <summary>
	/// 空中状態処理
	/// </summary>
	void AerialUpdate();
	/// <summary>
	/// 着地状態処理
	/// </summary>
	void LandingUpdate();
	/// <summary>
	/// ヒットストップ状態処理
	/// </summary>
	void HitUpdate();
	/// <summary>
	/// 死亡状態処理
	/// </summary>
	void DeathUpdate();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="speed">移動速度</param>
	/// <param name="isFixedStick">スティックの値を固定するか</param>
	/// <returns>true:移動した /false:移動していない</returns>
	bool Move(float speed, bool isFixedStick);
	/// <summary>
	/// 空中移動処理
	/// </summary>
	void AerialMove();

	/// <summary>
	/// 待機状態に遷移
	/// </summary>
	void OnIdle();
	/// <summary>
	/// 歩き状態に遷移
	/// </summary>
	void OnWalk();
	/// <summary>
	///	走り状態に遷移
	/// </summary>
	void OnRun();
	/// <summary>
	/// ジャンプ状態に遷移
	/// </summary>
	void OnJump();
	/// <summary>
	/// 空中状態に遷移
	/// </summary>
	void OnAerial();
	/// <summary>
	/// 着地状態に遷移
	/// </summary>
	void OnLanding();
	/// <summary>
	/// ヒットストップ状態に遷移
	/// </summary>
	void OnHit();
	/// <summary>
	/// 死亡状態に遷移
	/// </summary>
	void OnDeath();

	/// <summary>
	/// 銃の発射処理
	/// </summary>
	void OnShot();
	/// <summary>
	/// 手で掴む・離す処理
	/// </summary>
	/// <param name="obj">手で持てるオブジェクト</param>
	void OnHand(MyEngine::Collidable* obj);

	/// <summary>
	/// Pad情報描画
	/// </summary>
	/// <param name="y">描画位置(y)</param>
	/// <param name="isActive">そのボタンが使えるかどうか</param>
	/// <param name="imageId">画像ID</param>
	/// <param name="str">文字列</param>
	void DrawPadUI(int y, bool isActive, int imageId, const wchar_t* const str) const;

private:
	// 更新関数
	using UpdateFunc_t = void(Player::*)();
	UpdateFunc_t m_updateFunc;

	// ファイルリスト
	std::unordered_map<int, std::shared_ptr<FileBase>> m_files;

	std::shared_ptr<PlayerCamera> m_camera;	// カメラ
	std::shared_ptr<GateManager> m_gateMgr;	// ゲートマネージャ
	std::shared_ptr<AnimController> m_anim;	// アニメーション

	std::shared_ptr<MyEngine::ColliderCapsule> m_centerCol;	// 中心コライダー
	std::shared_ptr<MyEngine::ColliderSphere> m_handCol;	// 手で掴む・離す用コライダー

	// 現在のステート
	PlayerState m_nowState;
	// 現在持っているオブジェクト
	HandObject* m_handObj;
	// 次のモデル回転情報
	Quaternion m_nextRot;
	// ゲートの位置
	Vec3 m_gatePos;

	int m_gunVS;	// 銃の頂点シェーダ
	int m_gunPS;	// 銃のピクセルシェーダ
	int m_hp;	 // HP
	int m_preHp; // 前のHP
	int m_hitStopFrame; // ヒットストップフレーム
	int m_receverFrame; // 回復待機フレーム
	int m_shotInteval; // ショットインターバル
	int m_shakeHpBarFrame; // HPバーの揺れるフレーム
	int m_stayAerialMove; // 空中移動の待機フレーム

	bool m_isOneHand;	// 片手ステージかどうか
	bool m_isDeath;	// 死亡フラグ
	bool m_isRecever;	// 回復中かどうか
	bool m_isGround;	// 接地しているかどうか
	bool m_isCanHold;	// 物を持てるかどうか
	bool m_isHold;	// 物を持っているかどうか
	bool m_isCanWarp;	// ワープできるかどうか
	bool m_isWarpRange;	// ワープ範囲内かどうか
	bool m_isChangeNear;	// カメラのニアを変更しているかどうか

	// タグ追加フラグ
	std::unordered_map<MyEngine::Collidable*, bool> m_isAddTag;
};
