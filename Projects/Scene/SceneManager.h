#pragma once
#include <memory>
#include <unordered_map>

class SceneBase;
class SceneMain;
class FileBase;
enum class SceneKind;

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneManager();

	SceneManager(const SceneManager&) = delete;
	SceneManager(const SceneManager&&) = delete;
	SceneManager operator= (const SceneManager&) = delete;
	SceneManager operator= (const SceneManager&&) = delete;

public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~SceneManager();

    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>SceneManagerのインスタンス</returns>
    static SceneManager& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Init();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const;

    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// シーンの変更
    /// </summary>
    /// <param name="next">次のシーン</param>
    void Change(const std::shared_ptr<SceneBase>& next);

    /// <summary>
    /// 流すBgmを変更する
    /// </summary>
    /// <param name="handle">流すBgmのハンドル</param>
    void ChangeBgmH(int handle);

    /// <summary>
    /// オプションを開く
    /// </summary>
    /// <param name="openScene">開いたシーンの種類</param>
    /// <param name="scene">シーン内で必要な処理がある場合に渡す用</param>
    void OpenOption(SceneKind openScene, SceneBase* scene = nullptr);

    /// <summary>
    /// オプションorポーズを閉じる
    /// </summary>
    void CloseOption();

    /// <summary>
    /// オプションを開いているか
    /// </summary>
    /// <returns>true: 開いている /false: 開いていない</returns>
    bool IsOpenOption() const { return m_option != nullptr; }

    /// <summary>
    /// フェードが終了したフレームか
    /// </summary>
    /// <returns>true: 終了したフレーム /false: それ以外のフレーム</returns>
    bool IsFadeEndFrame() const { return m_isFadeEndFrame; }

    /// <summary>
    /// 現在のシーンの種類を取得
    /// </summary>
    /// <returns>シーンの種類</returns>
    SceneKind GetNowSceneKind() const;

private:
    using UpdateFunc_t = void(SceneManager::*)();
    /// <summary>
    /// 通常の更新処理
    /// </summary>
    void NormalUpdate();

    /// <summary>
    /// フェードアウト処理
    /// </summary>
    void FadeOut();

    /// <summary>
    /// ファイル読み込み中の更新処理
    /// </summary>
    void FileLoadingUpdate();

    /// <summary>
    /// フェードイン処理
    /// </summary>
    void FadeIn();

    using DrawFunc_t = void(SceneManager::*)() const;
    /// <summary>
    /// 通常の描画処理
    /// </summary>
    void DrawNormal() const;

    /// <summary>
    /// フェード中の描画処理
    /// </summary>
    void DrawFade() const;

    /// <summary>
    /// ファイル読み込み中の描画処理
    /// </summary>
    void FileLoadingDraw() const;

private:
    UpdateFunc_t m_updateFunc;  // 更新関数ポインタ
    DrawFunc_t m_drawFunc;      // 描画関数ポインタ

    std::shared_ptr<SceneBase> m_option;    // オプションシーン
    std::shared_ptr<SceneBase> m_scene;     // 現在のシーン
    std::shared_ptr<SceneBase> m_nextScene; // 次のシーン

    // ファイル保存リスト
    std::unordered_map<int, std::shared_ptr<FileBase>> m_files; 

    // 流しているBGMのハンドル
    int m_bgmH; 

    // フェードアウトに使うカウント
    float m_count; 
    // フェードアウトに使う角度速度
    float m_angleSpeed; 
    // フェードが終了したフレームかどうか
    bool m_isFadeEndFrame; 
};
