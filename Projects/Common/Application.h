#pragma once
#include <cstdio>
#include "Vec3.h"

struct Light
{
    Vec3 pos;
    float range = 0.0f;
    float intensity = 0.0f;
    float color[3] = { 0.0f };
};
struct LightBuffer
{
	Light lights[50];
    int num = 0;
    float dummy[3] = { 0.0f };
};

/// <summary>
/// アプリケーション全体を管理するクラス
/// </summary>
class Application final
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Application();

    Application(const Application&) = delete;
    Application(const Application&&) = delete;
    Application operator= (const Application&) = delete;
    Application operator= (const Application&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>Applicationのインスタンス</returns>
    static Application& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <returns>true: 成功/ false:失敗</returns>
    bool Init();
    /// <summary>
    /// ゲームの実行
    /// </summary>
    void Run();
    /// <summary>
    /// 終了処理
    /// </summary>
    void Terminate();

    /// <summary>
    /// ゲームを終了させる
    /// </summary>
    void OnEnd() { m_isEnd = true; }

    /// <summary>
	/// ライトバッファーを設定する
    /// </summary>
    /// <param name="ligths">ライトの情報</param>
    /// <param name="lightNum">ライトの数</param>
    void SetLightBuffer(Light* ligths, int lightNum);
	/// <summary>
	/// ライトバッファーのハンドルを取得
	/// </summary>
	/// <returns>ライトバッファーハンドル</returns>
	int GetLigthBufferH() const { return m_lightBufferH; }

private:
    /// <summary>
    /// マネージャーの初期化
    /// </summary>
    void InitManager();
    /// <summary>
    /// マネージャーの終了処理
    /// </summary>
    void EndManager();
#ifdef _DEBUG
    /// <summary>
    /// デバッグ用の時間表示
    /// </summary>
    void ShowTime() const;
#endif

private:
	// ライトバッファー
	LightBuffer* m_lightBuffer;
    // ライトバッファーハンドル
	int m_lightBufferH;

    // ゲーム終了フラグ
    bool m_isEnd;      
    // FullWindowsフラグ
    bool m_isWindows;  

#ifdef _DEBUG
    FILE* m_in;   // デバッグ用入力ファイル
    FILE* m_out;  // デバッグ用出力ファイル
#endif
};