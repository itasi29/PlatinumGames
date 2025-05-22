#ifdef _DEBUG
#include "SceneDebug.h"
#include <DxLib.h>
#include <string>
#include "Files.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneMain.h"
#include "SceneTutorial.h"
#include "SceneResult.h"
#include "CursorUtility.h"
#include "StageDataManager.h"

namespace
{
	// シーン名
	std::wstring NAME[] =
	{
		L"Title",
		L"StageSelect",
		L"Main",
		L"Tutorial",
		L"Result",
		L"Option",
		L"Debug",
	};
}

SceneDebug::SceneDebug() :
	SceneBase(SceneKind::DEBUG),
	m_next(0), 
	m_stageNo(0)
{
}

void SceneDebug::AsyncInit()
{
	m_files[B_TITLE] = FileManager::GetInstance().Load(B_TITLE);
}

void SceneDebug::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_TITLE)->GetHandle());
}

void SceneDebug::End()
{
}

void SceneDebug::Update(bool isFade)
{
	if (isFade) return;
	auto& stageDataMgr = StageDataManager::GetInstance();
	CursorUtility::CursorUp<SceneKind>(m_next, SceneKind::MAX);
	CursorUtility::CursorDown<SceneKind>(m_next, SceneKind::MAX);
	CursorUtility::CursorUp(m_stageNo, stageDataMgr.GetStageNum() - 2, Command::BTN_LEFT);
	CursorUtility::CursorDown(m_stageNo, stageDataMgr.GetStageNum() - 2, Command::BTN_RIGHT);

	auto& input = Input::GetInstance();
	if (input.IsTriggerd(KEY_INPUT_RETURN) || input.IsTriggerd(Command::BTN_OK))
	{
		std::shared_ptr<SceneBase> next;

		if (static_cast<SceneKind>(m_next) == SceneKind::TITLE)
		{
			next = std::make_shared<SceneTitle>();
		}
		else if (static_cast<SceneKind>(m_next) == SceneKind::STAGE_SELECT)
		{
			next = std::make_shared<SceneStageSelect>();
		}
		else if (static_cast<SceneKind>(m_next) == SceneKind::MAIN)
		{
			if (m_stageNo == 0)
			{
				next = std::make_shared<SceneTutorial>();
			}
			else
			{
				next = std::make_shared<SceneMain>(m_stageNo + 1);
			}
		}
		else if (static_cast<SceneKind>(m_next) == SceneKind::TUTORIAL)
		{
			next = std::make_shared<SceneTutorial>();
		}
		else if (static_cast<SceneKind>(m_next) == SceneKind::CLEAR)
		{
			next = std::make_shared<SceneResult>(m_stageNo + 1, 3600 * 12 + 60 * 34 + 56);
			
		}

		m_scnMgr.Change(next);
	}
}

void SceneDebug::Draw() const
{
	DrawString(8, 8, L"SceneDebug", 0xffffff);

	int y = 32;
	for (int i = 0; i < static_cast<int>(SceneKind::MAX); ++i)
	{
		if (i == static_cast<int>(m_next))
		{
			DrawBox(16, y - 4, 96, y + 20, 0xff0000, true);
		}

		DrawFormatString(16, y, 0xffffff, NAME[i].c_str());
		if (i >= static_cast<int>(SceneKind::OPTION))
		{
			DrawFormatString(128, y, 0xffffff, L"使えません");
		}
		

		y += 24;
	}

	DrawFormatString(256, 32, 0x00ffff, L"ステージ番号：%d", m_stageNo + 1);
	DrawString(256, 64, StageDataManager::GetInstance().GetStageName(m_stageNo + 1), 0x00ffff);
}
#endif