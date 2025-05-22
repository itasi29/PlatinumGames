#pragma once
#include "GimmickLinkObject.h"

/// <summary>
/// ピストンクラス
/// </summary>
class Piston : public GimmickLinkObject
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Piston();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Piston();

    /// <summary>
    /// 移動範囲を設定する
    /// </summary>
    /// <param name="start">開始位置</param>
    /// <param name="end">終了位置</param>
    void SetMoveRange(const Vec3& start, const Vec3& end) { m_start = start; m_end = end; }

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
    // 開始位置
    Vec3 m_start;  
    // 終了位置
    Vec3 m_end;  

    // 拡張フラグ
    bool m_isExpand;  
    // 収縮フラグ
    bool m_isShrink;  
};
