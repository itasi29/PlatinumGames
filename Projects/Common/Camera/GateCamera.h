#pragma once
#include "Camera.h"

class Gate;

/// <summary>
/// ゲートカメラクラス
/// </summary>
class GateCamera : public Camera
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    GateCamera();
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~GateCamera();

    /// <summary>
    /// ゲートカメラ情報を更新する
    /// </summary>
    /// <param name="playerCamera">プレイヤーカメラの情報</param>
    /// <param name="pairBase">ペアゲートの基準位置</param>
    void Update(const CameraInfo& playerCamera, const Vec3& pairBase);

    /// <summary>
    /// 基準となる視線方向、縦軸回転角度を設定する
    /// </summary>
    /// <param name="dir">視線方向</param>
    /// <param name="angle">縦軸回転角度</param>
    void SetBase(const Vec3& dir, float angle);

private:
    // 基準となる視線方向
    Vec3 m_baseViewDir;
	// 初期縦軸回転角度
	float m_baseVertexAngle;
};