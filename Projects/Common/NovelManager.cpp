#include "NovelManager.h"
#include <DxLib.h>
#include "Input.h"
#include "CommandList.h"
#include "Novel.h"
#include "Files.h"
#include "FileManager.h"
#include "Utility/StringUtility.h"

namespace
{
	const wchar_t* const PATH = L"Data/Master/NovelData.csv";
	enum class FileIndex
	{
		IS_NEXT,
		NEXT_ID,
		IS_GRAPH,
		GRAPH_ID,
		DRAW_INTERVAL,
		STR,
	};
}

NovelManager::NovelManager() :
	m_novel(std::make_shared<Novel>()),
	m_nowId(-1),
	m_isNowStart(false),
	m_isStop(true)
{
}

NovelManager::~NovelManager()
{
}


NovelManager& NovelManager::GetInstance()
{
	static NovelManager instance;
	return instance;
}

void NovelManager::Load()
{
	auto& fileMgr = FileManager::GetInstance();
	const auto& table = fileMgr.LoadCsv(PATH);
	for (auto& item : table) 
	{
		Info info;
		// 連続フラグ取得
		info.isNext = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_NEXT))));
		if (info.isNext)
		{
			// 次のIDを取得
			info.nextId = std::stoi(item.at(static_cast<int>(FileIndex::NEXT_ID)));
		}
		// 画像フラグ取得
		info.isGraph = static_cast<bool>(std::stoi(item.at(static_cast<int>(FileIndex::IS_GRAPH))));
		if (info.isGraph)
		{
			// 画像IDを取得
			info.graphId = std::stoi(item.at(static_cast<int>(FileIndex::GRAPH_ID)));
			m_files[info.graphId] = fileMgr.Load(info.graphId);
		}
		// 描画間隔を取得
		info.interval = std::stoi(item.at(static_cast<int>(FileIndex::DRAW_INTERVAL)));
		// 文字列を取得
		info.str = StringUtility::StringToWString(item.at(static_cast<int>(FileIndex::STR)));

		// 追加
		m_infoList.push_back(info);
	}
}

void NovelManager::Start(int id)
{
	m_novel->Start(m_infoList.at(id).str);
	m_nowId = id;
	m_isStop = false;
	m_isNowStart = true;
}

void NovelManager::Update()
{
	if (m_isStop) return;

	const auto& info = m_infoList.at(m_nowId);
	m_novel->Update(info.interval);

	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OK))
	{
		// 終了していない場合
		if (!m_novel->IsEnd())
		{
			// 今回初めていない場合スキップする
			if (!m_isNowStart) m_novel->Skip();
		}
		// 終了していて、次の文字列がある場合
		else if (info.isNext)
		{
			// 次の文章を開始する
			Start(info.nextId);
		}
		// 終了していて、次がない場合
		else
		{
			// 停止する
			m_isStop = true;
		}
	}

	m_isNowStart = false;
}

void NovelManager::Draw(int spaceWidthNum, int startY, unsigned int color) const
{
	if (m_isStop) return;
	m_novel->Draw(spaceWidthNum, startY, color);
}

void NovelManager::Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const
{
	if (m_isStop) return;
	m_novel->Draw(spaceWidthNum, startY, color, fontSize, rateW, rateH);
}

void NovelManager::DrawGraph(int x, int y, float size) const
{
	if (m_isStop) return;
	const auto& info = m_infoList.at(m_nowId);
	if (!info.isGraph) return;
	m_novel->DrawGraph(x, y, size, m_files.at(info.graphId)->GetHandle());
}

bool NovelManager::IsPlay() const
{
	// ストップなら終了していることに
	if (m_isStop) return true;

	const auto& info = m_infoList.at(m_nowId);
	// 次の文字列がある場合は終了していないことに
	if (info.isNext) return false;
	// 現在の状態を返す
	return m_novel->IsEnd();
}

NovelManager::State NovelManager::GetState() const
{
	const auto& info = m_infoList.at(m_nowId);
	// 終了していなければ描画中
	if (!m_novel->IsEnd())	return State::DRAWING;
	// 次の文字列があるので待機中
	if (info.isNext)		return State::WAIT;
	// 終了
	else					return State::END;
}
