#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class GateManager;
class StageManager;
class CameraManager;
class UIMoveData;
class Player;
class Camera;
enum class CameraKind;

/// <summary>
/// タイトルシーンクラス
/// </summary>
class SceneTitle : public SceneBase
{
private:
    /// <summary>
    /// シーンの状態を表す列挙型
    /// </summary>
    enum class State
    {
        MENU,               // メニュー
        RANKING,            // ランキング
        SELECT_SAVE_DATA,   // セーブデータ選択
        DECIDE_SAVE_DATA,   // セーブデータ決定
    };

    /// <summary>
    /// シェーダー用の定数バッファ
    /// </summary>
    struct CBuff
    {
        float frame;
        float fireRed;
        float fireGreen;
        float fireBlue;
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneTitle();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneTitle() {}

    /// <summary>
    /// 初期化処理(非同期)
    /// </summary>
    void AsyncInit() override;

    /// <summary>
    /// 初期化処理(同期)
    /// </summary>
    void Init() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 更新処理(同期)
    /// </summary>
    /// <param name="isFade">フェード中かどうか</param>
    void Update(bool isFade) override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const override;

private:
    /// <summary>
    /// ステージ情報の更新
    /// </summary>
    void StageUpdate();

    /// <summary>
    /// メニュー(セーブデータ・オプション・終了)の選択
    /// </summary>
    void SelectMenuUpdate();
    /// <summary>
    /// ランキングの更新
    /// </summary>
    void RankingUpdate();
    /// <summary>
    /// どのセーブデータを使うかの選択
    /// </summary>
    void SelectSaveDataUpdate();

    /// <summary>
    /// メニュー選択時の処理
    /// </summary>
    void OnSelectMenu();
    /// <summary>
    /// ランキング選択時の処理
    /// </summary>
    void OnRanking();
    /// <summary>
    /// セーブデータ選択時の処理
    /// </summary>
    void OnSelectSaveData();

    /// <summary>
    /// メニュー選択時の描画
    /// </summary>
    void DrawSelectMenu() const;
    /// <summary>
    /// ランキングの描画
    /// </summary>
    void DrawRanking() const;
    /// <summary>
    /// セーブデータ選択時の描画
    /// </summary>
    void DrawSelectSaveData() const;
    /// <summary>
    /// セーブデータ情報の描画
    /// </summary>
    /// <param name="saveNo">セーブデータ番号</param>
    void DrawSaveInfo(int saveNo) const;
    /// <summary>
    /// PadUIの描画
    /// </summary>
    void DrawPadUI() const;

    /// <summary>
    /// ゲームの開始
    /// </summary>
    void OnStart();

private:
    using UpdateFunc_t = void(SceneTitle::*)();
    UpdateFunc_t m_updateFunc;  // 更新関数ポインタ
    using DrawFunc_t = void(SceneTitle::*)() const;
    DrawFunc_t m_drawFunc;      // 描画関数ポインタ

    // ステージマネージャーのリスト
    std::vector<std::shared_ptr<StageManager>> m_stageMgrs; 
    // カメラマネージャー
    std::shared_ptr<CameraManager> m_cameraMgr; 
    std::shared_ptr<Camera> m_nowCamera;  // 現在のカメラ
    std::shared_ptr<Camera> m_nextCamera; // 次のカメラ

    // 現在のシーンの状態
    State m_state; 

    std::shared_ptr<UIMoveData> m_mainUI;       // メインUI
    std::shared_ptr<UIMoveData> m_saveUI;       // セーブUI
    std::shared_ptr<UIMoveData> m_saveInfoUI;   // セーブ情報UI
    std::shared_ptr<UIMoveData> m_rankingUI;    // ランキングUI

    CBuff* m_cBuff; // シェーダー用の定数バッファ
    int m_cBuffH; // 定数バッファハンドル

    int m_cameraRt1; // カメラ描画先RT1
    int m_cameraRt2; // カメラ描画先RT2
    int m_menuCurrent;      // メニューの選択カーソル
    int m_saveDataNo;       // セーブデータ番号
    int m_rankingCurrent;   // ランキング番号
    int m_stageIndex;       // ステージインデックス
    int m_fadeSaveInfoFrame;    // セーブ情報関係のフェードフレーム
    int m_fadeSaveNoFrame;      // セーブデータ番号変更フェード
	int m_uiCount;              // UIのカウント  
	int m_cameraFrame;          // カメラのフレーム
    int m_stageChangeFrame;     // ステージ変更フレーム
	int m_erroeCount;           // エラーカウント
	float m_stageFade;          // ステージのフェード

    // セーブデータ番号変更フェードフラグ
    bool m_isFadeSaveNo; 
    // 現在選択がメニューかどうか
    bool m_isNowSelectMenu;
    // 現在選択がセーブデータ番号選択かどうか
    bool m_isNowSelectSaveData; 
	// 新しくデータを作成するかどうか
    bool m_isNewData;
    // 選択エラー
	bool m_isError; 
	// 選択エラー時の揺れ
    bool m_isErrorShake;
    // ステージ変更フェードフラグ
    bool m_isStageChangeFade; 
};