#include "Physics.h"
#include <cassert>
#include "Game.h"
#include "Collidable.h"
#include "Collider/Colliders.h"
#include "DebugDraw.h"
#include "SceneManager.h"

using namespace MyEngine;

namespace
{
    // 判定回数
    constexpr int CHECK_MAX_COUNT = 1000;
    // 分割判定速度
    constexpr float CHECK_DIVISION_SPEED = 0.4f;
    // 判定するオブジェクト同士の距離
    constexpr float CHECK_COLLIDE_LENDGHT = 24.0f;
    constexpr float CHECK_COLLIDE_SQ_LENDGHT = CHECK_COLLIDE_LENDGHT * CHECK_COLLIDE_LENDGHT;
}

Physics::Physics() :
    m_objects{},
    m_onCollideInfo{},
    m_newCollideInfo{},
    m_preCollideInfo{},
    m_newTirrigerInfo{},
    m_preTirrigerInfo{},
    m_isNoMake(false)
{
}

Physics& Physics::GetInstance()
{
    static Physics instance;
    return instance;
}

void Physics::Entry(std::shared_ptr<Collidable> collidable)
{
    bool isFound = std::find(m_objects.begin(), m_objects.end(), collidable) != m_objects.end();
    // 未登録なら追加
    if (!isFound)
    {
        m_objects.emplace_back(collidable);
        m_isNoMake = true;
    }
    // 登録済みなら無視
    else
    {
        assert(false && "既にエントリーしています");
    }
}

void Physics::Exit(std::shared_ptr<Collidable> collidable)
{
    auto it = std::find(m_objects.begin(), m_objects.end(), collidable);
    // 登録済みなら削除
    if (it != m_objects.end())
    {
        m_objects.erase(it);
        m_checkList.remove_if(
            [&collidable](const auto& item)
            {
                if (item.primary.get() == collidable.get()) return true;
                if (item.secondary.get() == collidable.get()) return true;
                return false;
            });
    }
    // 未登録なら無視
    else
    {
        assert(false && "エントリーしていません");
    }
}

void Physics::Update()
{
    // オプションorポーズが開いていれば更新しない
    auto& scnMgr = SceneManager::GetInstance();
    if (scnMgr.IsOpenOption()) return;

    // 通知リストのクリア・更新
    m_onCollideInfo.clear();
    m_preCollideInfo = m_newCollideInfo;
    m_newCollideInfo.clear();
    m_preTirrigerInfo = m_newTirrigerInfo;
    m_newTirrigerInfo.clear();

    // 次の移動先を仮決定
    MoveNextPos();
    // 判定確認
    CheckCollide();
    // 通知リストを確認
    CheckSendOnCollideInfo(m_preTirrigerInfo, m_newTirrigerInfo, true);
    CheckSendOnCollideInfo(m_preCollideInfo, m_newCollideInfo, false);
    // 座標を決定
    FixPos();
    // 通知を送る
    for (const auto& item : m_onCollideInfo)
    {
        OnCollideInfo(item.own, item.send, item.ownColIndex, item.sendColIndex, item.hitInfo, item.kind);
    }
#ifdef _DEBUG
    for (auto& item : m_objects)
    {
        if (item->GetPriority() == Collidable::Priority::STATIC) continue;
        DebugDraw::GetInstance().DrawSphere(item->GetPos(), CHECK_COLLIDE_LENDGHT, 0x00ff00, false);
    }
#endif
}

std::vector<Physics::HitObjectInfo> MyEngine::Physics::CheckObject(Vec3& pos, Rigidbody rigid, ColliderBase* collider, int& count, int checkNum, bool isFix, PreHitInfoList_t preInfoList, std::list<ObjectTag> checkTags, std::list<Collidable*> throughObj)
{
    // 判定リストの作成
    std::list<std::shared_ptr<Collidable>> checkList;
    const auto checkPos = pos + collider->center;
    for (auto& item : m_objects)
    {
        const auto& tag = item->GetTag();
        // 全探索じゃない場合
        if (!checkTags.empty())
        {
            // チェックタグリストに存在しない場合は次のオブジェクトに
            const bool isTag = std::find(checkTags.begin(), checkTags.end(), tag) != checkTags.end();
            if (!isTag) continue;
        }
        if (!throughObj.empty())
        {
            // スルーオブジェクトなら次のオブジェクトに
            const bool isThrough = std::find(throughObj.begin(), throughObj.end(), item.get()) != throughObj.end();
            if (isThrough) continue;
        }
        // 余りにも離れすぎていたら判定しない
        for (auto& col : item->m_collider)
        {
            float sqLen = (checkPos - (item->GetPos() + col->center)).SqLength();
            if (sqLen < CHECK_COLLIDE_SQ_LENDGHT)
            {
                // 判定リスト追加して次へ
                checkList.emplace_back(item);
                break;
            }
        }
    }

    std::vector<HitObjectInfo> res;
    PreHitInfoList_t preInfoTable;
    count = 0;
    // 判定している間ループ
    while (true)
    {
        Vec3 fixPos = pos;
        preInfoTable = preInfoList;
        bool isNoHit = true;
        // 最大回数確認したら修正がうまく出来ていなくても終了する
        bool isLastCheck = count > checkNum - 1;
        for (auto& item : checkList)
        {
            for (int i = 0; i < item->m_collider.size(); ++i)
            {
                auto& checkCol = item->m_collider[i];
                auto& preHitInfo = preInfoTable[item->GetTag()];

                // 当たっていなければ次の判定に
                auto info = checkCol->IsCollide(item->GetPos(), item->m_rigid, collider, fixPos, rigid, preHitInfo);
                if (!info.isHit) continue;

                res.push_back({ item.get(), info, i });
                // 位置修正しない場合
                if (!isFix)
                {
                    // 判定回数増加
                    count++;
                    // 最大回数確認したら終了
                    if (count > checkNum - 1) return res;
                }

				// トリガーの場合は次の判定に
                if (checkCol->isTrigger) continue;

				// 位置修正
                fixPos = checkCol->FixNextPos(item->GetPos(), collider, pos, info);
                preHitInfo = { info, true };
                isNoHit = false;
                // 最後の判定ターンでないなら再度判定しなおし
                if (!isLastCheck) break;
            }
			// 最後の判定ターンでないなら再度判定しなおし
            if (!isNoHit && !isLastCheck) break;
        }
		// 位置修正
        pos = fixPos;
        // 判定回数増加
        count++;
        // 当たっていなければ終了
        if (isNoHit) break;
        if (isLastCheck) break;
    }

    return res;
}

void Physics::MoveNextPos() const
{
    int k = 0;
    for (const auto& item : m_objects)
    {
        auto& rigid = item->m_rigid;

        // 重力処理
        if (rigid.IsGravity())
        {
            auto frame = rigid.GetStayGravityFrame();
			// 停止フレームでない場合
            if (frame < 0)
            {
                auto vel = rigid.GetVelocity();
				// 重力を加算
                vel.y -= Game::GRAVITY;
				// 最大速度を超えていたら補正
                if (vel.y < -Game::GRAVITY_MAX_SPEED) vel.y = -Game::GRAVITY_MAX_SPEED;
				// 速度を設定
                rigid.SetVelocity(vel);
            }
            // 停止フレームの場合
            else
            {
				// フレーム減少
                --frame;
                rigid.SetStayGravityFrame(frame);
            }
        }

        // 座標に移動量を追加
        auto pos = rigid.GetPos();
        auto nextPos = pos + rigid.GetVelocity();
        rigid.SetNextPos(nextPos);

#ifdef _DEBUG
        // デバック情報
        AddDebugInfo(pos, item->m_collider, DebugDraw::COL_BEFOR);
        AddDebugInfo(nextPos, item->m_collider, DebugDraw::COL_NEXT);
#endif
    }
}

void Physics::CheckCollide()
{
    // 判定リスト取得
    MakeCheckList();
    const auto& list = GetCollisionList();

#ifdef _DEBUG
    int checkCount = 0;
#endif
    // 判定回数
    int count = 0;
    // 判定情報
    PreHitInfoTable_t preHitInfoList;
	// 判定している間ループ
    while (true)
    {
        bool isNoHit = true;
        // 最大回数確認したら修正がうまく出来ていなくても全部修正させる
        bool isLastCheck = count > CHECK_MAX_COUNT - 1;

        for (const auto& item : list)
        {
            const auto& colP = item.primary->GetColliderData(item.primaryIndex);
            const auto& colS = item.secondary->GetColliderData(item.secondaryIndex);

            auto& preHitInfo = preHitInfoList[item.secondary][item.primary->GetTag()];
            auto speed = item.secondary->GetVelocity().Length();
            Vec3 checkPos = item.secondary->m_rigid.GetNextPos();
            CollideHitInfo collideHitInfo;
            // 分割する速度に達していなければ純粋に分割せずに判定
            if (speed < CHECK_DIVISION_SPEED)
            {
#ifdef _DEBUG
                ++checkCount;
#endif
                collideHitInfo = colS->IsCollide(checkPos, item.primary->m_rigid, colP, item.primary->m_rigid.GetNextPos(), item.secondary->m_rigid, preHitInfo);
            }
            // 達していれば座標を分割して判定
            else
            {
                int num = static_cast<int>(speed / CHECK_DIVISION_SPEED) + 1;
                auto divisionDir = item.secondary->m_rigid.GetNextPos() - item.secondary->GetPos();
                divisionDir = divisionDir / static_cast<float>(num);
                checkPos = item.secondary->GetPos() + divisionDir;
				// 分割数分判定
                for (int k = 0; k < num; ++k)
                {
#ifdef _DEBUG
                    ++checkCount;
                    AddDebugInfo(checkPos, item.secondary->m_collider, DebugDraw::COL_HIT);
#endif
                    collideHitInfo = colS->IsCollide(checkPos, item.primary->m_rigid, colP, item.primary->m_rigid.GetNextPos(), item.secondary->m_rigid, preHitInfo);
					// 当たっていた場合その時点で終了
                    if (collideHitInfo.isHit) break;
                    checkPos += divisionDir;
                }
            }
            // 当たっていなければ次の判定に
            if (!collideHitInfo.isHit) continue;

            // 当たっていなければ次の判定に
            if (!collideHitInfo.isHit) continue;

            // 通過オブジェクト確認
            auto throughA = item.primary->m_throughTag;
            auto throughB = item.secondary->m_throughTag;
            bool isThrough = std::find(throughA.begin(), throughA.end(), item.secondary->GetTag()) != throughA.end()
                || std::find(throughB.begin(), throughB.end(), item.primary->GetTag()) != throughB.end();
            // isTriggerがtrueか通過オブジェクトなら通知だけ追加して次の判定に
            bool isTrigger = colP->isTrigger || colS->isTrigger || isThrough;
            if (isTrigger)
            {
                AddNewCollideInfo(item.primary, item.secondary, item.primaryIndex, item.secondaryIndex, m_newTirrigerInfo, collideHitInfo);
                continue;
            }

            // 通知を追加
            AddNewCollideInfo(item.primary, item.secondary, item.primaryIndex, item.secondaryIndex, m_newCollideInfo, collideHitInfo);

			// 位置修正
            auto nextPos = colP->FixNextPos(item.primary->m_rigid.GetNextPos(), colS, checkPos, collideHitInfo);
            item.secondary->m_rigid.SetNextPos(nextPos);

			// 事前判定済み情報を更新
            preHitInfo = { collideHitInfo, true };

            isNoHit = false;
			// 最後の判定ターンでないなら再度判定しなおし
            if (!isLastCheck) break;
        }

        // 判定回数増加
        count++;
        // 当たっていなければ終了
        if (isNoHit) break;
        // 最大回数確認したら修正がうまく出来ていなくても終了する
        if (isLastCheck) break;
    }

#ifdef _DEBUG
    printf("判定回数:%d\nループ回数:%d\n", checkCount, count - 1);
#endif
}

std::list<Physics::CheckData> Physics::GetCollisionList() const
{
    std::list<CheckData> res;
    CheckData temp;

	// チェックリストを回す
    for (auto& item : m_checkList)
    {
        temp.primary = item.primary;
        temp.secondary = item.secondary;
        // プライマリーが同じの場合は速度で判定する
        if (item.isSamePriority)
        {
            // primaryの速度の方が遅い場合切り替え
            if (temp.primary->GetVelocity().SqLength() < temp.secondary->GetVelocity().SqLength())
            {
                temp.primary = item.secondary;
                temp.secondary = item.primary;
            }
        }

        const auto& colP = temp.primary->m_collider;
        const auto& colS = temp.secondary->m_collider;
        for (int i = 0; i < colP.size(); ++i)
        {
            for (int j = 0; j < colS.size(); ++j)
            {
                // それぞれの判定同士の距離が一定いないなら追加
                const auto& posP = temp.primary->GetPos() + colP.at(i)->center;
                const auto& posS = temp.secondary->GetPos() + colS.at(j)->center;
                if ((posP - posS).SqLength() < CHECK_COLLIDE_SQ_LENDGHT)
                {
                    temp.primaryIndex = i;
                    temp.secondaryIndex = j;
                    res.emplace_back(temp);
                }
            }
        }
    }

    return res;
}

void Physics::MakeCheckList()
{
    if (!m_isNoMake) return;

    // チェックリストの初期化
    m_checkList.clear();

	// チェックリストの作成
    for (int i = 0; i < m_objects.size(); ++i)
    {
        for (int j = i + 1; j < m_objects.size(); ++j)
        {
            const auto& obj1 = m_objects[i];
            const auto& obj2 = m_objects[j];

            const auto priority1 = obj1->GetPriority();
            const auto priority2 = obj2->GetPriority();

            // 動かないオブジェクト同士ならリストに追加しない
            if (priority1 == Collidable::Priority::STATIC && priority2 == Collidable::Priority::STATIC) continue;

            CheckList data;
            data.primary = obj1;
            data.secondary = obj2;
            // obj1がobj2より優先度が低い場合反転
            if (priority1 < priority2)
            {
                data.primary = obj2;
                data.secondary = obj1;
            }
            // 同じ優先度ならフラグをtrueに
            else if (priority1 == priority2)
            {
                data.isSamePriority = true;
            }

            // リストに追加
            m_checkList.emplace_back(data);
        }
    }

    m_isNoMake = false;
}

void Physics::AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, int colIndexA, int colIndexB, SendCollideInfo& info, const CollideHitInfo& hitInfo)
{
    // 既に追加されている通知リストにあれば追加しない
    for (auto& i : info)
    {
        if (i.own.lock() == objA.lock() && i.send.lock() == objB.lock() && i.ownColIndex == colIndexA && i.sendColIndex == colIndexB) return;
        if (i.own.lock() == objB.lock() && i.send.lock() == objA.lock() && i.ownColIndex == colIndexB && i.sendColIndex == colIndexA) return;
    }

    // ここまで来たらまだ通知リストに追加されていないため追加
    info.emplace_back(SendInfo{ objA, objB, colIndexA, colIndexB, hitInfo });
}

void Physics::CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger)
{
    // 1つ前に通知リストが当たったか
    auto isPreExist = preSendInfo.size() != 0;

    for (auto& info : newSendInfo)
    {
        bool isEntry = false;

        // 1つ前に通知リストがあった場合
        if (isPreExist)
        {
            bool isEnter = true;
            // 1つ前の通知リストをすべて回す
            auto it = preSendInfo.begin();
            for (; it != preSendInfo.end(); ++it)
            {
                // 通知リストが存在した場合は当たった時の通知を呼ばないようにする
                if (it->own.lock() == info.own.lock() && it->send.lock() == info.send.lock() && it->ownColIndex == info.ownColIndex && it->sendColIndex == info.sendColIndex) isEnter = false;
                if (it->own.lock() == info.send.lock() && it->send.lock() == info.own.lock() && it->ownColIndex == info.sendColIndex && it->sendColIndex == info.ownColIndex) isEnter = false;
                if (!isEnter) break;
            }
            if (isEnter)
            {
                // 当たった時の通知を追加
                if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerEnter);
                else           AddOnCollideInfo(info, OnCollideInfoKind::CollideEnter);
            }
            else
            {
                // 一つ前の通知リストから今回存在する通知のものを削除する
                preSendInfo.erase(it);
            }
        }
        // 1つ前に通知リストがなかった場合
        else
        {
            // 当たった時の通知を追加
            if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerEnter);
            else           AddOnCollideInfo(info, OnCollideInfoKind::CollideEnter);
        }

        // 当たっている状態の通知を追加
        if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerStay);
        else           AddOnCollideInfo(info, OnCollideInfoKind::CollideStay);
    }

    // 上で削除されずに残った1つ前の通知リストは今回抜けているため
    // 離れた時の通知を追加
    for (auto& info : preSendInfo)
    {
        if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerExit);
        else           AddOnCollideInfo(info, OnCollideInfoKind::CollideExit);
    }
}

void Physics::AddOnCollideInfo(const SendInfo& info, OnCollideInfoKind kind)
{
    m_onCollideInfo.emplace_back(OnCollideInfoData{info.own, info.send, info.ownColIndex, info.sendColIndex, info.hitInfo, kind});
    m_onCollideInfo.emplace_back(OnCollideInfoData{info.send, info.own, info.sendColIndex, info.ownColIndex, info.hitInfo, kind});
}

void Physics::OnCollideInfo(const std::weak_ptr<Collidable>& own, const std::weak_ptr<Collidable>& send, int ownIndex, int sendIndex, const CollideHitInfo& hitInfo, OnCollideInfoKind kind)
{
    // 送る側、受け取る側どちらかのリンクが途切れていたら通知をやめる
    if (own.expired() || send.expired()) return;

    // 種類に合わせて通知を送る
    if (kind == OnCollideInfoKind::CollideEnter)
    {
        own.lock()->OnCollideEnter(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
    else if (kind == OnCollideInfoKind::CollideStay)
    {
        own.lock()->OnCollideStay(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
    else if (kind == OnCollideInfoKind::CollideExit)
    {
        own.lock()->OnCollideExit(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
    else if (kind == OnCollideInfoKind::TriggerEnter)
    {
        own.lock()->OnTriggerEnter(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
    else if (kind == OnCollideInfoKind::TriggerStay)
    {
        own.lock()->OnTriggerStay(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
    else if (kind == OnCollideInfoKind::TriggerExit)
    {
        own.lock()->OnTriggerExit(send.lock().get(), ownIndex, sendIndex, hitInfo);
    }
}

void Physics::FixPos() const
{
    for (const auto& item : m_objects)
    {
        auto& rigid = item->m_rigid;
        // 座標の確定
        rigid.SetPos(rigid.GetNextPos());
#ifdef _DEBUG
        // デバック情報
        AddDebugInfo(rigid.GetPos(), item->m_collider, DebugDraw::COL_AFFTER);
#endif
    }
}

#ifdef _DEBUG
void MyEngine::Physics::AddDebugInfo(const Vec3& pos, const std::vector<std::shared_ptr<ColliderBase>>& colliders, unsigned int color) const
{
    auto& debug = DebugDraw::GetInstance();

    for (auto& col : colliders)
    {
        auto kind = col->GetKind();
        if (kind == ColKind::SPHERE)
        {
            auto data = dynamic_cast<ColliderSphere*>(col.get());
            debug.DrawSphere(pos + data->center, data->radius, color, false);
        }
        else if (kind == ColKind::CAPSULE)
        {
            auto data = dynamic_cast<ColliderCapsule*>(col.get());
            debug.DrawCapsule(pos + data->center, data->dir, data->radius, data->size, color, false);
        }
        else if (kind == ColKind::BOX)
        {
            auto data = dynamic_cast<ColliderBox*>(col.get());
            debug.DrawCube(pos + data->center, data->size, data->rotation, color, false);
        }
    }
}
#endif