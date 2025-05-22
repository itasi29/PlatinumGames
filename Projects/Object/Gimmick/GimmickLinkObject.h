#pragma once
#include "Object/Object3DBase.h"
#include "StateMachine.h"

/// <summary>
/// ギミックリンククラス
/// </summary>
class GimmickLinkObject : public Object3DBase
{
protected:
    /// <summary>
    /// ギミックの状態を表す列挙型
    /// </summary>
    enum class State
    {
        GIMMICK_ON,  // ギミックオン状態
        GIMMICK_OFF  // ギミックオフ状態
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="priority">優先度</param>
    /// <param name="tag">オブジェクトタグ</param>
    GimmickLinkObject(Priority priority, ObjectTag tag);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~GimmickLinkObject();

    /// <summary>
    /// リスタート処理
    /// </summary>
    virtual void Restart() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() override;

    /// <summary>
    /// ギミックをオンにする
    /// </summary>
    virtual void OnGimmick();
    /// <summary>
    /// ギミックをオフにする
    /// </summary>
    virtual void StopGimmick();

protected:
    /// <summary>
    /// ギミックオンの更新処理
    /// </summary>
    virtual void GimmickOnUpdate() = 0;
    /// <summary>
    /// ギミックオフの更新処理
    /// </summary>
    virtual void GimmickOffUpdate() = 0;

    /// <summary>
    /// ギミックオンの処理
    /// </summary>
    virtual void ProcessGimmickOn() {}
    /// <summary>
    /// ギミックオフの処理
    /// </summary>
    virtual void ProcessGimmickOff() {}

protected:
    // ステートマシン
    StateMachine<State> m_stateMachine;

private:
    // ギミックオンフラグ
    bool m_isOn;  
    // ギミックリリースフラグ
    bool m_isRelease;  
    // カウント
    int m_count;  
};
