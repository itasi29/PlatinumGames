#pragma once
#include "Object3DBase.h"

/// <summary>
/// ガラスクラス
/// </summary>
class Glass : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Glass();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Glass();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="scale">スケール</param>
    /// <param name="rot">回転</param>
    /// <param name="list">コライダーリスト</param>
    /// <param name="isGravity">重力を使用するかどうか</param>
    void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity) override;
};
