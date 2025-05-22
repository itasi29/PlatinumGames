#include "StageDataManager.h"
#include <cassert>
#include "StringUtility.h"
#include "FileManager.h"

namespace
{
	const wchar_t* const FILE_PAHT = L"Data/Master/StageData.csv";
	enum StageDataCsv : int
	{
		NAME,
		TIME_S,
		TIME_A,
		TIME_B,
		IS_ONE_HAND,
		GATE_POS,
		GATE_NORM,
		GATE_DIR,
		GATE_TAG
	};
	const std::unordered_map<std::string, ObjectTag> GATE_TAG_TABLE =
	{
		{ "SystemWall", ObjectTag::WALL },
		{ "Floor", ObjectTag::FLOOR },
		{ "Roof", ObjectTag::ROOF },
	};
	const std::vector<RankKind> ORDER =
	{
		RankKind::S,
		RankKind::A,
		RankKind::B,
	};
}

StageDataManager::StageDataManager()
{
}

StageDataManager& StageDataManager::GetInstance()
{
	static StageDataManager instance;
	return instance;
}

void StageDataManager::Load()
{
	auto& mgr = FileManager::GetInstance();

	auto table = mgr.LoadCsv(FILE_PAHT);
	for (auto& info : table)
	{
		auto& name = info[StageDataCsv::NAME];
		const auto& wName = StringUtility::StringToWString(name);
		m_nameTable.push_back(wName);

		StageData data;
		data.rankTime[RankKind::S] = StringUtility::CsvToTime(info[StageDataCsv::TIME_S]);
		data.rankTime[RankKind::A] = StringUtility::CsvToTime(info[StageDataCsv::TIME_A]);
		data.rankTime[RankKind::B] = StringUtility::CsvToTime(info[StageDataCsv::TIME_B]);
		data.isOneHnad = static_cast<bool>(std::stoi(info[StageDataCsv::IS_ONE_HAND]));
		if (data.isOneHnad)
		{
			data.gatePos = StringUtility::CsvToVec3(info[StageDataCsv::GATE_POS]);
			data.gateNorm = StringUtility::CsvToVec3(info[StageDataCsv::GATE_NORM]);
			data.gateDir = StringUtility::CsvToVec3(info[StageDataCsv::GATE_DIR]);
			data.gateTag = GATE_TAG_TABLE.at(info[StageDataCsv::GATE_TAG]);
		}
		m_dataTable.push_back(data);
	}
}

int StageDataManager::GetStageNo(const wchar_t* const stageName) const
{
	for (int i = 0; i < m_nameTable.size(); ++i)
	{
		const auto& name = m_nameTable.at(i);
		if (name == stageName) return i;
	}
	assert(false && "存在しないステージ名です");
	return -1;
}

RankKind StageDataManager::GetRank(int stageNo, int time) const
{
	for (auto& rank : ORDER)
	{
		const auto& info = m_dataTable.at(stageNo).rankTime.at(rank);
		// タイムがランクタイム未満なら達成していることとし、現在のランクを返す
		if (time <= info) return rank;
	}

	return RankKind::C;
}
