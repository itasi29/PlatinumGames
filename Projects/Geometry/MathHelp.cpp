#include "MathHelp.h"
#include <cmath>
#include <algorithm>
#include "Vec2.h"
#include "Game.h"
#include "RandomUtility.h"
#include "DebugDraw.h"

float GetPitchAngle(const Vec3& vecA, const Vec3& vecB)
{
	// ベクトルaからベクトルbへの方向ベクトルを計算
	const auto& dir = vecB - vecA;

	// 水平方向の長さを計算
	float horizontalLength = std::sqrt(dir.x * dir.x + dir.z * dir.z);

	// ピッチ角を計算（ラジアン）
	float pitch = std::atan2(dir.y, horizontalLength);

	// ピッチ角を度に変換
	float pitchDegree = pitch * Game::RAD_2_DEG;

	return pitchDegree;
}

bool IsParallelOnLine(const Vec3& startA, const Vec3& endA, const Vec3& startB, const Vec3& endB)
{
	auto vA = endA - startA;
	auto vB = endB - startB;

	auto cross = Vec3::Cross(vA, vB);

	return cross.SqLength() < 0.001f;
}

Vec3 GetNearestPtOnLine(const Vec3& point, const Vec3& start, const Vec3& end)
{
	float t = 0.0f;
	return GetNearestPtOnLine(point, start, end, t);
}

Vec3 GetNearestPtOnLine(const Vec3& point, const Vec3& start, const Vec3& end, float& rate)
{
	Vec3 startToEnd = end - start;
	Vec3 startToPoint = point - start;

	// 線上のどの辺か
	rate = Vec3::Dot(startToEnd, startToPoint) / startToEnd.SqLength();
	// 排他処理
	rate = std::fmax(std::fmin(rate, 1.0f), 0.0f);

	return start + startToEnd * rate;
}

void GetNearestPtOnLine(const Vec3& centerA, const Vec3& dirA, const Vec3& centerB, const Vec3& dirB, Vec3& outPtA, Vec3& outPtB)
{
	auto aS = centerA + dirA;
	auto aE = centerA - dirA;
	auto bS = centerB + dirB;
	auto bE = centerB - dirB;

	outPtA = GetNearestPtOnLine(centerB, aS, aE);
	outPtB = GetNearestPtOnLine(outPtA, bS, bE);
}

Vec3 GetNearestPtOnBox(const Vec3& pos, const Vec3& boxPos, const Vec3& boxScale, const Quaternion& rot)
{
	bool isIn;
	return GetNearestPtOnBox(pos, boxPos, boxScale, rot, isIn);
}

Vec3 GetNearestPtOnBox(const Vec3& pos, const Vec3& boxPos, const Vec3& boxScale, const Quaternion& rot, bool& isIn)
{
	// ボックスの中心から円の中心までのベクトルを作成
	auto boxToPt = pos - boxPos;


	// それぞれの方向(正面・右・上)へのベクトルを作成する
	auto right = rot * Vec3::Right();
	auto up = rot * Vec3::Up();
	auto front = rot * Vec3::Front();
	auto size = boxScale * 0.5f;
	auto prRight = Vec3::Projection(boxToPt, right);
	auto prUp = Vec3::Projection(boxToPt, up);
	auto prFront = Vec3::Projection(boxToPt, front);
	// 向きをサイズ以内にする
	isIn = true;
	if (prRight.SqLength() > size.x * size.x)
	{
		prRight = prRight.GetNormalized() * size.x;
		isIn = false;
	}
	if (prUp.SqLength() > size.y * size.y)
	{
		prUp = prUp.GetNormalized() * size.y;
		isIn = false;
	}
	if (prFront.SqLength() > size.z * size.z)
	{
		isIn = false;
		prFront = prFront.GetNormalized() * size.z;
	}
	// 球に近い場所を求める
	return boxPos + prRight + prUp + prFront;
}

bool IsNearestPointOnLine(const Vec3& point, const Vec3& start, const Vec3& end)
{
	const auto& startToEnd = end - start;
	const auto& startToPoint = point - start;

	// 線上のどの辺か
	float t = Vec3::Dot(startToEnd, startToPoint) / startToEnd.SqLength();

	// tが線分上ならtrue
	return 0.0f <= t && t <= 1.0f;
}

Vec3 GetRandomVec(const Vec3& base, float angle)
{
	auto& rand = Random::GetInstance();
	// ランダムな角度を取得
	const auto& randAngle = rand.GetRand(0.0f, angle);
	// ランダムな回転軸を取得
	const auto& randAxis = Vec3(rand.GetRand(0.0f, 1.0f), rand.GetRand(0.0f, 1.0f), rand.GetRand(0.0f, 1.0f)).GetNormalized();

	return Quaternion::AngleAxis(randAngle, randAxis) * base;
}
