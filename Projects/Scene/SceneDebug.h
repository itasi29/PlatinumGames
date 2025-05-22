#pragma once
#ifdef _DEBUG
#include "SceneBase.h"

/// <summary>
/// デバッグシーンクラス
/// </summary>
class SceneDebug final : public SceneBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SceneDebug();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneDebug() {}

    /// <summary>
    /// シーンに入るときの初期化処理(非同期)
    /// </summary>
    void AsyncInit() override;

    /// <summary>
    /// シーンに入るときの初期化処理(同期)
    /// </summary>
    void Init() override;

    /// <summary>
    /// シーンを抜けるときの処理
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
    int m_next;    ///< 次のシーンへの遷移フラグ
    int m_stageNo; ///< ステージ番号
};
#endif