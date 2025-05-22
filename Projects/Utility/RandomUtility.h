#pragma once
#include <random>
#include <cassert>

class Random
{
private:
	Random() : m_isInitialized(false)
	{}

	Random(const Random& app) = delete;
	void operator= (const Random& app) = delete;

public:
	static Random& GetInstance()
	{
		static Random instance;
		return instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init()
	{
		assert(!m_isInitialized && "既にランダムの初期化を行っています");
		std::random_device rd;
		m_mt = std::mt19937(rd());
		m_isInitialized = true;
	}

	/// <summary>
	/// 乱数(整数)の取得
	/// </summary>
	/// <returns>乱数(整数)</returns>
	int GetRand()
	{
		assert(m_isInitialized && "ランダムの初期化をしていません");
		return m_mt();
	}
	/// <summary>
	/// 乱数の取得(整数で範囲制限)
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数(整数)</returns>
	int GetRand(int min, int max)
	{
		assert(m_isInitialized && "ランダムの初期化をしていません");
		std::uniform_int_distribution<int> uid(min, max);
		return uid(m_mt);
	}
	/// <summary>
	/// 乱数の取得(実数で範囲制限)
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数(実数)</returns>
	float GetRand(float min, float max)
	{
		assert(m_isInitialized && "ランダムの初期化をしていません");
		std::uniform_real_distribution<float> urd(min, max);
		return urd(m_mt);
	}

private:
	std::mt19937 m_mt;
	bool m_isInitialized;
};