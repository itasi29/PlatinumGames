#include <DxLib.h>
#include <cassert>
#include "FontManager.h"
#include "FileManager.h"
#include "StringUtility.h"

namespace
{
	const wchar_t* const kFontPath = L"Font/Kaisotai-Next-UP-B.otf";
	const wchar_t* const kFontName = L"廻想体 ネクスト UP B";
}

FontManager::FontManager()
{}

FontManager::~FontManager()
{
}

FontManager& FontManager::GetInstance()
{
	static FontManager instance;
	return instance;
}

void FontManager::Init()
{
	enum class FileIndex : int
	{
		NAME,
		PATH,
		EXTENSION
	};

	const wchar_t* const MASTER_PATH = L"Data/Master/FontMaster.csv";
	auto table = FileManager::GetInstance().LoadCsv(MASTER_PATH);
	bool isFirst = true;
	int size = 0;
	int count = 0;
	std::vector<std::wstring> fontNameTable;
	for (auto& data : table)
	{
		// 初めはフォント数を取得
		if (isFirst)
		{
			// フォント数を取得
			size = std::stoi(data[0]);
			m_handle.resize(size);
			isFirst = false;
			continue;
		}
		// 全てのフォントを読み込む
		else if (count < size)
		{
			count++;
			// フォント名・パス取得
			const auto& name = StringUtility::StringToWString(data[static_cast<int>(FileIndex::NAME)]);
			const auto& path = StringUtility::StringToWString(data[static_cast<int>(FileIndex::PATH)]);
			fontNameTable.push_back(name);
			m_fontPathTable.push_back(path);
			// フォント読み込み
			if (AddFontResourceEx(path.c_str(), FR_PRIVATE, NULL) <= 0)
			{
				// フォント読込エラー処理
				MessageBox(NULL, L"フォント読込失敗", L"", MB_OK);
			}
		}
		// 全フォントの生成サイズ取得
		else
		{
			for (int i = 0; i < fontNameTable.size(); ++i)
			{
				// フォントサイズ取得
				int createSize = std::stoi(data[0]);
				// フォントハンドル生成
				m_handle.at(i)[createSize] = CreateFontToHandle(fontNameTable[i].c_str(), createSize, -1);
			}
		}
	}
}

void FontManager::End()
{
	// 作ったフォントデータを全て削除
	for (auto& table : m_handle)
	{
		for (auto& handle : table)
		{
			DeleteFontToHandle(handle.second);
		}
	}

	// フォントファイルの削除
	for (const auto& path : m_fontPathTable)
	{
		if (RemoveFontResourceEx(path.c_str(), FR_PRIVATE, NULL))
		{
		}
		else
		{
			MessageBox(NULL, L"remove failure", L"", MB_OK);
		}
	}
}

int FontManager::GetHandle(int id, int size) const
{
	const auto& table = m_handle.at(id);
	auto it = table.find(size);
	if (it == table.end())
	{
		assert(false && L"そのフォントサイズは存在しません");
		return table.at(32);
	}

	return it->second;
}
