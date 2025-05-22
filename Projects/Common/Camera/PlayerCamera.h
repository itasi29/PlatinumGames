#pragma once
#include "Camera.h"

/// <summary>
/// プレイヤーカメラクラス
/// </summary>
class PlayerCamera : public Camera
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    PlayerCamera();
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PlayerCamera();

    /// <summary>
    /// プレイヤーカメラ情報を更新する
    /// </summary>
    /// <param name="playerPos">プレイヤーの座標</param>
    void Update(const Vec3& playerPos);

    /// <summary>
    /// ワープ処理を行う
    /// </summary>
    /// <param name="preVelDir">ワープ前の速度方向</param>
    /// <param name="newVelDir">ワープ後の速度方向</param>
    /// <param name="pos">ワープ先の座標</param>
    void OnWarp(const Vec3& preVelDir, const Vec3& newVelDir, const Vec3& pos);

private:
    // フレーム数
    int m_frame;
    // 回転フラグ
    bool m_isTurn;
};