#pragma once
#include "Vec3.h"
#include "Quaternion.h"

/// <summary>
/// カメラ情報を格納する構造体
/// </summary>
struct CameraInfo
{
	// 上方向固定フラグ
	bool isUpFixed = false;
	// Tpsフラグ
	bool isTps = false;
	// near, far
	float n = 1.0f;
	float f = 100.0f;
	// 視野角
	float fov = 0.0f;
	// ターゲット位置
	Vec3 targetPos;
	// 正面方向
	Vec3 front;
	// 右方向
	Vec3 right;
	// 縦方向の回転具合
	float vertexAngle = 0.0f;
	Quaternion vertexRot;
	// 見ている方向
	Vec3 look;
	// 場所
	Vec3 cameraPos;
};

namespace MyEngine
{
	class ColliderSphere;
	struct CollideHitInfo;
}
struct Quaternion;
class GateManager;

/// <summary>
/// カメラクラス
/// </summary>
class Camera
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Camera();

	/// <summary>
	/// カメラ情報を更新する
	/// </summary>
	void Update();
	/// <summary>
	/// カメラ情報を取得する
	/// </summary>
	/// <returns>カメラ情報</returns>
	const CameraInfo& GetInfo() const { return m_info; }

	/// <summary>
	/// near, farを設定する
	/// </summary>
	/// <param name="n">near(-値ならデフォルトの値になる)</param>
	/// <param name="f">far(-値ならデフォルトの値になる)</param>
	void SetNearFar(float n, float f);
	/// <summary>
	/// 視野角を設定する
	/// </summary>
	/// <param name="angle">角度(deg)(-値ならデフォルトの値になる)</param>
	void SetFov(float angle);
	/// <summary>
	/// 正面方向を設定する
	/// </summary>
	/// <param name="front">正面方向のベクトル</param>
	/// <param name="vertexAngle">縦方向の回転角度</param>
	void SetFront(const Vec3& front, float vertexAngle);
	/// <summary>
	/// 初期位置を設定する
	/// </summary>
	/// <param name="targetPos">カメラの注視点</param>
	void SetTargetPos(const Vec3& targetPos);
	/// <summary>
	/// TPS/FPSを設定する
	/// </summary>
	/// <param name="isTps">true:TPS /false: FPS</param>
	void SetTps(bool isTps);
	/// <summary>
	/// カメラの上方向固定化を設定する
	/// </summary>
	/// <param name="isUpFixed">true: 固定(0, 1, 0) /false: 正面、右方向から上を計算</param>
	void SetUpFixed(bool isUpFixed) { m_info.isUpFixed = isUpFixed; }

	/// <summary>
	/// カメラを有効化する
	/// </summary>
	void AppInfo();

	/// <summary>
	/// カメラの位置を取得する
	/// </summary>
	/// <returns>カメラの位置</returns>
	const Vec3& GetPos() const;
	/// <summary>
	/// TPSかFPSかを判定する
	/// </summary>
	/// <returns>true:TPS /false: FPS</returns>
	bool IsTps() const { return m_info.isTps; }

protected:
	/// <summary>
	/// カメラの距離を取得する
	/// </summary>
	/// <returns>カメラの距離</returns>
	float GetDistance() const;

protected:
	// カメラ情報
	CameraInfo m_info;	
};