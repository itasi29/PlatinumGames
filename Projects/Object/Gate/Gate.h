#pragma once
#include "Object/Object3DBase.h"
#include <memory>
#include "Collidable.h"
#include "GateManager.h"
#include "Vec3.h"
#include "Quaternion.h"

namespace MyEngine
{
    class ColliderCapsule;
}
class Player;
class FileBase;
class GateCamera;

/// <summary>
/// ゲートクラス
/// </summary>
class Gate : public Object3DBase
{
private:
    struct UserData
    {
        Vec3 center;  // 中心座標
        float radius;  // 半径
        Vec3 dir;  // 方向
        float size;  // サイズ
        float frame;  // フレーム
        bool isCreate;  // 作成フラグ
        bool dummy1[3];  // ダミー
        float dummy2[2];  // ダミー
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="camera">ゲートカメラ</param>
    /// <param name="cameraFromPair">ペアゲートのカメラ</param>
    /// <param name="kind">ゲートの種類</param>
    Gate(const std::shared_ptr<GateCamera>& camera, const std::shared_ptr<GateCamera>& cameraFromPair, GateKind kind);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~Gate();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="hitObjTag">ヒットオブジェクトタグ</param>
    /// <param name="pos">位置</param>
    /// <param name="norm">法線</param>
    /// <param name="dir">方向</param>
    /// <param name="player">プレイヤー</param>
    void Init(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir, const std::weak_ptr<Player>& player);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 位置を変更する
    /// </summary>
    /// <param name="hitObjTag">ヒットオブジェクトタグ</param>
    /// <param name="pos">位置</param>
    /// <param name="norm">法線</param>
    /// <param name="dir">方向</param>
    void ChangePos(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir);
    /// <summary>
    /// カメラ情報を設定する
    /// </summary>
    void SetCameraInfo();
    /// <summary>
    /// ペアゲートを設定する
    /// </summary>
    /// <param name="pairGate">ペアゲート</param>
    void SetPairGate(const std::shared_ptr<Gate>& pairGate) { m_pairGate = pairGate; }

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// ゲートを描画する
    /// </summary>
    /// <param name="tex">テクスチャ</param>
    void DrawGate(int tex) const;

    /// <summary>
    /// ワープをチェックする
    /// </summary>
    /// <param name="targetPos">ターゲットの位置</param>
    /// <returns>ワープするかどうか</returns>
    bool CheckWarp(const Vec3& targetPos);
    /// <summary>
    /// ワープ処理
    /// </summary>
    /// <param name="targetPos">ターゲットの位置</param>
    /// <param name="targetRigid">ターゲットのリジッドボディ</param>
    /// <param name="isChangeVel">速度を変更するかどうか</param>
	/// <param name="height">モデルの高さ</param>
    void OnWarp(const Vec3& targetPos, MyEngine::Rigidbody& targetRigid, bool isChangeVel, float height);

    /// <summary>
    /// ゲートの種類を取得する
    /// </summary>
    /// <returns>ゲートの種類</returns>
    GateKind GetKind() const { return m_kind; }
    /// <summary>
    /// ヒットオブジェクトタグを取得する
    /// </summary>
    /// <returns>ヒットオブジェクトタグ</returns>
    ObjectTag GetHitObjectTag() const { return m_hitObjTag; }
    /// <summary>
    /// コライダーを取得する
    /// </summary>
    /// <returns>コライダー</returns>
    const std::shared_ptr<MyEngine::ColliderCapsule> GetCol() const { return m_collider; }
    /// <summary>
    /// ペアゲートを取得する
    /// </summary>
    /// <returns>ペアゲート</returns>
    const std::shared_ptr<Gate> GetPairGate() const { return m_pairGate; }
    /// <summary>
    /// 法線を取得する
    /// </summary>
    /// <returns>法線</returns>
    const Vec3& GetNorm() const { return m_norm; }
    /// <summary>
    /// 右方向を取得する
    /// </summary>
    /// <returns>右方向</returns>
    const Vec3& GetRight() const { return m_right; }

private:
    /// <summary>
    /// 開く状態の更新処理
    /// </summary>
    void OpenUpdate();

    /// <summary>
    /// 通常状態の更新処理
    /// </summary>
    void NormalUpdate();

    /// <summary>
    /// 閉じる状態の更新処理
    /// </summary>
    void CloseUpdate();

private:
    using UpdateFunc_t = void(Gate::*)();
    // 更新関数
    UpdateFunc_t m_updateFunc;  
    // ゲートカメラ
    std::shared_ptr<GateCamera> m_camera;  
    // ペアゲートのカメラ
    std::shared_ptr<GateCamera> m_cameraFromPair;  
    // ディゾルブエフェクト
    std::shared_ptr<FileBase> m_dissolveFile;  
    // ワープサウンドエフェクト
    std::shared_ptr<FileBase> m_warpSe;  
    // コライダー
    std::shared_ptr<MyEngine::ColliderCapsule> m_collider;  
    // ペアゲート
    std::shared_ptr<Gate> m_pairGate;  
    // プレイヤー
    std::weak_ptr<Player> m_player;  

    // ゲートの種類
    GateKind m_kind;  
    // ヒットオブジェクトタグ
    ObjectTag m_hitObjTag;  

    // 法線
    Vec3 m_norm;  
    // 右方向
    Vec3 m_right;  

    // 定数バッファハンドル
    int m_cbuffH;  

    // ユーザーデータ
    UserData* m_userData;  
};