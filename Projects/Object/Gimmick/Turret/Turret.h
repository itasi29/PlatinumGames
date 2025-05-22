#pragma once
#include "Object/Object3DBase.h"
#include <list>
#include <memory>

class Player;
class TurretBullet;
class AnimController;
class FileBase;

/// <summary>
/// タレットクラス
/// </summary>
class Turret : public Object3DBase
{
private:
    enum class State
    {
        SEARCH,  // 探索状態
        OPEN,    // オープン状態
        ATTACK,  // 攻撃状態
        CLOSE,   // クローズ状態
        DEATH    // 死亡状態
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Turret();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Turret();

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
    /// 初期化
    /// </summary>
    /// <param name="dir">方向</param>
    /// <param name="player">プレイヤー</param>
    void Init(const Vec3& dir, Player* player);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const override;

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="dir">ダメージ方向</param>
    void OnDamage(const Vec3& dir);

private:
    /// <summary>
    /// アニメーションの更新処理
    /// </summary>
    void AnimUpdate();
    /// <summary>
    /// 探索状態の更新処理
    /// </summary>
    void SearchUpdate();
    /// <summary>
    /// オープン状態の更新処理
    /// </summary>
    void OpenUpdate();
    /// <summary>
    /// 攻撃状態の更新処理
    /// </summary>
    void AttackUpdate();
    /// <summary>
    /// クローズ状態の更新処理
    /// </summary>
    void CloseUpdate();
    /// <summary>
    /// 死亡状態の更新処理
    /// </summary>
    void DeathUpdate();

    /// <summary>
    /// プレイヤーを探索しているかどうかを確認する
    /// </summary>
    /// <returns>true:探索している / false:探索していない</returns>
    bool IsSearch() const;

    /// <summary>
    /// 弾を発射する
    /// </summary>
    void OnShot();

    /// <summary>
    /// 探索状態に遷移する
    /// </summary>
    void OnSearch();
    /// <summary>
    /// オープン状態に遷移する
    /// </summary>
    void OnOpen();
    /// <summary>
    /// 攻撃状態に遷移する
    /// </summary>
    void OnAttack();
    /// <summary>
    /// クローズ状態に遷移する
    /// </summary>
    void OnClose();

private:
    // 更新関数
    using UpdateFunc_t = void(Turret::*)();
    UpdateFunc_t m_updateFunc;  

    // アニメーションコントローラー
    std::shared_ptr<AnimController> m_anim;  
    // 死亡エフェクト
    std::shared_ptr<FileBase> m_deathEff;  

    // プレイヤー
    Player* m_player;  

    // 弾リスト
    std::list<std::shared_ptr<TurretBullet>> m_bulletList;  

    // 現在の状態
    State m_state;  

    // 基本方向
    Vec3 m_baseDir;  
    // 見ている方向
    Vec3 m_lookDir;  
    // 落下回転
    Quaternion m_fallRot;  
    // 次の方向
    Vec3 m_nextDir;  

    // 攻撃フレーム
    int m_attackFrame;  
    // 死亡後モデル描画フレーム
    int m_deathFrame;
    // 死亡エフェクトハンドル
    int m_deathEffPlayH;  
    // 右ウィングハンドル
    int m_rightWingH;  
    // 左ウィングハンドル
    int m_leftWingH;  

    // 左ウィングから弾を生成するかどうか
    bool m_isCreateLeft;
    // 死亡しているかどうか
	bool m_isDeath;
};





