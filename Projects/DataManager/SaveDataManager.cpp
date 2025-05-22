#include "SaveDataManager.h"
#include <DxLib.h>
#include <cassert>
#include "StringUtility.h"
#include "StageDataManager.h"
#include "SoundManager.h"

namespace
{
	// セーブデータの数
	constexpr int SAVE_DATA_NUM = 3;

	// セーブデータのパス
	const char* const FILE_PATH = "Data/SaveData/SaveData.dat";

	constexpr int MAX_BGM_RATE = 100;	// 最大BGM割合
	constexpr int MAX_SE_RATE = 100;	// 最大SE割合
	constexpr int MAX_CAMERA_SENS = 100;	// 最大カメラ感度
}

SaveDataManager::SaveDataManager() :
	m_useSaveNo(0),
	m_bgmVolume(MAX_BGM_RATE),
	m_seVolume(MAX_SE_RATE)
{
}

SaveDataManager& SaveDataManager::GetInstance()
{
	static SaveDataManager instance;
	return instance;
}

void SaveDataManager::Load()
{
	SetUseASyncLoadFlag(false);
	auto path = StringUtility::StringToWString(FILE_PATH);
	int handle = FileRead_open(path.c_str());

	// ファイルが存在しない場合はデフォルトのデータにする
	if (handle == 0)
	{
		m_dataTable.resize(SAVE_DATA_NUM);
		for (int i = 0; i < SAVE_DATA_NUM; ++i)
		{
			Initialized(i);
			m_dataTable.at(i).cameraSens = static_cast<int>(MAX_CAMERA_SENS * 0.5f);
		}
	}
	// ファイルが存在する場合はファイルからデータを読み込む
	else
	{
		LoadFile(handle);
		// ファイルを閉じる
		FileRead_close(handle);
	}
	// 新規解放状況の初期化
	m_isNewRelease.resize(m_dataTable.at(0).isClear.size(), false);
	SetUseASyncLoadFlag(true);
}

void SaveDataManager::Save() const
{
	// ファイルオープン
	FILE* fp;
	auto err = fopen_s(&fp, FILE_PATH, "wb");
	if (err != 0)
	{
		assert(false && "セーブファイルを開くのに失敗しました");
		return;
	}

	// BGMの音量を書き込み
	fwrite(&m_bgmVolume, sizeof(int), 1, fp);
	// SEの音量を書き込み
	fwrite(&m_seVolume, sizeof(int), 1, fp);
	// ステージ数取得(タイトル・リザルト数分減らす)
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageNum = stageDataMgr.GetStageNum() - 2;
	for (const auto& data : m_dataTable)
	{
		// カメラの感度を書き込み
		fwrite(&data.cameraSens, sizeof(int), 1, fp);
		// 存在フラグの書き込み
		fwrite(&data.isExist, sizeof(bool), 1, fp);
		// 総プレイ時間の書き込み
		fwrite(&data.totalTime, sizeof(int), 1, fp);
		// クリア情報の書き込み
		for (int i = 0; i < stageNum; ++i)
		{
			// クリア情報の書き込み
			bool isClear = data.isClear.at(i);
			fwrite(&isClear, sizeof(bool), 1, fp);
			// ステージクリアタイムを書き込み
			fwrite(&data.clearTime.at(i), sizeof(int), 1, fp);
		}
	}

	// ファイルクローズ
	fclose(fp);
}

void SaveDataManager::Initialized(int no)
{
	auto& data = m_dataTable[no];
	// セーブデータを存在しないに
	data.isExist = false;
	// プレイ時間の初期化
	data.totalTime = 0;
	// ステージ数分繰り返す(タイトル・リザルト数分減らす)
	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto& stageNum = stageDataMgr.GetStageNum() - 2;
	// クリア情報の初期化
	data.isClear.clear();
	data.clearTime.clear();
	for (int i = 0; i < stageNum; ++i)
	{
		data.isClear.push_back(false);
		data.clearTime.push_back(99 * 3600 + 59 * 60 + 59);
	}
}

void SaveDataManager::ChangeBgmRate(int changeSize)
{
	// BGMの音量の変更
	m_bgmVolume = std::min<int>(std::max<int>(m_bgmVolume + changeSize, 0), MAX_BGM_RATE);
	// 変更した音量を適用
	SoundManager::GetInstance().ChangePlayBgmVolume();
}

void SaveDataManager::ChangeSeRate(int changeSize)
{
	// SEの音量の変更
	m_seVolume = std::min<int>(std::max<int>(m_seVolume + changeSize, 0), MAX_SE_RATE);
}

void SaveDataManager::ChangeCameraSens(int changeSize)
{
	// カメラ感度の変更
	m_dataTable.at(m_useSaveNo).cameraSens = std::min<int>(std::max<int>(m_dataTable.at(m_useSaveNo).cameraSens + changeSize, 0), 100);
}

void SaveDataManager::OnClear(int stageNo, int clearTime)
{
	auto& data = m_dataTable.at(m_useSaveNo);

	// クリア時間が保存されているプレイ時間を超えている場合は更新
	if (data.clearTime.at(stageNo) > clearTime) data.clearTime.at(stageNo) = clearTime;
	// クリアしていない場合
	if (!data.isClear.at(stageNo))
	{
		// 次のステージが存在する場合は新規解放とする
		if (stageNo + 1 < m_isNewRelease.size())
		{
			m_isNewRelease.at(stageNo + 1) = true;
		}
	}
	// クリアしたことに
	data.isClear.at(stageNo) = true;
	// データが存在することに
	data.isExist = true;
	// トータルタイムの更新
	data.totalTime += clearTime;
}

void SaveDataManager::ResetNewRelease()
{
	// 全部falseにする
	for (int i = 0; i < m_isNewRelease.size(); ++i) m_isNewRelease.at(i) = false;
}

int SaveDataManager::GetSaveDataNum() const
{
	return SAVE_DATA_NUM;
}

int SaveDataManager::GetClearStageNum(int saveNo) const
{
	int count = 0;
	for (const auto& isClear : m_dataTable.at(saveNo).isClear)
	{
		// クリアしていなければ抜ける
		if (!isClear) break;
		// カウント増加
		++count;
	}
	return count;
}

bool SaveDataManager::IsReleaseStage(int stageNo) const
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	// ステージが1未満なら解放していることに(0番はタイトルの情報)
	if (stageNo < 1) return true;
	// それ以外は一つ前のステージがクリアしていたら解放していることにする
	return m_dataTable.at(m_useSaveNo).isClear.at(stageNo - 1);
}

void SaveDataManager::LoadFile(int handle)
{
	auto& stageDataMgr = StageDataManager::GetInstance();

	// BGMの音量を読み込み
	FileRead_read(&m_bgmVolume, sizeof(int), handle);
	// SEの音量を読み込み
	FileRead_read(&m_seVolume, sizeof(int), handle);

	// ステージ数取得(タイトル・リザルト分減らす)
	const auto& stageNum = stageDataMgr.GetStageNum() - 2;
	for (int i = 0; i < SAVE_DATA_NUM; ++i)
	{
		SaveData data;
		// カメラの感度を読み込み
		FileRead_read(&data.cameraSens, sizeof(int), handle);
		// 存在フラグを読み込み
		FileRead_read(&data.isExist, sizeof(bool), handle);
		// プレイ時間の読み込み
		FileRead_read(&data.totalTime, sizeof(int), handle);
		// サイズ分確保
		data.isClear.resize(stageNum);
		data.clearTime.resize(stageNum);
		for (int j = 0; j < stageNum; ++j)
		{
			// クリア情報読み込み
			bool isClear;
			FileRead_read(&isClear, sizeof(bool), handle);
			data.isClear[j] = isClear;
			// ステージクリアタイム読み込み
			FileRead_read(&data.clearTime[j], sizeof(int), handle);
		}

		m_dataTable.emplace_back(data);
	}
}
