#pragma once
#include "Input.h"
#include "CommandList.h"

class CursorUtility
{
public:
	static bool CursorUp(int& index, int max, const char* command = Command::BTN_UP, bool isRepeat = false, int repeatIntreval = 5)
	{
		auto& input = Input::GetInstance();

		RepeatKind kind;
		// 入力からtrueが返ってきた場合
		if (input.IsRepeat(command, kind, repeatIntreval))
		{
			// 入力直後のフレームの場合
			if (kind == RepeatKind::TRIGGER)
			{
				// 繰り返しありでインデックス減少
				index = (index - 1 + max) % max;
				return true;
			}
			// 繰り返しありの場合
			else if (isRepeat)
			{
				// 繰り返しなしでインデックス減少
				--index;
				if (index < 0)
				{
					// 繰り返しの場合はカーソルが移動していないことに
					index = 0;
					return false;
				}
				return true;
			}
		}

		return false;
	}
	static bool CursorDown(int& index, int max, const char* command = Command::BTN_DOWN, bool isRepeat = false, int repeatIntreval = 5)
	{
		auto& input = Input::GetInstance();

		RepeatKind kind;
		// 入力からtrueが返ってきた場合
		if (input.IsRepeat(command, kind, repeatIntreval))
		{
			// 入力直後のフレームの場合
			if (kind == RepeatKind::TRIGGER)
			{
				// 繰り返しありでインデックス増加
				index = (index + 1) % max;
				return true;
			}
			// 繰り返しありの場合
			else if (isRepeat)
			{
				// 繰り返しなしでインデックス増加
				++index;
				if (index > max - 1)
				{
					// 繰り返しの場合はカーソルが移動していないことに
					index = max - 1;
					return false;
				}
				return true;
			}
		}
		
		return false;
	}
	template<class T>
	static bool CursorUp(int& index, T max, const char* command = Command::BTN_UP, bool isRepeat = false, int repeatIntreval = 5)
	{
		return CursorUp(index, static_cast<int>(max), command, isRepeat, repeatIntreval);
	}
	template<class T>
	static bool CursorDown(int& index, T max, const char* command = Command::BTN_DOWN, bool isRepeat = false, int repeatIntreval = 5)
	{
		return CursorDown(index, static_cast<int>(max), command, isRepeat, repeatIntreval);
	}
};
