#include "Input.h"
#include <DxLib.h>
#include <cassert>
#include <array>
#include "Game.h"
#include "CommandList.h"

Input::Input() :
	m_padstate(0),
	m_prePadstate(0)
#ifdef _DEBUG
	, 
	m_keystate {},
	m_preKeystate {}
#endif
{
	// 上
	m_commandTable[Command::BTN_UP]			 = PAD_INPUT_UP;
	// 下
	m_commandTable[Command::BTN_DOWN]		 = PAD_INPUT_DOWN;
	// 右
	m_commandTable[Command::BTN_RIGHT]		 = PAD_INPUT_RIGHT;
	// 左
	m_commandTable[Command::BTN_LEFT]		 = PAD_INPUT_LEFT;
	// オプション
	m_commandTable[Command::BTN_OPTION]		 = PAD_INPUT_8;
	// 決定
	m_commandTable[Command::BTN_OK]			 = PAD_INPUT_1;
	// キャンセル
	m_commandTable[Command::BTN_CANCEL]		 = PAD_INPUT_2;
	// ジャンプ
	m_commandTable[Command::BTN_JUMP]		 = PAD_INPUT_1;
	// 走り
	m_commandTable[Command::BTN_RUN]		 = PAD_INPUT_3;
	// オブジェクトを持つ/離す
	m_commandTable[Command::BTN_HAND]		 = PAD_INPUT_2;
	// ビューの変更
	m_commandTable[Command::BTN_CHANGE_VIEW] = PAD_INPUT_4;
	// カメラ180度回転
	m_commandTable[Command::BTN_TURN_CAMERA] = PAD_INPUT_6;
}

Input& Input::GetInstance()
{
	static Input input;
	return input;
}

void Input::Init()
{
}

void Input::Update()
{
	// 事前の情報を保存
	m_prePadstate = m_padstate;

	// 入力情報の取得
	m_padstate = GetJoypadInputState(DX_INPUT_PAD1);
	XINPUT_STATE temp;
	GetJoypadXInputState(DX_INPUT_PAD1, &temp);
	auto& xinput = m_trigger;
	xinput.LT = GetTriggerRate(temp.LeftTrigger);
	xinput.RT = GetTriggerRate(temp.RightTrigger);
	xinput.LStick.x = GetStickRate(temp.ThumbLX);
	xinput.LStick.y = GetStickRate(temp.ThumbLY);
	xinput.RStick.x = GetStickRate(temp.ThumbRX);
	xinput.RStick.y = GetStickRate(temp.ThumbRY);

#ifdef _DEBUG
	for (int i = 0; i < 256; ++i)
	{
		m_preKeystate[i] = m_keystate[i];
	}
	GetHitKeyStateAll(m_keystate);
#endif
}

bool Input::IsPress(const char* const command) const
{
	const auto& cmd = m_commandTable.at(command);
	return m_padstate & cmd;
}

bool Input::IsTriggerd(const char* const command) const
{
	const auto& cmd = m_commandTable.at(command);
	return (m_padstate & cmd) && !(m_prePadstate & cmd);
}

bool Input::IsReleased(const char* const command) const
{
	const auto& cmd = m_commandTable.at(command);
	return !(m_padstate & cmd) && (m_prePadstate & cmd);
}

bool Input::IsRepeat(const char* const command, int repeatInterval, int startInterval)
{
	RepeatKind temp;
	return IsRepeat(command, temp, repeatInterval, startInterval);
}

bool Input::IsRepeat(const char* const command, RepeatKind& kind, int repeatInterval, int startInterval)
{
	auto& frame = m_repeatFrame[command];
	kind = RepeatKind::NONE;
	// 押した瞬間
	if (IsTriggerd(command))
	{
		// スタートリピート間隔が-ならリピート時間にする
		if (startInterval < 0)	frame = repeatInterval;
		// 違うならその値を入れる
		else					frame = startInterval;
		kind = RepeatKind::TRIGGER;
		return true;
	}
	// 押している間
	else if (IsPress(command))
	{
		bool isCheck = false;
		kind = RepeatKind::COOL_DOWN;

		// フレームが0未満になったら
		if (frame < 0)
		{
			// フレームをリピート時間にする
			frame = repeatInterval;
			kind = RepeatKind::REPEAT;
			isCheck = true;
		}

		// フレーム時間を減らす
		--frame;
		return isCheck;
	}
	return false;
}

#ifdef _DEBUG
bool Input::IsPress(int key) const
{
	return m_keystate[key];
}

bool Input::IsTriggerd(int key) const
{
	return m_keystate[key] && !m_preKeystate[key];
}
#endif

float Input::GetTriggerRate(unsigned char trigger)
{
	return ApplyDeadZone(trigger / Game::ANALOG_TRIGGER_RANGE);
}

float Input::GetStickRate(short val)
{
	// スティックの入力値を取得
	float rate = val / Game::ANALOG_STICK_RANGE;
	// 逆回転の場合は正に変換
	bool isReverse = rate < 0.0f;
	if (isReverse) rate = std::abs(rate);
	// デッドゾーンを適用
	rate = ApplyDeadZone(rate);
	// 逆回転の場合は負に変換
	if (isReverse) rate *= -1;
	return rate;
}

float Input::ApplyDeadZone(float rate)
{
	// デッドゾーンの適用
	rate = (rate - Game::ANALOG_DEAD_ZONE_INSIDE) / (Game::ANALOG_DEAD_ZONE_OUTSIDE - Game::ANALOG_DEAD_ZONE_INSIDE);
	rate = std::min<float>(std::max<float>(rate, 0.0f), 1.0f);

	return rate;
}
