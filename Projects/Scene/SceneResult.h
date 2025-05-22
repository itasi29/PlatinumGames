#pragma once
#include "SceneBase.h"
#include <array>
#include <memory>

class StageManager;
class AnimController;

/// <summary>
/// リザルトシーンクラス
/// </summary>
class SceneResult : public SceneBase
{
private:
    /// <summary>
    /// 時間の決定を表す列挙型
    /// </summary>
    enum class DecideTime
    {
        DEC, // 10分の1秒
        SEC, // 秒
        MIN, // 分
        MAX
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <param name="clearTime">クリア時間</param>
    SceneResult(const int stageNo, const int clearTime);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneResult() {}

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
    /// <summary>
    /// 時間の更新処理
    /// </summary>
    void TimeUpdate();

    /// <summary>
    /// ランクの更新処理
    /// </summary>
    void RankUpdate();

    /// <summary>
    /// ランキングの更新処理
    /// </summary>
    void RankingUpdate();

    /// <summary>
    /// PadUIのフェード更新処理
    /// </summary>
    void PadUIFadeUpdate();

    /// <summary>
    /// 選択の更新処理
    /// </summary>
    void SelectUpdate();

    /// <summary>
    /// 時間の描画処理
    /// </summary>
    void DrawTime() const;

    /// <summary>
    /// 時間の描画処理
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="kind">時間の種類</param>
    /// <param name="time">時間</param>
    /// <param name="color">色</param>
    void DrawTime(int x, int y, DecideTime kind, int time, unsigned int color) const;

    /// <summary>
    /// ランクの描画処理
    /// </summary>
    void DrawRank() const;

    /// <summary>
    /// ランキング更新の描画処理
    /// </summary>
    void DrawRankingUpdate() const;

    /// <summary>
    /// PadUIの描画処理
    /// </summary>
    void DrawPadUI() const;

    /// <summary>
    /// スタンプの描画処理
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="fileId">ファイルID</param>
    /// <param name="defSize">デフォルトサイズ</param>
    /// <param name="largeSize">大きいサイズ</param>
    /// <param name="largeSize2">さらに大きいサイズ</param>
    /// <param name="smallSize">小さいサイズ</param>
    /// <param name="count">カウント</param>
    /// <param name="isStamped">スタンプが押されているかどうか</param>
    void DrawStamp(int x, int y, int fileId, float defSize, float largeSize, float largeSize2, float smallSize, int count, bool isStamped) const;

private:
    // ステージ名
    const int m_stageNo; 
    // クリア時間
    const int m_clearTime; 

    using ProcessUpdate_t = void(SceneResult::*)();
    ProcessUpdate_t m_processFunc; // 更新関数ポインタ

    std::shared_ptr<StageManager> m_stageMgr;   // ステージマネージャー
    std::shared_ptr<AnimController> m_anim;     // アニメーションコントローラー

    // 移動カウント
    std::array<int, static_cast<size_t>(DecideTime::MAX)> m_moveCount[3]; 
    // 決定インデックス
    int m_indexDecide; 
    // カウント
    int m_count; 
    // 頂点シェーダーハンドル
    int m_vsH; 
    // ピクセルシェーダーハンドル
    int m_psH; 

    std::array<bool, static_cast<size_t>(DecideTime::MAX)> m_isDecide[3];   // 決定フラグ
    std::array<bool, static_cast<size_t>(DecideTime::MAX)> m_isMove[3];     // 移動フラグ
    bool m_isAllDecide; // 全て決定されたかどうか
    bool m_isAllMoved;  // 全て移動されたかどうか
    bool m_isRankStamped;    // ランクがスタンプされたかどうか
    bool m_isRankingStamped; // ランキングがスタンプされたかどうか
    bool m_isRankingUpdate;  // ランキングが更新されたかどうか
	bool m_isExistNext; // 次のステージが存在するか
};