#include "RankingDataManager.h"
#include <DxLib.h>
#include <cassert>
#include "StringUtility.h"
#include "StageDataManager.h"

namespace
{
	// パス
	const char* const FILE_PATH = "Data/SaveData/RankingData.dat";
	// スタートタイムを設定（10分）
	constexpr int START_TIME = 60 * 60 * 10;
	// スタートタイムの増加値（1分）
	constexpr int ADD_START_TIME = 60 * 60;
}

RankingDataManager::RankingDataManager()
{
}

RankingDataManager& RankingDataManager::GetInstance()
{
    static RankingDataManager instance;
    return instance;
}

void RankingDataManager::Load()
{
	SetUseASyncLoadFlag(false);
	auto path = StringUtility::StringToWString(FILE_PATH);
	int handle = FileRead_open(path.c_str());
	
	// ファイルが存在しない場合
	if (handle == 0)
	{
		// デフォルトのデータにする
		Initialize();
	}
	// ファイルが存在する場合
	else
	{
		auto& stageDataMgr = StageDataManager::GetInstance();
		auto stageNum = stageDataMgr.GetStageNum();
		for (int i = 0; i < stageNum; ++i)
		{
			std::array<int, RANKING_DATA_NUM> list;
			for (int j = 0; j < RANKING_DATA_NUM; ++j)
			{
				FileRead_read(&list[j], sizeof(int), handle);
			}
			m_localData.push_back(list);
		}
		FileRead_close(handle);
	}

	SetUseASyncLoadFlag(true);
}

void RankingDataManager::Save() const
{
	// ファイルオープン
	FILE* fp;
	auto err = fopen_s(&fp, FILE_PATH, "wb");
	if (err != 0)
	{
		assert(false && "セーブファイルを開くのに失敗しました");
		return;
	}

	for (auto& list : m_localData)
	{
		for (auto& time : list)
		{
			fwrite(&time, sizeof(int), 1, fp);
		}
	}
}

bool RankingDataManager::CheckRankingUpdate(int stageNo, int clearTime)
{
	bool isUpdate = false;

	if (CheckRankingUpdate(m_localData.at(stageNo), clearTime)) isUpdate = true;
//	if (CheckRankingUpdate(m_onlineData.at(stageName), clearTime)) isUpdate = true;

    return isUpdate;
}

void RankingDataManager::Initialize()
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	// ステージ数を取得(タイトル・リザルトの数分減らす)
	auto stageNum = stageDataMgr.GetStageNum() - 2;
	// 初期化
	m_localData.resize(stageNum);
	for (int i = 0; i < stageNum; ++i)
	{
		auto& list = m_localData[i];
		for (int j = 0; j < RANKING_DATA_NUM; ++j)
		{
			list[j] = START_TIME + ADD_START_TIME * j;
		}
	}
}

bool RankingDataManager::CheckRankingUpdate(std::array<int, RANKING_DATA_NUM>& list, int clearTime)
{
	for (int i = 0; i < RANKING_DATA_NUM; ++i)
	{
		// i位のタイムがクリアタイムより早い かつ データが初期でない なら次の順位へ
		if (list[i] <= clearTime && list[i] >= 0) continue;

		// 順位の変動を行う
		for (int j = RANKING_DATA_NUM - 1; j > i; --j)
		{
			list[j] = list[j - 1];
		}
		list[i] = clearTime;

		// 処理終了
		return true;
	}

	return false;
}
