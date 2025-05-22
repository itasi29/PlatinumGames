#pragma once
#include "SceneBase.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <Input.h>

class UIMoveData;
class SceneBase;

/// <summary>
/// オプションシーンクラス
/// </summary>
class SceneOption : public SceneBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneOption();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneOption() {}

    /// <summary>
    /// 初期化処理(非同期)
    /// </summary>
    void AsyncInit() override;

    /// <summary>
    /// 初期化処理(同期)
    /// </summary>
    void Init() override;

    /// <summary>
    /// 初期化処理(同期) - シーンを指定
    /// </summary>
    /// <param name="scene">シーン</param>
    void Init(SceneBase* scene) { m_scene = scene; }

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

    /// <summary>
    /// 開いたシーンの種類をチェック
    /// </summary>
    /// <param name="openScene">開いたシーンの種類</param>
    void CheckOpenScene(SceneKind openScene);

private:
    /// <summary>
    /// フェードインの更新処理
    /// </summary>
    void FadeInUpdate();

    /// <summary>
    /// フェードアウトの更新処理
    /// </summary>
    void FadeOutUpdate();

    /// <summary>
    /// メインタイトルの更新処理
    /// </summary>
    void MainTitleUpdate();

    /// <summary>
    /// ステージセレクトの更新処理
    /// </summary>
    void MainStageSelectUpdate();

    /// <summary>
    /// メインの更新処理
    /// </summary>
    void MainMainUpdate();

    /// <summary>
    /// サウンド設定の更新処理
    /// </summary>
    void SoundUpdate();

    /// <summary>
    /// カメラ設定の更新処理
    /// </summary>
    void CameraUpdate();

    /// <summary>
    /// 再スタートの更新処理
    /// </summary>
    void RestartUpdate();

    /// <summary>
    /// ステージセレクトに戻る更新処理
    /// </summary>
    void ReturnStageSelectUpdate();

    /// <summary>
    /// サブ描画の変更
    /// </summary>
    void ChangeSubDraw();

    /// <summary>
    /// メニューを開く
    /// </summary>
    void OnMenu();

    /// <summary>
    /// フェードアウトを開始
    /// </summary>
    void OnFadeOut();

    /// <summary>
    /// サウンド設定を開く
    /// </summary>
    void OnSound();

    /// <summary>
    /// カメラ設定を開く
    /// </summary>
    void OnCamera();

    /// <summary>
    /// 再スタートを行う
    /// </summary>
    void OnRestart();

    /// <summary>
    /// ステージセレクトに戻る
    /// </summary>
    void OnReturnStageSelect();

    /// <summary>
    /// フェード描画
    /// </summary>
    void DrawFade() const;

    /// <summary>
    /// 共通描画
    /// </summary>
    void DrawCommon() const;

    /// <summary>
    /// サウンド設定の描画
    /// </summary>
    void DrawSound() const;

    /// <summary>
    /// カメラ設定の描画
    /// </summary>
    void DrawCamera() const;

    /// <summary>
    /// 再スタートの描画
    /// </summary>
    void DrawRestart() const;

    /// <summary>
    /// ステージセレクトに戻る描画
    /// </summary>
    void DrawReturnStageSelect() const;

    /// <summary>
    /// クローズ描画
    /// </summary>
    void DrawClose() const;

    /// <summary>
    /// サウンドバーの描画
    /// </summary>
    /// <param name="y">Y座標</param>
    /// <param name="volume">音量</param>
    void DrawSoundBar(int y, int volume) const;

    /// <summary>
    /// サブフレームの描画
    /// </summary>
    void DrawSubFrame() const;

    /// <summary>
    /// PadUIの描画
    /// </summary>
    void DrawPadUI() const;

private:
    using UpdateFunc_t = void(SceneOption::*)();
    UpdateFunc_t m_updateFunc;  // 更新関数ポインタ
    using DrawFunc_t = void(SceneOption::*)() const;
    DrawFunc_t m_drawFunc;      // 描画関数ポインタ
    DrawFunc_t m_subDrawFunc;   // サブ描画関数ポインタ

    // 現在のシーン
    SceneBase* m_scene; 

    // シーン別文字列リスト
    std::unordered_map<SceneKind, std::vector<std::wstring>> m_selectStrList; 

    std::shared_ptr<UIMoveData> m_mainUI; // メインUI
    std::shared_ptr<UIMoveData> m_subUI;  // サブUI

    // 開いたシーンの種類
    SceneKind m_openScene; 
    // 前回のリピート種類
    RepeatKind m_preRepeatKind; 

    // メインの現在の選択肢
    int m_mainCurrent; 
    // サブの現在の選択肢
    int m_subCurrent; 
    // UIカウント
    int m_uiCount; 

    // フェードカウント
    float m_fadeCount; 

    // 現在メニューを開いているかどうか
    bool m_isNowMenu; 
};