#include "Novel.h"
#include <DxLib.h>
#include <cassert>
#include "Game.h"
#include "Font.h"

namespace
{
	// 文字間隔
	constexpr int INTERVAL_X = 16;
	constexpr int INTERVAL_Y = 16;
	// 描画可能文字列
	constexpr int NUM_DRAW_BASE = Game::WINDOW_W / INTERVAL_X;
	// ベースフォントサイズ
	constexpr int BASE_FONT_SIZE = 16;
}

Novel::Novel() :
	m_frame(0),
	m_current(1),
	m_isEnd(false)
{
}

Novel::~Novel()
{
}

void Novel::Start(const std::wstring& str)
{
	m_str = str;
	// 初期化
	m_current = 1;
	m_frame = 0;
	m_isEnd = false;
}

void Novel::Skip()
{
	if (m_isEnd) return;

	// 現在位置を最後尾に
	m_current = static_cast<int>(m_str.size());
	m_isEnd = true;
}

void Novel::Update(const int interval)
{
	if (m_isEnd) return;

	++m_frame;
	// 一定間隔経ったら次の文字へ
	if (m_frame > interval)
	{
		m_frame = 0;
		++m_current;
		// 文字列の最後に達したら終了
		if (m_current >= m_str.size())
		{
			m_isEnd = true;
		}
	}
}

void Novel::Draw(int spaceWidthNum, int startY, unsigned int color) const
{
	// 描画可能文字列数
	const int num = NUM_DRAW_BASE - spaceWidthNum * 2;
	// 空白サイズ
	const int space = INTERVAL_X * spaceWidthNum;

	int x = space;
	int y = startY;
	int count = 0;
	// 1文字ずつ描画
	for (int i = 0; i < m_current; ++i)
	{
		const auto& indent = m_str.substr(i, 2);
		// 改行
		if (indent == L"\\n")
		{
			x = space;
			y += INTERVAL_Y;
			count = 0;
			++i;
			continue;
		}
		// 文字描画
		else
		{
			const auto& c = m_str.at(i);
			DrawFormatString(x, y, color, L"%c", c);
			x += INTERVAL_X;
			++count;
			// 行末に達したら改行
			if (count >= num)
			{
				x = space;
				y += INTERVAL_Y;
				count = 0;
			}
		}
	}
}

void Novel::Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const
{
	// 文字列ハンドル
	const auto fontH = FontManager::GetInstance().GetHandle(FONT_MEIRYO, fontSize);

	const float rate = fontSize / static_cast<float>(BASE_FONT_SIZE);
	const int intervalX = static_cast<int>(INTERVAL_X * rate * rateW);
	const int intervalY = static_cast<int>(INTERVAL_Y * rate * rateH);

	// 描画可能文字列数
	const int num = Game::WINDOW_W / intervalX - spaceWidthNum * 2;
	// 空白サイズ
	const int space = intervalX * spaceWidthNum;

	int x = space;
	int y = startY;
	int count = 0;
	// 1文字ずつ描画
	for (int i = 0; i < m_current; ++i)
	{
		const auto& indent = m_str.substr(i, 2);
		// 改行
		if (indent == L"\\n")
		{
			x = space;
			y += intervalY;
			count = 0;
			++i;
			continue;
		}
		// 文字描画
		else
		{
			const auto& c = m_str.at(i);
			DrawFormatStringToHandle(x, y, color, fontH, L"%c", c);
			x += intervalX;
			++count;
			// 行末に達したら改行
			if (count >= num)
			{
				x = space;
				y += intervalY;
				count = 0;
			}
		}
	}
}

void Novel::DrawGraph(int x, int y, float size, int handle)
{
	auto err = DrawRotaGraph(x, y, size, 0.0f, handle, true);
	assert(err != -1 && "グラフ読み込みに失敗しています");
}
