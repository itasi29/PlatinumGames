#include "SceneResult.h"
#include <DxLib.h>
#include <algorithm>
#include "RandomUtility.h"
#include "TimeUtility.h"
#include "NumUtility.h"
#include "Matrix4x4.h"
#include "Game.h"
#include "Files.h"
#include "Input.h"
#include "CommandList.h"
#include "RankingDataManager.h"
#include "StageDataManager.h"
#include "SceneManager.h"
#include "SceneStageSelect.h"
#include "SceneMain.h"
#include "AnimController.h"
#include "Object/AnimPlayer.h"
#include "Object/Stage/StageManager.h"

namespace
{
	/* ウィンドウ */
	// 描画位置
	constexpr int DRAW_WINDOW_X = 900;
	constexpr int DRAW_WINDOW_Y = Game::CENTER_Y;
	/* タイムフレーム */
	// 描画位置
	constexpr int DRAW_TIME_FRAME_X = DRAW_WINDOW_X;
	constexpr int DRAW_TIME_FRAME_Y = 400;
	/* タイム */
	constexpr float NUM_SIZE = 0.875f;
	constexpr float NUM_SZIE_TIME_SMALL = NUM_SIZE * 0.8f;
	constexpr float NUM_SZIE_TIME_LARGE = NUM_SIZE * 1.2f;
	constexpr unsigned int COLOR_TIME = 0xffffff;
	// 描画間隔
	constexpr int DRAW_TIME_X_INTERVAL = static_cast<int>(NumUtility::SRC_SIZE_X * NUM_SIZE);
	// 基準描画位置
	constexpr int DRAW_TIME_X = 945;
	constexpr int DRAW_TIME_Y = 410;
	// 描画位置
	constexpr int DRAW_TIME_MIN_X = DRAW_TIME_X - DRAW_TIME_X_INTERVAL * 3;		// 分
	constexpr int DRAW_TIME_COLON_X = DRAW_TIME_X - DRAW_TIME_X_INTERVAL * 2;	// ：
	constexpr int DRAW_TIME_SEC_X = DRAW_TIME_X;								// 秒
	constexpr int DRAW_TIME_DOT_X = DRAW_TIME_X + DRAW_TIME_X_INTERVAL;			// .
	// 描画位置(コンマ以下)
	constexpr int DRAW_TIME_DEC_X = DRAW_TIME_X + DRAW_TIME_X_INTERVAL + static_cast<int>(NumUtility::SRC_SIZE_X * NUM_EX_RATE * NUM_SIZE * 2);
	constexpr int DRAW_TIME_DEC_Y = DRAW_TIME_Y + static_cast<int>(NumUtility::SRC_SIZE_Y * 0.15f * NUM_EX_RATE * NUM_SIZE);

	/* クリアランク描画関係 */
	// ランクごとのファイルID
	const std::unordered_map<RankKind, int> FILE_RANK =
	{
		{ RankKind::S, I_RANK_S },
		{ RankKind::A, I_RANK_A },
		{ RankKind::B, I_RANK_B },
		{ RankKind::C, I_RANK_C },
	};
	// 描画位置
	constexpr int DRAW_RANK_X = DRAW_WINDOW_X;
	constexpr int DRAW_RANK_Y = 200;
	// 画像サイズ
	constexpr float FILE_SIZE_RANK = 0.4f;
	constexpr float FILE_SIZE_RANK_LARGE = 0.7f;
	constexpr float FILE_SIZE_RANK_LARGE_2 = 0.45f;
	constexpr float FILE_SIZE_RANK_SMALL = 0.3f;
	
	/* ランキング更新 */
	// 描画位置
	constexpr int DRAW_RANKING_X = DRAW_WINDOW_X;
	constexpr int DRAW_RANKING_Y = 550;
	// 画像サイズ
	constexpr float FILE_SIZE_RANKING = 1.0f;
	constexpr float FILE_SIZE_RANKING_LARGE = 1.3f;
	constexpr float FILE_SIZE_RANKING_LARGE_2 = 1.1f;
	constexpr float FILE_SIZE_RANKING_SMALL = 0.9f;

	/* PadUI系 */
	constexpr int DRAW_PAD_X = 1040;	// 画像X描画位置
	constexpr int DRAW_PAD_Y = 688;		// 画像Y描画位置
	constexpr int DRAW_PAD_STR_X = 1060;	// 文字列X描画位置
	constexpr int RETURN_STR_SHOADOW_POS = 2;	// 影描画位置
	constexpr int DRAW_PAD_X_INTERVAL = -212;	// X描画位置の間隔
	constexpr float FILE_SIZE_PAD = 0.3f;	// 画像サイズ
	constexpr int FONT_SIZE_RETURN = 28;	// フォントサイズ
	constexpr unsigned int COLOR_PAD = 0xffffff;	// 文字カラー
	constexpr int DOWN_PAD_Y = 60;			// Y位置の下げる大きさ

	/* フレーム系 */
	constexpr int DECIDE_COUNT = 20;	// 決定までのフレーム
	constexpr int MOVE_COUNT = 20;		// 移動フレーム
	constexpr int TOTAL_STAMP_FRAME = 30;						// 合計スタンプフレーム
	constexpr int STAMP_FIRST_FRAME = 20;						// 縮小フレーム1
	constexpr int STAMP_SECOND_FRAME = STAMP_FIRST_FRAME + 7;	// 拡大フレーム1
	constexpr int PAD_FADE_FRAME = 15;

	/* カメラ情報 */
	constexpr float CAMERA_NEAR = 0.1f;
	constexpr float CAMERA_FAR = 100.0f;
	constexpr float CAMERA_FOV_ANGLE = 60.0f * Game::DEG_2_RAD;
	const VECTOR CAMERA_POS = VGet(-132.0f, 7.5f, 30.0f);
	const VECTOR CAMERA_DIR = VGet(10.0f, 0.0f, 0.0f);

	/* 背景モデル情報 */
	constexpr int STAGE_NO = 11;							// ステージ名
	const wchar_t* const ANIM_INFO_PATH = L"Data/Master/AnimPlayer.csv";	// アニメーションパス
	constexpr float MODEL_SCALE = 0.075f;	// モデルサイズ
	const Vec3 MODEL_POS = Vec3(-116.0f, 0.0f, 36.0f);	// モデル座標
	const Quaternion MODEL_ROT = Quaternion::AngleAxis(70.0f, Vec3::Up());	// 回転情報
	const Matrix4x4 MODEL_MAT = Matrix4x4::Scale(MODEL_SCALE) * Matrix4x4::Rot(MODEL_ROT) * Matrix4x4::Pos(MODEL_POS);	// 上3つを行列情報に変換
}

SceneResult::SceneResult(const int stageNo, const int clearTime) :
	SceneBase(SceneKind::CLEAR),
	m_stageNo(stageNo),
	m_clearTime(clearTime),
	m_processFunc(&SceneResult::TimeUpdate),
	m_moveCount{ 0 },
	m_indexDecide(0),
	m_count(0),
	m_isDecide{ false },
	m_isMove{ false },
	m_isAllDecide(false),
	m_isAllMoved(false),
	m_isRankStamped(false),
	m_isRankingStamped(false),
	m_isRankingUpdate(false),
	m_isExistNext(false)
{
}

void SceneResult::AsyncInit()
{
	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_files[I_RANK_S]			= fileMgr.Load(I_RANK_S);
	m_files[I_RANK_A]			= fileMgr.Load(I_RANK_A);
	m_files[I_RANK_B]			= fileMgr.Load(I_RANK_B);
	m_files[I_RANK_C]			= fileMgr.Load(I_RANK_C);
	m_files[I_CLEAR_WINDOW]		= fileMgr.Load(I_CLEAR_WINDOW);
	m_files[I_CLEAR_TIME_FRAME]	= fileMgr.Load(I_CLEAR_TIME_FRAME);
	m_files[I_RANKING_UPDATE]	= fileMgr.Load(I_RANKING_UPDATE);
	m_files[I_PAD_A]			= fileMgr.Load(I_PAD_A);
	m_files[I_PAD_B]			= fileMgr.Load(I_PAD_B);
	m_files[B_RESULT]			= fileMgr.Load(B_RESULT);
	m_files[M_PLAYER]			= fileMgr.Load(M_PLAYER);
	m_psH = fileMgr.GetPS(M_PLAYER);
	m_vsH = fileMgr.GetVS(M_PLAYER);
	// マネージャー生成
	m_stageMgr = std::make_shared<StageManager>(STAGE_NO);
	m_stageMgr->AsyncInit();
}

void SceneResult::Init()
{
	m_scnMgr.ChangeBgmH(m_files.at(B_RESULT)->GetHandle());

	// クリア時間が10位に入ったか
	m_isRankingUpdate = RankingDataManager::GetInstance().CheckRankingUpdate(m_stageNo - 1, m_clearTime);
	// 入っていなければランキング更新のスタンプ既にしていることに
	if (!m_isRankingUpdate) m_isRankingStamped = true;
	// 次のステージがあるか
	m_isExistNext = m_stageNo + 1 < StageDataManager::GetInstance().GetStageNum() - 1;

	// 背景モデル情報を取得
	m_stageMgr->Init(nullptr, nullptr);
	m_stageMgr->AppLights();
	// モデルにアニメーションを追加
	m_anim = std::make_shared<AnimController>();
	m_anim->Init(ANIM_INFO_PATH, m_files.at(M_PLAYER)->GetHandle(), ANIM_CLEAR);
	// モデルの座標を設定
	MV1SetMatrix(m_files.at(M_PLAYER)->GetHandle(), MODEL_MAT.GetMATRIX());
	// カメラの設定
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);
	SetupCamera_Perspective(CAMERA_FOV_ANGLE);
	SetCameraPositionAndTarget_UpVecY(CAMERA_POS, VAdd(CAMERA_POS, CAMERA_DIR));
}

void SceneResult::End()
{
	m_stageMgr->End();
}

void SceneResult::Update(bool isFade)
{
	(this->*m_processFunc)();
	m_anim->Update();
	if (isFade) return;
	SelectUpdate();
}

void SceneResult::Draw() const
{
	m_stageMgr->Draw();
	SetUseVertexShader(m_vsH);
	SetUsePixelShader(m_psH);
	MV1DrawModel(m_files.at(M_PLAYER)->GetHandle());

	// ウィンドウ
	DrawRotaGraphFast(DRAW_WINDOW_X, DRAW_WINDOW_Y, 1.0f, 0.0f, m_files.at(I_CLEAR_WINDOW)->GetHandle(), true);
	// タイムフレーム
	DrawRotaGraphFast(DRAW_TIME_FRAME_X, DRAW_TIME_FRAME_Y, 1.0f, 0.0f, m_files.at(I_CLEAR_TIME_FRAME)->GetHandle(), true);
	DrawTime();
	DrawRank();
	DrawRankingUpdate();
	DrawPadUI();
}

void SceneResult::TimeUpdate()
{
	bool isAllMove = true;
	for (int i = 0; i < static_cast<int>(DecideTime::MAX); ++i)
	{
		// 動かないなら次へ
		if (!m_isMove->at(i))
		{
			isAllMove = false;
			continue;
		}

		// 大きさ変更
		auto& count = m_moveCount->at(i);
		++count;
		if (count > MOVE_COUNT)
		{
			count = MOVE_COUNT;
		}
		else
		{
			isAllMove = false;
		}
	}
	// 全ての移動が完了していればランクのスタンプに遷移
	if (isAllMove)
	{
		m_count = 0;
		m_isAllMoved = true;
		m_processFunc = &SceneResult::RankUpdate;
		return;
	}
	
	// 時間の確定を全て行っていたら終了
	if (m_isAllDecide) return;
	// 一定時間経過したら1つずつ時間を確定させる
	++m_count;
	if (m_count > DECIDE_COUNT)
	{
		m_isDecide->at(m_indexDecide) = true;
		m_isMove->at(m_indexDecide) = true;
		m_count = 0;
		++m_indexDecide;
		if (m_indexDecide >= static_cast<int>(DecideTime::MAX))
		{
			m_isAllDecide = true;
		}
	}
}

void SceneResult::RankUpdate()
{
	// スタンプ更新
	++m_count;
	if (m_count > TOTAL_STAMP_FRAME)
	{
		m_count = 0;
		m_isRankStamped = true;
		// ランキングの更新がなければPadUIのフェードに遷移
		if (!m_isRankingUpdate)  m_processFunc = &SceneResult::PadUIFadeUpdate;
		// 更新があればスタンプに遷移
		else					 m_processFunc = &SceneResult::RankingUpdate;
	}
}

void SceneResult::RankingUpdate()
{
	// ランキングを更新していなければ終了
	if (!m_isRankingUpdate) return;

	// スタンプ更新
	++m_count;
	if (m_count > TOTAL_STAMP_FRAME) 
	{
		m_count = 0;
		m_isRankingStamped = true;
		m_processFunc = &SceneResult::PadUIFadeUpdate;
	}
}

void SceneResult::PadUIFadeUpdate()
{
	++m_count;
	if (m_count > PAD_FADE_FRAME) m_count = PAD_FADE_FRAME;
}

void SceneResult::SelectUpdate()
{
	auto& input = Input::GetInstance();
	if (input.IsTriggerd(Command::BTN_OK))
	{
		// タイムを決定していない場合
		if (!m_isAllMoved)
		{
			m_processFunc = &SceneResult::RankUpdate;
			m_isAllMoved = true;
			m_count = 0;
		}
		// ランクをスタンプ仕切っていない場合
		else if (!m_isRankStamped)
		{
			m_processFunc = &SceneResult::RankingUpdate;
			m_isRankStamped = true;
			m_count = 0;
		}
		// ランキング更新がある時かつランキングをスタンプ仕切っていない場合
		else if (m_isRankingUpdate && !m_isRankingStamped)
		{
			m_isRankingStamped = true;
			m_count = 0;
		}
		// 全て完了していれば
		else
		{
			// 次のステージが存在していれば、次のステージへ
			if (m_isExistNext)
			{
				auto next = std::make_shared<SceneMain>(m_stageNo + 1);
				m_scnMgr.Change(next);
			}
			// 存在しなければ、ステージ選択へ
			else
			{
				auto next = std::make_shared<SceneStageSelect>();
				next->ChangeSelectCurrent(m_stageNo - 1);
				m_scnMgr.Change(next);
			}
		}
		return;
	}
	// キャンセルでステージ選択へ
	if (input.IsTriggerd(Command::BTN_CANCEL))
	{
		auto next = std::make_shared<SceneStageSelect>();
		next->ChangeSelectCurrent(m_stageNo - 1);
		m_scnMgr.Change(next);
	}
}

void SceneResult::DrawTime() const
{
	if (m_isAllMoved)
	{
		TimeUtility::DrawTime(DRAW_TIME_X, DRAW_TIME_Y, m_clearTime, NUM_SIZE, COLOR_TIME);
	}
	else
	{
		auto& util = NumUtility::GetInstance();
		const auto& time = TimeUtility::FrameToTime(m_clearTime);
		DrawTime(DRAW_TIME_MIN_X, DRAW_TIME_Y, DecideTime::MIN, time.min, COLOR_TIME);
		util.DrawNumber(DRAW_TIME_COLON_X, DRAW_TIME_Y, NUM_SIZE, COLOR_TIME, -1, L":");
		DrawTime(DRAW_TIME_SEC_X, DRAW_TIME_Y, DecideTime::SEC, time.sec, COLOR_TIME);
		util.DrawNumber(DRAW_TIME_DOT_X, DRAW_TIME_Y, NUM_SIZE, COLOR_TIME, -1, L".");
		DrawTime(DRAW_TIME_DEC_X, DRAW_TIME_DEC_Y, DecideTime::DEC, time.dec, COLOR_TIME);
	}
}

void SceneResult::DrawTime(int x, int y, DecideTime kind, int time, unsigned int color) const
{
	auto& util = NumUtility::GetInstance();

	float size = NUM_SIZE;
	if (kind == DecideTime::DEC)	size *= NUM_EX_RATE;
	// 確定している場合
	if (m_isDecide->at(static_cast<int>(kind)))
	{
		int count = m_moveCount->at(static_cast<int>(kind));
		// 移動させる場合
		if (count < MOVE_COUNT)
		{
			float size2 = size;
			if (count < 5)
			{
				size2 = NUM_SZIE_TIME_SMALL;
				if (kind == DecideTime::DEC) size2 *= NUM_EX_RATE;
			}
			else if (count < 8)
			{
			}
			else
			{
				size2 = NUM_SZIE_TIME_LARGE;
				if (kind == DecideTime::DEC) size2 *= NUM_EX_RATE;
			}
			y -= static_cast<int>((size2 - size) * SLIDE_Y);
			util.DrawNumberMore(x, y, size2, color, time, 2);
		}
		// させない場合
		else
		{
			util.DrawNumberMore(x, y, size, color, time, 2);
		}
	}
	// 確定していない場合
	else
	{
		// ランダムで数を描画
		int rand = Random::GetInstance().GetRand(0, 99);
		util.DrawNumberMore(x, y, size, color, rand, 2);
	}
}

void SceneResult::DrawRank() const
{
	// 数字が全て動き終わるまで描画しない
	if (!m_isAllMoved) return;

	auto& stageDataMgr = StageDataManager::GetInstance();
	const auto rank = stageDataMgr.GetRank(m_stageNo, m_clearTime);

	DrawStamp(DRAW_RANK_X, DRAW_RANK_Y, FILE_RANK.at(rank), FILE_SIZE_RANK, FILE_SIZE_RANK_LARGE, FILE_SIZE_RANK_LARGE_2, FILE_SIZE_RANK_SMALL, m_count, m_isRankStamped);
}

void SceneResult::DrawRankingUpdate() const
{
	// ランキングを更新していなければ終了
	if (!m_isRankingUpdate) return;
	// ランクの動きが終わるまで描画しない
	if (!m_isRankStamped) return;

	DrawStamp(DRAW_RANKING_X, DRAW_RANKING_Y, I_RANKING_UPDATE, FILE_SIZE_RANKING, FILE_SIZE_RANKING_LARGE, FILE_SIZE_RANKING_LARGE_2, FILE_SIZE_RANKING_SMALL, m_count, m_isRankingStamped);
}

void SceneResult::DrawPadUI() const
{
	// ランクのスタンプが終わるまでは描画しない
	if (!m_isRankStamped) return;
	// ランキング更新のスタンプが終わるまでは描画しない
	if (!m_isRankingStamped) return;

	int y = DRAW_PAD_Y;
	const bool isAlpha = m_count < PAD_FADE_FRAME;
	if (isAlpha)
	{
		const float rate = m_count / static_cast<float>(PAD_FADE_FRAME);
		const int alpha = static_cast<int>(Game::ALPHA_VAL * rate);
		y += static_cast<int>(DOWN_PAD_Y * (1.0f - rate));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	}

	if (m_isExistNext)
	{
		// ステージ選択へ
		DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_B)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, RETURN_STR_SHOADOW_POS, RETURN_STR_SHOADOW_POS, COLOR_PAD, L"ステージ選択へ", FONT_SIZE_RETURN, 0x000000);
		// 次のステージへ
		DrawRotaGraphFast(DRAW_PAD_X + DRAW_PAD_X_INTERVAL, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X + DRAW_PAD_X_INTERVAL, y, RETURN_STR_SHOADOW_POS, RETURN_STR_SHOADOW_POS, COLOR_PAD, L"次のステージへ", FONT_SIZE_RETURN, 0x000000);
	}
	else
	{
		// ステージ選択へ
		DrawRotaGraphFast(DRAW_PAD_X, y, FILE_SIZE_PAD, 0.0f, m_files.at(I_PAD_A)->GetHandle(), true);
		UIUtility::DrawShadowStrLeft(DRAW_PAD_STR_X, y, RETURN_STR_SHOADOW_POS, RETURN_STR_SHOADOW_POS, COLOR_PAD, L"ステージ選択へ", FONT_SIZE_RETURN, 0x000000);
	}
	
	if (isAlpha) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneResult::DrawStamp(int x, int y, int fileId, float defSize, float largeSize, float largeSize2, float smallSize, int count, bool isStamped) const
{
	if (isStamped)
	{
		DrawRotaGraph(x, y, defSize, 0.0f, m_files.at(fileId)->GetHandle(), true);
	}
	else
	{
		// 縮小
		if (count < STAMP_FIRST_FRAME)
		{
			const float rate = count / static_cast<float>(STAMP_FIRST_FRAME);
			float size = Easing::EaseIn(largeSize, smallSize, rate);
			int alpha = static_cast<int>(255 * rate);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawRotaGraph(x, y, size, 0.0f, m_files.at(fileId)->GetHandle(), true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		// 拡大
		else if (count < STAMP_SECOND_FRAME)
		{
			const float rate = (count - STAMP_FIRST_FRAME) / static_cast<float>(STAMP_SECOND_FRAME - STAMP_FIRST_FRAME);
			float size = Easing::EaseIn(smallSize, largeSize2, rate);
			DrawRotaGraph(x, y, size, 0.0f, m_files.at(fileId)->GetHandle(), true);
		}
		// 縮小
		else
		{
			const float rate = (count - STAMP_SECOND_FRAME) / static_cast<float>(TOTAL_STAMP_FRAME - STAMP_SECOND_FRAME);
			float size = Easing::EaseIn(largeSize2, defSize, rate);
			DrawRotaGraph(x, y, size, 0.0f, m_files.at(fileId)->GetHandle(), true);
		}
	}
}
