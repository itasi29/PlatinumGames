#include "MoveFloorManager.h"
#include "MoveFloor.h"
#include "FileId.h"

namespace
{
    // 生成間隔
    constexpr int CREATE_INTERVAL = 60 * 8;
}

MoveFloorManager::MoveFloorManager() :
    GimmickLinkObject(Priority::STATIC, ObjectTag::NONE),
    m_createFrame(0),
    m_isLoop(false)
{
}

MoveFloorManager::~MoveFloorManager()
{
}

void MoveFloorManager::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
    GimmickLinkObject::Init(pos, scale, rot, list, isGravity);
    m_colList = list;
    // HACK: 無駄にPhysicsにエントリーをしている
    OnExistPhysics();
}

void MoveFloorManager::End()
{
    GimmickLinkObject::End();

    for (auto& item : m_floorList)  item->End();
}

void MoveFloorManager::Restart()
{
    GimmickLinkObject::Restart();
    // 生成フレームの初期化
    m_createFrame = CREATE_INTERVAL;
    // 動く床の終了処理
    for (auto& item : m_floorList)  item->End();
    // 動く床の削除
    m_floorList.clear();
    // ループの再設置
    SetLoop(m_isLoop);
}

void MoveFloorManager::Draw() const
{
    for (auto& item : m_floorList)  item->Draw();
}

void MoveFloorManager::SetLoop(bool isLoop)
{
    m_isLoop = isLoop;
    if (isLoop)
    {
        auto floor = std::make_shared<MoveFloor>(*this);
        // 初期化
        floor->LoadModel(M_FLOOR);
        floor->Init(m_rigid.GetPos(), m_scale, m_rotation, m_colList, false);
        floor->InitDir(GetMoveDir(m_rigid.GetPos(), 1));
        floor->SetFunc(true);
        floor->StopMove();
        // リストに追加
        m_floorList.emplace_back(floor);
    }
}

Vec3 MoveFloorManager::GetMoveDir(const Vec3& pos, int no) const
{
    const auto& next = m_checkPtTable.at(no);
    return (next - pos).GetNormalized();
}

bool MoveFloorManager::CheckMovedCheckPt(const Vec3& pos, int no)
{
    const auto& next = m_checkPtTable.at(no);

    // 現在地から次のチェックポイントまでの距離が一定値以上ならばまだついていない
    auto dis = (next - pos).SqLength();
    // FIXME: 距離は後でいいぐらいの位置に調整する
    return dis < 1.0f;
}

void MoveFloorManager::GimmickOnUpdate()
{
    CreateUpdate();
    FloorUpdate();
}

void MoveFloorManager::GimmickOffUpdate()
{
    // MEMO:特に処理なし
}

void MoveFloorManager::CreateUpdate()
{
    // ループ時は生成しない
    if (m_isLoop) return;

    --m_createFrame;
    if (m_createFrame < 0)
    {
        auto floor = std::make_shared<MoveFloor>(*this);
        // 初期化
        floor->LoadModel(M_FLOOR);
        floor->Init(m_rigid.GetPos(), m_scale, m_rotation, m_colList, false);
        floor->InitDir(GetMoveDir(m_rigid.GetPos(), 1));
        floor->SetFunc(false);
        // リストに追加
        m_floorList.emplace_back(floor);
        // カウント初期化
        m_createFrame = CREATE_INTERVAL;
    }
}

void MoveFloorManager::FloorUpdate()
{
    // 更新
    for (auto& item : m_floorList)  item->Update();
    // 削除
    m_floorList.remove_if(
        [](const auto& item)
        {
            return item->IsEnd();
        });
}

void MoveFloorManager::ProcessGimmickOn()
{
    // 停止させていたものを再開
    for (auto& item : m_floorList)  item->ReStartMove();
}

void MoveFloorManager::ProcessGimmickOff()
{
    // 一時停止
    for (auto& item : m_floorList)  item->StopMove();
}
