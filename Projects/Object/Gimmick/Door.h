#pragma once
#include "GimmickLinkObject.h"

class FileBase;

/// <summary>
/// ドアクラス
/// </summary>
class Door : public GimmickLinkObject
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Door();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Door();

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
    /// リスタート処理
    /// </summary>
    void Restart() override;

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
    /// ギミックオンの処理
    /// </summary>
    void ProcessGimmickOn() override;
    /// <summary>
    /// ギミックオフの処理
    /// </summary>
    void ProcessGimmickOff() override;

private:
    // 開くサウンドエフェクト
    std::shared_ptr<FileBase> m_openSe;  

    // 初期位置
    Vec3 m_startPos;  
    // 右方向
    Vec3 m_right;  

    // 開始フラグ
    bool m_isStart;  
    // 開くフラグ
    bool m_isOpen;  
    // 閉じるフラグ
    bool m_isClose;  
};
