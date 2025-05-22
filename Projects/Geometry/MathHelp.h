#pragma once
#include "Vec3.h"
#include "Quaternion.h"

/// <summary>
/// ベクトルaからベクトルbまで向かうベクトルの上下の回転具合（ピッチ角）を取得する
/// </summary>
/// <param name="a">ベクトルa</param>
/// <param name="b">ベクトルb</param>
/// <returns>ピッチ角（度数法）</returns>
float GetPitchAngle(const Vec3& vecA, const Vec3& vecB);

/// <summary>
/// 線分同士が平行か判定
/// </summary>
/// <param name="startA">線分Aの開始地点</param>
/// <param name="endA">線分Aの終了地点</param>
/// <param name="startB">線分Bの開始地点</param>
/// <param name="endB">線分Bの終了地点</param>
/// <returns>true: 平行/ false: 平行じゃない</returns>
bool IsParallelOnLine(const Vec3& startA, const Vec3& endA, const Vec3& startB, const Vec3& endB);

/// <summary>
/// 線分上での最近接点を取得
/// </summary>
/// <param name="point">点</param>
/// <param name="start">線分の開始地点</param>
/// <param name="end">線分の終了地点</param>
/// <returns>最近接点</returns>
Vec3 GetNearestPtOnLine(const Vec3& point, const Vec3& start, const Vec3& end);
/// <summary>
/// 線分上での最近接点を取得
/// </summary>
/// <param name="point">点</param>
/// <param name="start">線分の開始地点</param>
/// <param name="end">線分の終了地点</param>
/// <param name="rate">線分上での割合</param>
/// <returns>最近接点</returns>
Vec3 GetNearestPtOnLine(const Vec3& point, const Vec3& start, const Vec3& end, float& rate);
/// <summary>
/// 線分同士の最近接点を取得
/// </summary>
/// <param name="centerA">線分Aの中心点</param>
/// <param name="dirA">線分Aの方向</param>
/// <param name="centerB">線分Bの中心点</param>
/// <param name="dirB">線分Bの方向</param>
/// <param name="outPtA">線分A上での最近接点</param>
/// <param name="outPtB">線分B上での最近接点</param>
void GetNearestPtOnLine(const Vec3& centerA, const Vec3& dirA, const Vec3& centerB, const Vec3& dirB, Vec3& outPtA, Vec3& outPtB);
/// <summary>
/// ボックス上での最近接点を取得
/// </summary>
/// <param name="pos">点</param>
/// <param name="boxPos">ボックスの中心点</param>
/// <param name="boxScale">ボックスのサイズ</param>
/// <param name="rot">ボックスの回転</param>
/// <returns>最近接点</returns>
Vec3 GetNearestPtOnBox(const Vec3& pos, const Vec3& boxPos, const Vec3& boxScale, const Quaternion& rot);
/// <summary>
/// ボックス上での最近接点を取得、ボックス内判定結果取得
/// </summary>
/// <param name="pos">点</param>
/// <param name="boxPos">ボックスの中心点</param>
/// <param name="boxScale">ボックスのサイズ</param>
/// <param name="rot">ボックスの回転</param>
/// <param name="isIn">true: ボックス内に存在する /false:存在しない</param>
/// <returns>最近接点</returns>
Vec3 GetNearestPtOnBox(const Vec3& pos, const Vec3& boxPos, const Vec3& boxScale, const Quaternion& rot, bool& isIn);
/// <summary>
/// 線分上に最近接点があるかの判定
/// </summary>
/// <param name="point">点</param>
/// <param name="start">線分の開始地点</param>
/// <param name="end">線分の終了地点</param>
/// <returns>最近接点</returns>
bool IsNearestPointOnLine(const Vec3& point, const Vec3& start, const Vec3& end);

/// <summary>
/// 基準ベクトルからangle度以内にランダムでベクトルを作成する
/// </summary>
/// <param name="base">ベースとなるベクトル</param>
/// <param name="angle">生成範囲内角度</param>
/// <returns>ランダムベクトル</returns>
Vec3 GetRandomVec(const Vec3& base, float angle);