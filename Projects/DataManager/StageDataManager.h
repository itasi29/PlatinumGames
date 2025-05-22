#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Vec3.h"
#include "ObjectTag.h"

/// <summary>
/// ランクの種類を表す列挙型
/// </summary>
enum class RankKind
{
    S,
    A,
    B,
    C,
};

/// <summary>
/// ステージデータを格納する構造体
/// </summary>
struct StageData
{
    std::unordered_map<RankKind, int> rankTime;  // ランクごとのクリアタイム
    bool isOneHnad = false;  // 片手ステージかどうか
    Vec3 gatePos;  // ゲートの座標
    Vec3 gateNorm;  // ゲートの法線
    Vec3 gateDir;  // ゲートの上向き
    ObjectTag gateTag;  // ゲートのタグ
};

using StageDataTable_t = std::vector<StageData>;
using StageNameTable_t = std::vector<std::wstring>;

/// <summary>
/// ステージデータ管理クラス
/// </summary>
class StageDataManager final
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    StageDataManager();

	StageDataManager(const StageDataManager&) = delete;
	StageDataManager(const StageDataManager&&) = delete;
	StageDataManager& operator= (const StageDataManager&) = delete;
	StageDataManager& operator= (const StageDataManager&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>StageDataManagerのインスタンス</returns>
    static StageDataManager& GetInstance();

    /// <summary>
    /// ステージデータの読み込み
    /// </summary>
    void Load();

    /// <summary>
    /// ステージ数の取得
    /// </summary>
    /// <returns>ステージ数</returns>
    int GetStageNum() const { return static_cast<int>(m_nameTable.size()); }
    /// <summary>
    /// ステージ名を取得する
    /// </summary>
    /// <param name="no">ステージ番号</param>
    /// <returns>ステージ名</returns>
    const wchar_t* const GetStageName(int no) const { return m_nameTable.at(no).c_str(); }
    /// <summary>
    /// 自動生成ゲートの座標を取得する
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <returns>座標</returns>
    const Vec3& GetGatePos(int stageNo) const { return m_dataTable.at(stageNo).gatePos; }
    /// <summary>
    /// 自動生成ゲートの法線を取得する
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <returns>法線</returns>
    const Vec3& GetGateNorm(int stageNo) const { return m_dataTable.at(stageNo).gateNorm; }
    /// <summary>
    /// 自動生成ゲートの上向きを取得する
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <returns>上向き</returns>
    const Vec3& GetGateDir(int stageNo) const { return m_dataTable.at(stageNo).gateDir; }
    /// <summary>
    /// 自動生成ゲートの触れているタグを取得する
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <returns>タグ</returns>
    const ObjectTag& GetGateObjectTag(int stageNo) const { return m_dataTable.at(stageNo).gateTag; }
    /// <summary>
    /// ステージ番号を取得する
    /// </summary>
    /// <param name="stageName">ステージ名</param>
    /// <returns>ステージ番号</returns>
    int GetStageNo(const wchar_t* const stageName) const;
    /// <summary>
    /// ランクタイムを取得する
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <param name="kind">ランクの種類</param>
    /// <returns>ランクタイム</returns>
    int GetTimeRank(int stageNo, const RankKind& kind) const { return m_dataTable.at(stageNo).rankTime.at(kind); }
    /// <summary>
    /// ランクを取得
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <param name="time">タイム</param>
    /// <returns>ランク</returns>
    RankKind GetRank(int stageNo, int time) const;
    /// <summary>
    /// ワープゲート銃片手ステージフラグ
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    /// <returns>true:片手ステージ /false:両手ステージ</returns>
    bool IsOneHand(int stageNo) const { return m_dataTable.at(stageNo).isOneHnad; }

private:
    // ステージデータのテーブル
    StageDataTable_t m_dataTable;  
    // ステージ名のテーブル
    StageNameTable_t m_nameTable;  
};