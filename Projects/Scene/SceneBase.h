#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

class SceneManager;
class FileBase;
class Input;

/// <summary>
/// シーンの種類
/// </summary>
enum class SceneKind
{
    TITLE,          // タイトル
    STAGE_SELECT,   // ステージセレクト
    MAIN,           // プレイ画面
    TUTORIAL,       // チュートリアル画面
    CLEAR,          // クリア画面
    OPTION,         // オプション
    DEBUG,          // デバッグ
    MAX
};

/// <summary>
/// シーンの基底クラス
/// </summary>
class SceneBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="kind">シーンの種類</param>
    SceneBase(SceneKind kind);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~SceneBase() {}

    /// <summary>
    /// シーンに入る時の初期化処理(非同期で行うもの)
    /// </summary>
    virtual void AsyncInit() = 0;

    /// <summary>
    /// シーンに入るときの初期化処理(同期処理で行うの)
    /// </summary>
    virtual void Init() = 0;

    /// <summary>
    /// シーンを抜けるときの処理
    /// </summary>
    virtual void End() = 0;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="isFade">true:フェードしている /false:フェードしていない</param>
    virtual void Update(bool isFade) = 0;

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw() const = 0;

    /// <summary>
    /// 現在のシーンの種類を取得
    /// </summary>
    /// <returns>シーンの種類</returns>
    SceneKind GetNowKind() const { return m_nowSceneKind; }

protected:
    SceneManager& m_scnMgr; ///< シーンマネージャーの参照
    std::unordered_map<int, std::shared_ptr<FileBase>> m_files; ///< ファイル保存リスト

private:
    SceneKind m_nowSceneKind; ///< 現在のシーンの種類
};