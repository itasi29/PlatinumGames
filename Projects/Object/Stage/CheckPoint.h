#pragma once
#include "Collidable.h"

class StageManager;
class GimmickLinkObject;

/// <summary>
/// チェックポイントクラス
/// </summary>
class CheckPoint : public MyEngine::Collidable
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="mgr">ステージマネージャー</param>
    /// <param name="no">チェックポイント番号</param>
    CheckPoint(StageManager& mgr, const int no);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CheckPoint();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="dir">方向</param>
    /// <param name="size">サイズ</param>
    /// <param name="radius">半径</param>
	/// <param name="respawnDir">リスポーン方向</param>
    void Init(const Vec3& pos, const Vec3& dir, float size, float radius, const Vec3& respawnDir);

    /// <summary>
    /// リンクオブジェクトを設定する
    /// </summary>
    /// <param name="link">リンクオブジェクト</param>
    void SetLinkObj(GimmickLinkObject* link) { m_link = link; }

    /// <summary>
    /// トリガー侵入時の処理
    /// </summary>
    /// <param name="colider">衝突した対象</param>
    /// <param name="selfIndex">自身の判定インデックス</param>
    /// <param name="sendIndex">対象の判定インデックス</param>
    /// <param name="hitInfo">衝突情報</param>
    virtual void OnTriggerEnter(MyEngine::Collidable* colider, int selfIndex, int sendIndex, const MyEngine::CollideHitInfo& hitInfo) override;

    const Vec3& GetDir() const { return m_respawnDir; }

private:
    // ステージマネージャー
    StageManager& m_mgr;  
    // チェックポイント番号
    const int m_no;  

    // リンクオブジェクト
    GimmickLinkObject* m_link;

	// リスポーン時カメラ方向
	Vec3 m_respawnDir;
};
