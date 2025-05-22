#pragma once
#include <cassert>
#include <functional>
#include <unordered_map>

template<class State_t> class StateMachine
{
public:
	using Delegate_t = std::function<void(void)>;

private:
	struct StateDelegateData
	{
		Delegate_t entry;
		Delegate_t update;
		Delegate_t exit;
	};

public:
	StateMachine() :
		m_isInit(true)
	{
	}

	/// <summary>
	/// ステートの追加
	/// </summary>
	/// <param name="state">ステート</param>
	/// <param name="entry">入ったときに呼ぶ関数</param>
	/// <param name="update">毎フレーム呼ぶ関数</param>
	/// <param name="exit">終わったときに呼ぶ関数</param>
	void AddState(State_t state, Delegate_t entry, Delegate_t update, Delegate_t exit)
	{
		// 重複処理
		if (m_stateFuncMap.find(state) != m_stateFuncMap.end())
		{
			assert(false && "既にステートが追加されています");
			return;
		}

		StateDelegateData data;
		data.entry = entry;
		data.update = update;
		data.exit = exit;
		m_stateFuncMap.emplace(state, data);
	}

	/// <summary>
	/// ステートの変更
	/// </summary>
	/// <typeparam name="State_t">ステート</typeparam>
	void ChangeState(State_t state)
	{
		// 初回ならEntryだけ呼ぶ
		if (m_isInit)
		{
			// nullptrでなければ呼ぶ
			if (m_stateFuncMap.at(state).entry)
			{
				m_stateFuncMap.at(state).entry();
			}
			m_nowState = state;
			m_isInit = false;
		}
		// 違うステートが入ってきたら更新
		else if (m_nowState != state)
		{
			if (m_stateFuncMap.at(m_nowState).exit)
			{
				m_stateFuncMap.at(m_nowState).exit();
			}
			if (m_stateFuncMap.at(state).entry)
			{
				m_stateFuncMap.at(state).entry();
			}
			m_nowState = state;
		}
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update()
	{
		m_stateFuncMap.at(m_nowState).update();
	}

	/// <summary>
	/// ステートの取得
	/// </summary>
	/// <returns>現在のステート</returns>
	State_t GetNowState() const { return m_nowState; }

private:
	// 現在のステート
	State_t m_nowState;
	// ステートの関数マップ
	std::unordered_map<State_t, StateDelegateData> m_stateFuncMap;
	// 初回かどうか
	bool m_isInit;
};