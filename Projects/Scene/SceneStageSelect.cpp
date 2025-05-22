#include "SceneStageSelect.h"
#include <DxLib.h>
#include <sstream>
#include "CursorUtility.h"
#include "UIUtility.h"
#include "TimeUtility.h"
#include "SaveDataManager.h"
#include "StageDataManager.h"
#include "SoundManager.h"
#include "Game.h"
#include "Files.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneMain.h"
#include "SceneTutorial.h"

namespace
{
	enum class CheckCurrent : int
	{
		CANCEL,
		RETURN,
		MAX
	};

	/* ウィンドウ */
	constexpr int DRAW_WINDOW_Y = 420;	// 描画位置
	constexpr int DRAW_OBI_Y = 64;		// 帯描画位置
	// 分割線描画位置
	constexpr int DRAW_DIVIDING_LINE_X = 927;
	constexpr int DRAW_DIVIDING_LINE_Y = DRAW_WINDOW_Y;

	/* 画像飾り */
	// 描画位置
	constexpr int DRAW_STAGE_IMAGE_X = 927;
	constexpr int DRAW_STAGE_IMAGE_Y = 281;
	// 飾り位置
	constexpr int DRAW_DECORATIVE_FRAME_X_1 = 1166;
	constexpr int DRAW_DECORATIVE_FRAME_Y_1 = 188;
	constexpr int DRAW_DECORATIVE_FRAME_X_2 = 688;
	constexpr int DRAW_DECORATIVE_FRAME_Y_2 = 375;

	/* ステージフレーム */
	constexpr int DRAW_STAGE_X = 170;	// 描画位置(x)
	constexpr int DRAW_STAGE_Y = 240;	// 描画位置(y)
	constexpr int DRAW_STAGE_INTERVAL = 180;	// 描画間隔
	constexpr int SRC_SIZE_STAGE_LOGO = 99;	// ステージロゴの切り取りサイズ
	constexpr int STAGE_LOGO_LINE_NUM = 4;	// ステージロゴの列数
	constexpr int STAGE_LOGO_ROW_NUM = 4;	// ステージロゴの行数
	constexpr int DRAW_SUB_STAGE_NEW_X = 44;	// Newの描画位置差分(x)
	constexpr int DRAW_SUB_STAGE_NEW_Y = -40;	// Newの描画位置差分(y)
	constexpr int DRAW_SUB_STAGE_NEW_SELECT = 8;	// 選択中の場合の差分
	// クリア時カラー
	constexpr float COLOR_CLEAR_LOGO_R = 0.125f; // 赤
	constexpr float COLOR_CLEAR_LOGO_G = 1.0f;	 // 緑
	constexpr float COLOR_CLEAR_LOGO_B = 0.125f; // 青
	// 未クリア時カラー
	constexpr float COLOR_NOT_CLEAR_LOGO_R = 1.0f;	// 赤
	constexpr float COLOR_NOT_CLEAR_LOGO_G = 1.0f;	// 緑
	constexpr float COLOR_NOT_CLEAR_LOGO_B = 1.0f;	// 青
	// 未開放時のカラー
	constexpr float COLOR_NOT_RELEASE_LOGO_R = 0.75f;	// 赤
	constexpr float COLOR_NOT_RELEASE_LOGO_G = 0.75f;	// 緑
	constexpr float COLOR_NOT_RELEASE_LOGO_B = 0.75f;	// 青
	/* ロゴの影色 */
	constexpr float COLOR_LOGO_SHADOW_R = 0.0f;	// 赤
	constexpr float COLOR_LOGO_SHADOW_G = 0.0f;	// 緑
	constexpr float COLOR_LOGO_SHADOW_B = 0.0f;	// 青

	/* ランクタイム */
	// 描画間隔
	constexpr int DRAW_RANK_Y_INTERVAL = 88;
	constexpr int DRAW_RANK_X_INTERVAL = 32;
	// 描画位置
	constexpr int DRAW_RANK_Y = 473;		// 基準Y
	constexpr int DRAW_RANK_X = 700;		// 画像X
	constexpr int DRAW_RANK_TIME_X = 760;	// 文字列X
	constexpr int DRAW_RANK_TIME_Y_SUB = 24;	// 文字列描画差分
	// サイズ
	constexpr int FONT_SIZE_RANK_TIME = 48;		// フォントサイズ
	constexpr float FILE_SIZE_RANK = 0.125f;	// ファイルサイズ
	// その他
	constexpr unsigned int COLOR_RANK_TIME = 0xffffff;			// 文字列カラー
	constexpr unsigned int SHADOW_COLOR_RANK_TIME = 0x444444;	// 影の色
	constexpr int SHADOW_POS_RANK_TIME = 3;		// 影位置

	/* PadUi描画 */
	constexpr int DRAW_PAD_X = 1068;		// 基準Y描画位置
	constexpr int DRAW_PAD_Y = 670;			// 画像描画位置
	constexpr int DRAW_PAD_Y_INTERVAL = 50;	// 描画間隔
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int DRAW_PAD_STR_X = 1088;	// 文字列描画位置
	constexpr int FONT_SIZE_PAD = 28;		// フォントサイズ
	constexpr int PAD_STR_SHADOW_POS = 2;	// 影描画位置
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー

	/* その他 */
	constexpr int STAGE_LINE_NUM = 3;					// ステージ列数
	constexpr float ADD_SCALE_STAGE_SELECT = 0.1f;		// 選択フレームの追加拡大率
	constexpr float CYCLE_SCALING_STAGE_SELECT = 3.0f;	// 選択フレームの拡縮サイクル
	constexpr float WAVE_SPEED = 3.0f;					// 文字ウェーブスピード
	constexpr int REPEATE_INTERVAL = 15;				// リピート間隔
}

SceneStageSelect::SceneStageSelect() :
	SceneBase(SceneKind::STAGE_SELECT),
	m_cBuff(nullptr),
	m_cBuffH(-1),
	m_selectCurrent(0),
	m_count(0)
{
}

void SceneStageSelect::AsyncInit()
{
	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[PS_COLOR]				= fileMgr.Load(PS_COLOR);
	m_files[I_SELECT_WINDOW_BACK]	= fileMgr.Load(I_SELECT_WINDOW_BACK);
	m_files[I_SELECT_WINDOW]		= fileMgr.Load(I_SELECT_WINDOW);
	m_files[I_RELEASE_FRAME]		= fileMgr.Load(I_RELEASE_FRAME);
	m_files[I_NOT_RELEASE_FRAME]	= fileMgr.Load(I_NOT_RELEASE_FRAME);
	m_files[I_STAGE_SELECT_FRAME]	= fileMgr.Load(I_STAGE_SELECT_FRAME);
	m_files[I_DECORATIVE_FRAME]		= fileMgr.Load(I_DECORATIVE_FRAME);
	m_files[I_DIVIDING_LINE]		= fileMgr.Load(I_DIVIDING_LINE);
	m_files[I_SELECT_OBI]			= fileMgr.Load(I_SELECT_OBI);
	m_files[I_COMMON_FRAME]			= fileMgr.Load(I_COMMON_FRAME);
	m_files[I_STAGE_LOGOS]			= fileMgr.Load(I_STAGE_LOGOS);
	m_files[I_PAD_A]				= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]				= fileMgr.Load(I_PAD_B);
	m_files[I_RANK_S]				= fileMgr.Load(I_RANK_S);
	m_files[I_RANK_A]				= fileMgr.Load(I_RANK_A);
	m_files[I_RANK_B]				= fileMgr.Load(I_RANK_B);
	m_files[I_RELEASE_NEW]			= fileMgr.Load(I_RELEASE_NEW);
	m_files[S_CANCEL]				= fileMgr.Load(S_CANCEL);
	m_files[S_DECIDE]				= fileMgr.Load(S_DECIDE);
	m_files[S_CURSOR_MOVE]			= fileMgr.Load(S_CURSOR_MOVE);
	m_files[B_STAGE_SELECT]			= fileMgr.Load(B_STAGE_SELECT);
	auto stageNum = StageDataManager::GetInstance().GetStageNum();
	for (int i = 0; i < stageNum; ++i)
	{
		m_files[I_STAGE_IMAGE_0 + i] = fileMgr.Load(I_STAGE_IMAGE_0 + i);
	}
	// 定数バッファの作成
	m_cBuffH = CreateShaderConstantBuffer(sizeof(CBuff));
	assert(m_cBuffH != -1);
}

void SceneStageSelect::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_STAGE_SELECT)->GetHandle());
	// 定数バッファの取得
	m_cBuff = static_cast<CBuff*>(GetBufferShaderConstantBuffer(m_cBuffH));
	assert(m_cBuff != nullptr);
}

void SceneStageSelect::Update(bool isFade)
{
	++m_count;
	if (isFade) return;
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OPTION))
	{
		m_scnMgr.OpenOption(SceneKind::STAGE_SELECT);
		return;
	}
	SelectUpdate();
#ifdef _DEBUG
	// 全ステージクリアにする
	if (input.IsTriggerd(KEY_INPUT_C))
	{
		for (int i = 0; i < StageDataManager::GetInstance().GetStageNum() - 2; ++i)
		{
			SaveDataManager::GetInstance().OnClear(i, 12*3600+34*60+56);
		}
		return;
	}
#endif
}

void SceneStageSelect::End()
{
	// 定数バッファの解放
	DeleteShaderConstantBuffer(m_cBuffH);
	// 新規解放状況のリセット
	SaveDataManager::GetInstance().ResetNewRelease();
}

void SceneStageSelect::Draw() const
{
	auto& stageDataMgr = StageDataManager::GetInstance();
	auto& fontMgr = FontManager::GetInstance();
	const auto& stageName = stageDataMgr.GetStageName(m_selectCurrent + 1);
#ifdef _DEBUG
	printf("セーブデータ番号:%d\n", SaveDataManager::GetInstance().GetUseSaveNo() + 1);
#endif
	
	// 背景
	DrawGraph(0, 0, m_files.at(I_SELECT_WINDOW_BACK)->GetHandle(), true);
	// ウィンドウ
	DrawRotaGraphFast(Game::CENTER_X, DRAW_WINDOW_Y, 1.0f, 0.0f, m_files.at(I_SELECT_WINDOW)->GetHandle(), true);
	// 帯
	DrawRotaGraphFast(Game::CENTER_X, DRAW_OBI_Y, 1.0f, 0.0f, m_files.at(I_SELECT_OBI)->GetHandle(), true);
	// 分割線
	DrawRotaGraphFast(DRAW_DIVIDING_LINE_X, DRAW_DIVIDING_LINE_Y, 1.0f, 0.0f, m_files.at(I_DIVIDING_LINE)->GetHandle(), true);
	// ステージ画像
	DrawRotaGraphFast(DRAW_STAGE_IMAGE_X, DRAW_STAGE_IMAGE_Y, 1.0f, 0.0f, m_files.at(I_STAGE_IMAGE_0 + m_selectCurrent)->GetHandle(), true);
	// ステージ画像飾り
	DrawRotaGraphFast(DRAW_DECORATIVE_FRAME_X_1, DRAW_DECORATIVE_FRAME_Y_1, 1.0f, 0.0f, m_files.at(I_DECORATIVE_FRAME)->GetHandle(), true);
	DrawRotaGraphFast(DRAW_DECORATIVE_FRAME_X_2, DRAW_DECORATIVE_FRAME_Y_2, 1.0f, 180.0f * Game::DEG_2_RAD, m_files.at(I_DECORATIVE_FRAME)->GetHandle(), true);
	// ステージ名描画
	UIUtility::DrawWaveStr(Game::CENTER_X, DRAW_OBI_Y, 0xffffff, stageName, 36, m_count * WAVE_SPEED, 16);
	// ステージフレーム描画
	DrawStageFrame();

	DrawRankTime(m_selectCurrent + 1);
	// PadUI描画
	DrawPadUI();
}

void SceneStageSelect::SelectUpdate()
{
	auto& input = Input::GetInstance();
	auto& stageDataMgr = StageDataManager::GetInstance();
	auto& sndMgr = SoundManager::GetInstance();
	auto stageNum = stageDataMgr.GetStageNum() - 2;
	// 行数
	const int STAGE_ROW_NUM = stageNum / STAGE_LINE_NUM;
	// 最後の行の列数
	const int LAST_STAGE_LINE_NUM = stageNum % STAGE_LINE_NUM;
	// 最後の行の列数が自分が定義した列数と同じか
	const bool IS_LAST_SAME_SELF_NUM = LAST_STAGE_LINE_NUM == 0;

	bool isCursorMove = false;
	RepeatKind kind;
	// 上移動
	if (input.IsRepeat(Command::BTN_UP, kind, REPEATE_INTERVAL))
	{
		isCursorMove = true;
		m_selectCurrent -= STAGE_LINE_NUM;
		// 1未満になった場合
		if (m_selectCurrent < 0)
		{
			// 押された瞬間なら
			if (kind == RepeatKind::TRIGGER)
			{
				// 最後の行が列数と同じなら
				if (IS_LAST_SAME_SELF_NUM)
				{
					// ステージ数分足す
					m_selectCurrent += stageNum;
				}
				// 同じでないなら
				else
				{
					int row = STAGE_ROW_NUM;
					// 列上で何番目かを取得
					auto temp2 = STAGE_LINE_NUM + m_selectCurrent;
					// 番数が最後の行の列数以上なら行を一つ上にする
					if (temp2 >= LAST_STAGE_LINE_NUM) --row;
					// 計算
					m_selectCurrent = row * STAGE_LINE_NUM + temp2;
				}
			}
			// 押された瞬間でないなら
			else
			{
				// 元の場所に戻す
				m_selectCurrent += STAGE_LINE_NUM;
				isCursorMove = false;
			}
		}
	}
	// 下移動
	if (input.IsRepeat(Command::BTN_DOWN, kind, REPEATE_INTERVAL))
	{
		isCursorMove = true;
		m_selectCurrent += STAGE_LINE_NUM;
		// ステージ数を超えた場合
		if (m_selectCurrent > stageNum - 1)
		{
			// 押された瞬間なら
			if (kind == RepeatKind::TRIGGER)
			{
				// カーソルを一番上に戻す
				auto temp = m_selectCurrent - STAGE_LINE_NUM;
				m_selectCurrent = temp % STAGE_LINE_NUM;
			}
			// 押された瞬間でないなら
			else
			{
				// 元の場所に戻す
				m_selectCurrent -= STAGE_LINE_NUM;
				isCursorMove = false;
			}
		}
	}
#if true
	// 左移動
	if (input.IsRepeat(Command::BTN_LEFT, kind, REPEATE_INTERVAL))
	{
		isCursorMove = true;
		--m_selectCurrent;
		// 押された瞬間なら
		if (kind == RepeatKind::TRIGGER)
		{
			// そのまま繰り返し
			if (m_selectCurrent < 0) m_selectCurrent = stageNum - 1;
		}
		// 押された瞬間でないなら
		else
		{
			// 繰り返さないように処理
			if (m_selectCurrent < 0) m_selectCurrent = 0;
			// 列を移動するのも押された瞬間のみ
			else if (m_selectCurrent % STAGE_LINE_NUM == STAGE_LINE_NUM - 1) ++m_selectCurrent;
		}
	}
	// 右移動
	if (input.IsRepeat(Command::BTN_RIGHT, kind, REPEATE_INTERVAL))
	{
		isCursorMove = true;
		++m_selectCurrent;
		// 押された瞬間なら
		if (kind == RepeatKind::TRIGGER)
		{
			// そのまま繰り返し
			if (m_selectCurrent > stageNum - 1) m_selectCurrent = 0;
		}
		// 押された瞬間でないなら
		else
		{
			// 繰り返さないように処理
			if (m_selectCurrent > stageNum - 1) m_selectCurrent = stageNum - 1;
			// 列を移動するのも押された瞬間のみ
			else if (m_selectCurrent % STAGE_LINE_NUM == 0) --m_selectCurrent;
		}
	}
#else
	// 左右移動(行を変えない)

	// 右移動
	if (input.IsRepeat(Command::BTN_RIGHT, kind, REPEATE_INTERVAL))
	{
		// 現在の行までの数値を取得
		int Row = (m_selectCurrent / STAGE_LINE_NUM) * STAGE_LINE_NUM;
		// 現在の行での数値を取得
		int now = m_selectCurrent - Row;
		// 最大列数
		int maxLine = std::min<int>(STAGE_LINE_NUM, stageNum - Row) - 1;

		isCursorMove = true;
		++now;
		// 最大列数を超えた場合
		if (now > maxLine)
		{
			// 押された瞬間なら
			if (kind == RepeatKind::TRIGGER)
			{
				// 左端に戻す
				now = 0;
			}
			// 押された瞬間でないなら
			else
			{
				// 1つ前の位置に戻す
				--now;
				isCursorMove = false;
			}
		}
		// カーソル位置の更新
		m_selectCurrent = Row + now;
	}
	if (input.IsRepeat(Command::BTN_LEFT, kind, REPEATE_INTERVAL))
	{
		// 現在の行までの数値を取得
		int Row = (m_selectCurrent / STAGE_LINE_NUM) * STAGE_LINE_NUM;
		// 現在の行での数値を取得
		int now = m_selectCurrent - Row;
		// 最大列数
		int maxLine = std::min<int>(STAGE_LINE_NUM, stageNum - Row) - 1;

		isCursorMove = true;
		--now;
		// 0未満になった場合
		if (now < 0)
		{
			// 押された瞬間なら
			if (kind == RepeatKind::TRIGGER)
			{
				// 右端に戻す
				now = maxLine;
			}
			// 押された瞬間でないなら
			else
			{
				// 1つ後の位置に戻す
				++now;
				isCursorMove = false;
			}
		}
		// カーソル位置の更新
		m_selectCurrent = Row + now;
	}
#endif
	// 移動していたらSEを鳴らす
	if (isCursorMove) sndMgr.PlaySe(m_files.at(S_CURSOR_MOVE)->GetHandle());

	// 決定
	if (input.IsTriggerd(Command::BTN_OK))
	{
		const auto& saveDataMgr = SaveDataManager::GetInstance();
		// ステージ解放されている場合
		if (saveDataMgr.IsReleaseStage(m_selectCurrent))
		{
			// スタート
			sndMgr.PlaySe(m_files.at(S_DECIDE)->GetHandle());
			std::shared_ptr<SceneBase> next;
			if (m_selectCurrent == 0)	next = std::make_shared<SceneTutorial>();
			else						next = std::make_shared<SceneMain>(m_selectCurrent + 1);
			m_scnMgr.Change(next);
		}
		// 解放されていない場合
		else
		{

		}
	}
	// キャンセル
	else if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		sndMgr.PlaySe(m_files.at(S_CANCEL)->GetHandle());
		// タイトルへ
		auto next = std::make_shared<SceneTitle>();
		m_scnMgr.Change(next);
	}
}

void SceneStageSelect::DrawStageFrame() const
{
	const auto& saveDataMgr = SaveDataManager::GetInstance();
	// ステージ数(タイトル・リザルト分減らす)
	int id;
	float rate = 1.0f;
	float scale = 1.0f;
	int stageNum = StageDataManager::GetInstance().GetStageNum() - 2;
	for (int i = 0; i < stageNum; ++i)
	{
		int x = DRAW_STAGE_X + DRAW_STAGE_INTERVAL * (i % STAGE_LINE_NUM);
		int y = DRAW_STAGE_Y + DRAW_STAGE_INTERVAL * (i / STAGE_LINE_NUM);

		/* フレーム画像の選択 */
		bool isSelect = i == m_selectCurrent;
		bool isRelease = saveDataMgr.IsReleaseStage(i);
		// ステージ解放されている場合
		if (isRelease)
		{
			// 現在選んでいる場合
			if (isSelect)	id = I_STAGE_SELECT_FRAME;
			// 選んでいない場合
			else			id = I_RELEASE_FRAME;
		}
		// ステージ解放されていない場合
		else
		{
			id = I_NOT_RELEASE_FRAME;
		}

		// 選択中の場合
		if (isSelect)
		{
			// 割合の計算
			rate = std::sin(m_count * CYCLE_SCALING_STAGE_SELECT * Game::DEG_2_RAD);
			// 拡大率の計算
			scale += rate * ADD_SCALE_STAGE_SELECT;
		}
		// 未選択の場合
		else
		{
			rate = 0.0f;
			scale = 1.0f;
		}

		// フレームの描画
		DrawRotaGraphFast(x, y, scale, 0.0f, m_files.at(id)->GetHandle(), true);

		// ステージ内ロゴの描画
		int srcX = SRC_SIZE_STAGE_LOGO * (i % STAGE_LOGO_LINE_NUM);
		int srcY = SRC_SIZE_STAGE_LOGO * (i / STAGE_LOGO_ROW_NUM);
		// 影
		DrawStageLogo(x + 4, y + 4, srcX, srcY, scale, COLOR_LOGO_SHADOW_R, COLOR_LOGO_SHADOW_G, COLOR_LOGO_SHADOW_B);
		// クリア・解放状況による色変更描画
		if (saveDataMgr.IsClear(i))
		{
			DrawStageLogo(x, y, srcX, srcY, scale, COLOR_CLEAR_LOGO_R, COLOR_CLEAR_LOGO_G, COLOR_CLEAR_LOGO_B);
		}
		else if (isRelease)
		{
			DrawStageLogo(x, y, srcX, srcY, scale, COLOR_NOT_CLEAR_LOGO_R, COLOR_NOT_CLEAR_LOGO_G, COLOR_NOT_CLEAR_LOGO_B);
		}
		else
		{
			DrawStageLogo(x, y, srcX, srcY, scale, COLOR_NOT_RELEASE_LOGO_R, COLOR_NOT_RELEASE_LOGO_G, COLOR_NOT_RELEASE_LOGO_B);
		}

		// Newの描画
		const bool isNew = saveDataMgr.IsNewRelease(i);
		if (isRelease && isNew)
		{
			int sub = static_cast<int>(rate * DRAW_SUB_STAGE_NEW_SELECT);
			x += DRAW_SUB_STAGE_NEW_X + sub;
			y += DRAW_SUB_STAGE_NEW_Y - sub;
			
			DrawRotaGraphFast(x, y, 1.0f, 0.0f, m_files.at(I_RELEASE_NEW)->GetHandle(), true);
		}
	}
}

void SceneStageSelect::DrawStageLogo(int x, int y, int srcX, int srcY, float scale, float r, float g, float b) const
{
	// 色設定
	m_cBuff->red = r;
	m_cBuff->green = g;
	m_cBuff->blue = b;
	UpdateShaderConstantBuffer(m_cBuffH);
	SetShaderConstantBuffer(m_cBuffH, DX_SHADERTYPE_PIXEL, 4);
	// 描画
	int psH = m_files.at(PS_COLOR)->GetHandle();
	int handle = m_files.at(I_STAGE_LOGOS)->GetHandle();
	MyEngine::DrawRectRotaGraph(x, y, srcX, srcY, SRC_SIZE_STAGE_LOGO, SRC_SIZE_STAGE_LOGO, scale, 0.0f, psH, handle);
}

void SceneStageSelect::DrawRankTime(const int stageNo) const
{
	DrawRankTime(DRAW_RANK_Y, DRAW_RANK_X, DRAW_RANK_TIME_X, stageNo, RankKind::S, I_RANK_S);
	DrawRankTime(DRAW_RANK_Y + DRAW_RANK_Y_INTERVAL, DRAW_RANK_X + DRAW_RANK_X_INTERVAL, DRAW_RANK_TIME_X + DRAW_RANK_X_INTERVAL, stageNo, RankKind::A, I_RANK_A);
	DrawRankTime(DRAW_RANK_Y + DRAW_RANK_Y_INTERVAL * 2, DRAW_RANK_X + DRAW_RANK_X_INTERVAL * 2, DRAW_RANK_TIME_X + DRAW_RANK_X_INTERVAL * 2, stageNo, RankKind::B, I_RANK_B);
}

void SceneStageSelect::DrawRankTime(int y, int graphX, int strX, const int stageNo, RankKind rank, int rankId) const
{
	const int time = StageDataManager::GetInstance().GetTimeRank(stageNo, rank);
	DrawRotaGraphFast(graphX, y, FILE_SIZE_RANK, 0.0f, m_files.at(rankId)->GetHandle(), true);
	TimeUtility::DrawTimeLeft(strX, y - DRAW_RANK_TIME_Y_SUB, time, FONT_SIZE_RANK_TIME, COLOR_RANK_TIME, nullptr, nullptr, true, SHADOW_POS_RANK_TIME, SHADOW_POS_RANK_TIME, SHADOW_COLOR_RANK_TIME);
}

void SceneStageSelect::DrawPadUI() const
{
	int y = DRAW_PAD_Y;
	// A:決定
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"決定", FONT_SIZE_PAD);
	// B:タイトルへ
	y -= DRAW_PAD_Y_INTERVAL;
	DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
	UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, PAD_STR_SHADOW_POS, PAD_STR_SHADOW_POS, COLOR_PAD, L"タイトルへ", FONT_SIZE_PAD);
}
