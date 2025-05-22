#ifdef _DEBUG
#include "DebugDraw.h"
#include <DxLib.h>
#include <cassert>
#include "Game.h"

namespace
{
    // グリッド
    constexpr int GULID_NUM = 10;
    constexpr int GULID_SIZE = 20;
    constexpr int GULID_SPACE = GULID_SIZE * 2 / GULID_NUM;
}
using namespace MyEngine;

DebugDraw::DebugDraw() :
    m_isStop(false)
{
}

DebugDraw::~DebugDraw()
{
}

DebugDraw& DebugDraw::GetInstance()
{
    static DebugDraw instance;
    return instance;
}

void DebugDraw::Gulid() const
{
    assert(GULID_SPACE != 0 && "グリッドが無限ループします");
    for (int x = -GULID_SIZE; x <= GULID_SIZE; x += GULID_SPACE)
    {
        DrawLine3D(VGet(static_cast<float>(x), 0, -GULID_SIZE), VGet(static_cast<float>(x), 0, GULID_SIZE), 0xffff00);
    }
    for (int z = -GULID_SIZE; z <= GULID_SIZE; z += GULID_SPACE)
    {
        DrawLine3D(VGet(-GULID_SIZE, 0, static_cast<float>(z)), VGet(GULID_SIZE, 0, static_cast<float>(z)), 0xff0000);
    }

    // X+-,Z+-の方向が分かりやすいように表示を追加する
    VECTOR dispPos = ConvWorldPosToScreenPos(VGet(GULID_SIZE, 0, 0));
    if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
    {
        DrawStringF(dispPos.x, dispPos.y, L"X+", 0xffffff);
    }

    dispPos = ConvWorldPosToScreenPos(VGet(-GULID_SIZE, 0, 0));
    if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
    {
        DrawStringF(dispPos.x, dispPos.y, L"X-", 0xffffff);
    }

    dispPos = ConvWorldPosToScreenPos(VGet(0, 0, GULID_SIZE));
    if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
    {
        DrawStringF(dispPos.x, dispPos.y, L"Z+", 0xffffff);
    }

    dispPos = ConvWorldPosToScreenPos(VGet(0, 0, -GULID_SIZE));
    if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
    {
        DrawStringF(dispPos.x, dispPos.y, L"Z-", 0xffffff);
    }
}

void MyEngine::DebugDraw::Tree(int level, const Vec3& min, const Vec3& max) const
{
    auto vec = max - min;
    vec = vec / static_cast<float>(level);

    Vec3 start = min;
    Vec3 end = min;
    end.x += vec.x;
    for (int i = 0; i < level; ++i)
    {
        DrawCube3D(start.VGet(), end.VGet(), 0xff0000, 0, false);
    }
}

void DebugDraw::Clear()
{
    m_circleInfo.clear();
    m_lineInfo.clear();
    m_sphereInfo.clear();
    m_capsuleInfo.clear();
    m_boxInfo.clear();
    m_terrainInfo.clear();
}

void DebugDraw::Draw() const
{
    // 円情報描画
    for (const auto& info : m_circleInfo)
    {
        int x = static_cast<int>(info.center.x);
        int y = static_cast<int>(info.center.y);
        int radius = static_cast<int>(info.radius);
        DxLib::DrawCircle(x, y, radius, info.color);
        
    }
    // 線情報描画
    for (const auto& info : m_lineInfo)
    {
        DxLib::DrawLine3D(info.start.VGet(), info.end.VGet(), info.color);
    }
    SetUseLighting(false);
    // 球描画
    int y = 32;
    for (const auto& info : m_sphereInfo)
    {
        DxLib::DrawSphere3D(info.center.VGet(), info.radius, 8, info.color, 0, info.isFill);
    }
    // カプセル描画a
    for (const auto& info : m_capsuleInfo)
    {
        auto halfSize = info.size * 0.5f;
        auto start = info.center + info.dir * halfSize;
        auto end = info.center - info.dir * halfSize;
        DxLib::DrawCapsule3D(start.VGet(), end.VGet(), info.radius, 8, info.color, 0, info.isFill);
    }
    // ボックス描画
    for (const auto& info : m_boxInfo)
    {
        auto size = info.size * 0.5f;
        auto right = info.rot * Vec3::Right() * size.x;
        auto up = info.rot * Vec3::Up() * size.y;
        auto front = info.rot * Vec3::Front() * size.z;

        auto fru = info.center + front + up + right;
        auto flu = info.center + front + up - right;
        auto frd = info.center + front - up + right;
        auto fld = info.center + front - up - right;
        auto bru = info.center - front + up + right;
        auto blu = info.center - front + up - right;
        auto brd = info.center - front - up + right;
        auto bld = info.center - front - up - right;

        // 前面
        DxLib::DrawLine3D(fru.VGet(), flu.VGet(), info.color);
        DxLib::DrawLine3D(frd.VGet(), fld.VGet(), info.color);
        DxLib::DrawLine3D(fru.VGet(), frd.VGet(), info.color);
        DxLib::DrawLine3D(flu.VGet(), fld.VGet(), info.color);
        // 後面
        DxLib::DrawLine3D(bru.VGet(), blu.VGet(), info.color);
        DxLib::DrawLine3D(brd.VGet(), bld.VGet(), info.color);
        DxLib::DrawLine3D(bru.VGet(), brd.VGet(), info.color);
        DxLib::DrawLine3D(blu.VGet(), bld.VGet(), info.color);
        // 左右
        DxLib::DrawLine3D(fru.VGet(), bru.VGet(), info.color);
        DxLib::DrawLine3D(frd.VGet(), brd.VGet(), info.color);
        DxLib::DrawLine3D(flu.VGet(), blu.VGet(), info.color);
        DxLib::DrawLine3D(fld.VGet(), bld.VGet(), info.color);
    }
    // 地面を描画
    for (const auto& info : m_terrainInfo)
    {
        if (info.isFill || false)
        {
            VERTEX3D v[4];
            WORD index[6];

            int r = (0xff0000 & info.color) >> 16;
            int g = (0x00ff00 & info.color) >> 8;
            int b = (0x0000ff & info.color);
            for (auto& t : v)
            {
                t.norm = info.norm.VGet();
                t.dif = GetColorU8(r, g, b, 255);
                t.spc = GetColorU8(0, 0, 0, 0);
                t.u = 0.0f;
                t.v = 0.0f;
                t.su = 0.0f;
                t.sv = 0.0f;
            }

            auto rp = info.rot * Vec3::Right() * info.size.x * 0.5f;
            auto fp = info.rot * Vec3::Front() * info.size.y * 0.5f;

            auto rf = info.center + rp + fp;
            auto lf = info.center - rp + fp;
            auto rb = info.center + rp - fp;
            auto lb = info.center - rp - fp;

            v[0].pos = lb.VGet();
            v[1].pos = rb.VGet();
            v[2].pos = lf.VGet();
            v[3].pos = rf.VGet();

            index[0] = 0;
            index[1] = 1;
            index[2] = 2;
            index[3] = 3;
            index[4] = 2;
            index[5] = 1;

            DxLib::DrawPolygonIndexed3D(v, 4, index, 2, DX_NONE_GRAPH, false);
        }
        else
        {
            auto rp = info.rot * Vec3::Right() * info.size.x * 0.5;
            auto fp = info.rot * Vec3::Front() * info.size.y * 0.5;

            auto rf = info.center + rp + fp;
            auto lf = info.center - rp + fp;
            auto rb = info.center + rp - fp;
            auto lb = info.center - rp - fp;

            int size = 100;
            auto shift = rp / (size * 0.5f);
            auto s = rb;
            auto e = rf;
            for (int i = 0; i < size; ++i)
            {
                DrawLine3D(s.VGet(), e.VGet(), info.color);
                s -= shift;
                e -= shift;
            }
            DrawLine3D(lb.VGet(), lf.VGet(), info.color);

            shift = fp / (size * 0.5f);
            s = rb;
            e = lb;
            for (int i = 0; i < size; ++i)
            {
                DrawLine3D(s.VGet(), e.VGet(), info.color);
                s += shift;
                e += shift;
            }
            DrawLine3D(rf.VGet(), lf.VGet(), info.color);
        }
    }
    SetUseLighting(true);
}

void MyEngine::DebugDraw::DrawCircle(const Vec3& center, float radius, unsigned int color)
{
    if (m_isStop) return;

    m_circleInfo.emplace_back(CircleInfo{ center, radius, color });
}

void MyEngine::DebugDraw::DrawLine(const Vec3& start, const Vec3& end, unsigned int color)
{
    if (m_isStop) return;

    m_lineInfo.emplace_back(LineInfo{ start, end, color });
}

void MyEngine::DebugDraw::DrawSphere(const Vec3& center, float radius, unsigned int color, bool isFill)
{
    if (m_isStop) return;

    m_sphereInfo.emplace_back(SphereInfo{ center, radius, color, isFill});
}

void MyEngine::DebugDraw::DrawCapsule(const Vec3& center, const Vec3& dir, float radius, float size, unsigned int color, bool isFill)
{

    if (m_isStop) return;

    m_capsuleInfo.emplace_back(CapsuleInfo{ center, dir, radius, size, color, isFill });
}

void MyEngine::DebugDraw::DrawCube(const Vec3& center, const Vec3& size, const Quaternion& rot, unsigned int color, bool isFill)
{
    if (m_isStop) return;

    m_boxInfo.emplace_back(BoxInfo{ center, size, rot, color, isFill });
}

void MyEngine::DebugDraw::DrawTerrain(const Vec3& center, const Vec3& norm, const Vec2& size, const Quaternion& rot, unsigned int color, bool isFill)
{
    if (m_isStop) return;

    m_terrainInfo.emplace_back(TerrainInfo{ center, size, norm, rot, color, isFill });
}
#endif