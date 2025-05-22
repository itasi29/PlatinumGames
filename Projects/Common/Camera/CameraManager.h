#pragma once
#include <array>
#include <memory>

class Camera;
class GateManager;

/// <summary>
/// カメラの種類を表す列挙型
/// </summary>
enum class CameraKind
{
	PLAYER,         // プレイヤーカメラ
	GATE_A,         // ゲートAのカメラ
	GATE_B,         // ゲートBのカメラ
	GATE_A_FROM_B,  // ゲートBから見たゲートAのカメラ
	GATE_B_FROM_A,  // ゲートAから見たゲートBのカメラ
	TITLE_1,		// タイトルカメラ１
	TITLE_2,		// タイトルカメラ２
	MAX             // カメラの種類の最大値
};

/// <summary>
/// カメラ管理クラス
/// </summary>
class CameraManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CameraManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraManager();

	/// <summary>
	/// カメラ情報を更新する
	/// </summary>
	void Update();
	/// <summary>
	/// 終了処理を行う
	/// </summary>
	void End();

	/// <summary>
	/// 指定した種類のカメラを有効化する
	/// </summary>
	/// <param name="kind">カメラの種類</param>
	void AppInfo(CameraKind kind);

	/// <summary>
	/// 指定した種類のカメラを取得する
	/// </summary>
	/// <param name="kind">カメラの種類</param>
	/// <returns>指定した種類のカメラ</returns>
	std::shared_ptr<Camera> GetCamera(CameraKind kind) const { return m_cameras[static_cast<size_t>(kind)]; }

private:
	// カメラの配列
	std::array<std::shared_ptr<Camera>, static_cast<size_t>(CameraKind::MAX)> m_cameras;
};