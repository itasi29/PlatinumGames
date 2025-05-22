#pragma once
#include <unordered_map>
#include <string>
#include <array>

/// <summary>
/// ランキングデータ管理クラス
/// </summary>
class RankingDataManager final
{
public:
    // ランキングデータ数
    static constexpr int RANKING_DATA_NUM = 5;

private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    RankingDataManager();

	RankingDataManager(const RankingDataManager&) = delete;
	RankingDataManager(const RankingDataManager&&) = delete;
	RankingDataManager& operator= (const RankingDataManager&) = delete;
	RankingDataManager& operator= (const RankingDataManager&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>RankingDataManagerのインスタンス</returns>
    static RankingDataManager& GetInstance();

    /// <summary>
    /// ランキングデータの読み込み
    /// </summary>
    void Load();
    /// <summary>
    /// ランキングデータの保存
    /// </summary>
    void Save() const;

    /// <summary>
    /// ランキングの更新確認
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <param name="clearTime">クリアタイム</param>
    /// <returns>true:更新した/ false:更新していない</returns>
    bool CheckRankingUpdate(int stageNo, int clearTime);

    /// <summary>
    /// ランキングタイムの取得
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <param name="rankingNo">ランキング番号</param>
    /// <returns>ランキングタイム</returns>
    int GetTime(const int stageNo, int rankingNo) { return m_localData.at(stageNo).at(rankingNo); }

private:
    /// <summary>
    /// データ初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// ランキングの更新
    /// </summary>
    /// <param name="list">ランキングデータ</param>
    /// <param name="clearTime">クリア時間</param>
    /// <returns>true:更新した/ false:更新していない</returns>
    bool CheckRankingUpdate(std::array<int, RANKING_DATA_NUM>& list, int clearTime);

private:
    using RankingData_t = std::vector<std::array<int, RANKING_DATA_NUM>>;
    RankingData_t m_localData;  // ローカルのランキングデータ
//  RankingData_t m_onlineData; // オンラインのランキングデータ
};
