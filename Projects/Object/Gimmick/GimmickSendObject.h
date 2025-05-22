#pragma once
#include "Object/Object3DBase.h"

class GimmickLinkObject;

/// <summary>
/// ギミック送信クラス
/// </summary>
class GimmickSendObject : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="priority">優先度</param>
    /// <param name="tag">オブジェクトタグ</param>
    GimmickSendObject(Priority priority, ObjectTag tag);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~GimmickSendObject();

    /// <summary>
    /// リンクオブジェクトを設定する
    /// </summary>
    /// <param name="linkObj">リンクオブジェクト</param>
    void SetLinkObject(GimmickLinkObject* linkObj) { m_linkObj = linkObj; }

protected:
    // リンクオブジェクト
    GimmickLinkObject* m_linkObj;  
};
