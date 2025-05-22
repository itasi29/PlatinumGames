#include "FileManager.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include "FileBase.h"
#include "FileImage.h"
#include "FileSound.h"
#include "FileModel.h"
#include "FileEffekseer.h"
#include "FileShader.h"
#include "Utility/StringUtility.h"

namespace
{
	// マスターファイルパス
	const wchar_t* const PATH_FILE_MASTER = L"Data/Master/FileData.csv";
	const wchar_t* const MODEL_SHADER_MASTER = L"Data/Master/ModelShaderData.csv";
	// ファイル要素番号
	const enum FileIndex : int
	{
		TYPE,
		PATH,
		EXTENSION
	};
	// ロードするファイルの種類
	const std::vector<LoadType> TO_TYPE =
	{
		{ LoadType::MODEL },
		{ LoadType::PIXEL_SHADER },
		{ LoadType::VERTEX_SHADER },
		{ LoadType::IMAGE },
		{ LoadType::SOUND },
		{ LoadType::EFFEKSEER },
	};
}

FileManager::FileManager() :
	m_isEnd(false)
{
}

FileManager::~FileManager()
{
}

FileManager& FileManager::GetInstance()
{
	static FileManager mgr;
	return mgr;
}

void FileManager::Init()
{
	SetUseASyncLoadFlag(true);
	m_isEnd = false;
	// マスターファイル読み込み
	LoadPath();
	LoadModelShader();
}

void FileManager::End()
{
	if (m_isEnd) return;

	// ファイルを削除
	for (auto& file : m_file)
	{
		file.second->Delete();
		file.second->End();
	}

	m_isEnd = true;
}

CsvData_t FileManager::LoadCsv(const wchar_t* const path)
{
	CsvData_t data;

	// ファイル読み込み
	std::ifstream ifs(path);
	if (!ifs)
	{
		assert(false);
		return data;
	}
	// バッファー
	std::string strBuf;
	// データすべて読み込み
	while (std::getline(ifs, strBuf))
	{
		data.push_back(StringUtility::Split(strBuf, ','));
	}

	return data;
}

std::shared_ptr<FileBase> FileManager::Load(int id, bool isEternal)
{
	auto it = m_file.find(id);

	// 見つかれば参照カウンタを増やして、コピーを返す
	if (it != m_file.end()) 
	{
		auto& file = it->second;
		file->m_refCount++;
		return CastCopyFile(file);
	}

	// 見つかなければ作成し、コピーを返す
	std::shared_ptr<FileBase> file = MakeFile(id);
	m_file.insert(std::make_pair(id, file));
	file->m_id = id;
	file->m_handle = GetHandle(id);
	file->m_refCount++;
	file->m_isEternal = isEternal;

	return CastCopyFile(file);
}

void FileManager::Delete(int id)
{
	// 終了処理が呼ばれていた場合は何もしない
	if (m_isEnd) return;
	// 見つかなければ終了
	if (m_file.find(id) == m_file.end()) return;

	auto& file = m_file.at(id);
	// 永続フラグがtrueなら終了
	if (file->m_isEternal) return;

	// 参照カウンタを減らす
	file->m_refCount--;
	// 見ているものが無くなればデータを消す
	if (file->m_refCount <= 0) 
	{
		file->Delete();
		file->End();
		m_file.erase(id);
	}
}

std::shared_ptr<FileBase> FileManager::MakeFile(int id)
{
	auto type = m_path.at(id).type;

	// タイプに合わせてクラスを作成＆返す
	if (type == LoadType::IMAGE)			return std::make_shared<FileImage>(*this);
	else if (type == LoadType::SOUND) 		return std::make_shared<FileSound>(*this);
	else if (type == LoadType::MODEL)		return std::make_shared<FileModel>(*this);
	else if (type == LoadType::EFFEKSEER)	return std::make_shared<FileEffekseer>(*this);
	else if (type == LoadType::PIXEL_SHADER || type == LoadType::VERTEX_SHADER) return std::make_shared<FileShader>(*this);

	// ここまで来たらおかしい
	assert(false);
	// 動くようにFileGraphを返しておく
	return std::make_shared<FileImage>(*this);
}

std::shared_ptr<FileBase> FileManager::CastCopyFile(std::shared_ptr<FileBase>& file)
{
	auto type = m_path.at(file->m_id).type;

	// タイプに合わせてクラスをキャスト・コピーを返す
	if (type == LoadType::IMAGE)
	{
		auto fileGraph = std::dynamic_pointer_cast<FileImage>(file);
		return std::make_shared<FileImage>(*fileGraph);
	}
	else if (type == LoadType::SOUND) 
	{
		auto fileSound = std::dynamic_pointer_cast<FileSound>(file);
		return std::make_shared<FileSound>(*fileSound);
	}
	else if (type == LoadType::MODEL) 
	{
		auto fileModel = std::dynamic_pointer_cast<FileModel>(file);
		return std::make_shared<FileModel>(*fileModel);
	}
	else if (type == LoadType::EFFEKSEER)
	{
		auto fileEff = std::dynamic_pointer_cast<FileEffekseer>(file);
		return std::make_shared<FileEffekseer>(*fileEff);
	}
	else if (type == LoadType::PIXEL_SHADER || type == LoadType::VERTEX_SHADER)
	{
		auto fileShader = std::dynamic_pointer_cast<FileShader>(file);
		return std::make_shared<FileShader>(*fileShader);
	}

	// ここまで来たらおかしい
	assert(false);
	// 動くようにFileGraphを返しておく
	std::shared_ptr<FileImage> fileGraph = std::dynamic_pointer_cast<FileImage>(file);
	return std::make_shared<FileImage>(*fileGraph);
}

int FileManager::GetHandle(int id) const
{
	int handle = -1;
	auto path = m_path.at(id).path.c_str();
	auto type = m_path.at(id).type;

	// タイプに合わせてロードの仕方を変更
	if (type == LoadType::IMAGE) 				handle = LoadGraph(path);
	else if (type == LoadType::SOUND)			handle = LoadSoundMem(path);
	else if (type == LoadType::MODEL)			handle = MV1LoadModel(path);
	else if (type == LoadType::PIXEL_SHADER)	handle = LoadPixelShader(path);
	else if (type == LoadType::VERTEX_SHADER)	handle = LoadVertexShader(path);
	else if (type == LoadType::EFFEKSEER)
	{
		// MEMO: エフェクシアは非同期ロードだとエラーを吐くため一時的に無効化
		SetUseASyncLoadFlag(false);
		handle = LoadEffekseerEffect(path);
		SetUseASyncLoadFlag(true);
	}
	else 
	{
		assert(false);
	}
	assert(handle != -1);

	return handle;
}

void FileManager::LoadPath()
{
	const auto& csv = LoadCsv(PATH_FILE_MASTER);
	for (auto& item : csv)
	{
		// パスデータを取得
		LoadType type = TO_TYPE.at(std::stoi(item[FileIndex::TYPE]));
		const auto& wPath = StringUtility::StringToWString(item[FileIndex::PATH]);
		m_path.push_back({type, wPath });
	}
}

void FileManager::LoadModelShader()
{
	const auto& csv = LoadCsv(MODEL_SHADER_MASTER);
	for (auto& item : csv)
	{
		const auto& model = std::stoi(item[0]);
		const auto& vs = std::stoi(item[1]);
		const auto& ps = std::stoi(item[2]);

		// モデルシェーダデータを取得
		m_shaderData[model].vs = Load(vs, true)->GetHandle();
		m_shaderData[model].ps = Load(ps, true)->GetHandle();
	}
}
