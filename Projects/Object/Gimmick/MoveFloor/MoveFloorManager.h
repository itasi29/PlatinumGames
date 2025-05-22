#pragma once
#include "Object/Gimmick/GimmickLinkObject.h"
#include <vector>
#include <list>
#include <memory>

class MoveFloor;

/// <summary>
/// 移動床マネージャークラス
/// </summary>
class MoveFloorManager : public GimmickLinkObject
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    MoveFloorManager();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~MoveFloorManager();

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
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// リスタート処理
    /// </summary>
    void Restart() override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const override;

    /// <summary>
    /// ループ設定
    /// </summary>
    /// <param name="isLoop">ループするかどうか</param>
    void SetLoop(bool isLoop);
    /// <summary>
    /// チェックポイントテーブルを設定する
    /// </summary>
    /// <param name="table">チェックポイントテーブル</param>
    void SetCheckPtTable(const std::vector<Vec3>& table) { m_checkPtTable = table; }

    /// <summary>
    /// チェックポイント数を取得する
    /// </summary>
    /// <returns>チェックポイント数</returns>
    int GetCheckPtNum() const { return static_cast<int>(m_checkPtTable.size()); }
    /// <summary>
    /// 指定したチェックポイントの位置を取得する
    /// </summary>
    /// <param name="no">チェックポイント番号</param>
    /// <returns>チェックポイントの位置</returns>
    const Vec3& GetCheckPtPos(int no) const { return m_checkPtTable.at(no); }
    /// <summary>
    /// 移動方向を取得する
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="no">チェックポイント番号</param>
    /// <returns>移動方向</returns>
    Vec3 GetMoveDir(const Vec3& pos, int no) const;

    /// <summary>
    /// チェックポイントに到達したかどうかを確認する
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="no">チェックポイント番号</param>
    /// <returns>true:到達 / false:未到達</returns>
    bool CheckMovedCheckPt(const Vec3& pos, int no);

private:
    /// <summary>
    /// ギミックオンの更新処理
    /// </summary>
    void GimmickOnUpdate() override;
    /// <summary>
    /// ギミックオフの更新処理
    /// </summary>
    void GimmickOffUpdate() override;

    /// <summary>
    /// 作成更新処理
    /// </summary>
    void CreateUpdate();

    /// <summary>
    /// 床の更新処理
    /// </summary>
    void FloorUpdate();

    /// <summary>
    /// ギミックオンの処理
    /// </summary>
    void ProcessGimmickOn() override;
    /// <summary>
    /// ギミックオフの処理
    /// </summary>
    void ProcessGimmickOff() override;

private:
    // コライダーリスト
    std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> m_colList;  

    // 移動床リスト
    std::list<std::shared_ptr<MoveFloor>> m_floorList;  
    // チェックポイントテーブル
    std::vector<Vec3> m_checkPtTable;  

    // 作成フレーム
    int m_createFrame;  

    // ループフラグ
    bool m_isLoop;  
};
