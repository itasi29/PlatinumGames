#include "ColliderBase.h"
#include <cassert>
#include "CollideHelp.h"

using namespace MyEngine;

ColliderBase::ColliderBase(ColKind kind) :
	isTrigger(false),
	center(Vec3()),
	m_kind(kind)
{
	// 判定関数の登録
	m_collideFunc[ColKind::SPHERE] = &ColliderBase::IsCollideSphere;
	m_collideFunc[ColKind::CAPSULE] = &ColliderBase::IsCollideCapsule;
	m_collideFunc[ColKind::BOX] = &ColliderBase::IsCollideBox;
	// 修正関数の登録
	m_fixFunc[ColKind::SPHERE] = &ColliderBase::FixToSphere;
	m_fixFunc[ColKind::CAPSULE] = &ColliderBase::FixToCapsule;
}

MyEngine::ColliderBase::ColliderBase(ColKind kind, const Vec3& center, bool isTrigger) :
	isTrigger(isTrigger),
	center(center),
	m_kind(kind)
{
	// 判定関数の登録
	m_collideFunc[ColKind::SPHERE] = &ColliderBase::IsCollideSphere;
	m_collideFunc[ColKind::CAPSULE] = &ColliderBase::IsCollideCapsule;
	m_collideFunc[ColKind::BOX] = &ColliderBase::IsCollideBox;
	// 修正関数の登録
	m_fixFunc[ColKind::SPHERE] = &ColliderBase::FixToSphere;
	m_fixFunc[ColKind::CAPSULE] = &ColliderBase::FixToCapsule;
}

CollideHitInfo ColliderBase::IsCollide(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	// 対象の種類に応じて判定関数を呼び出す
	auto kind = colliderCol->GetKind();
	return (this->*m_collideFunc.at(kind))(pos, rigid, colliderCol, colliderPos, colliderRigid, preInfo);
}

const Vec3& MyEngine::ColliderBase::FixNextPos(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	// 対象の種類に応じて修正関数を呼び出す
	auto kind = colliderCol->GetKind();
	return (this->*m_fixFunc.at(kind))(pos, colliderCol, colliderPos, hitInfo);
}

CollideHitInfo ColliderBase::IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	assert(false && "実装されていない判定です");
	return CollideHitInfo{false};
}

CollideHitInfo ColliderBase::IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	assert(false && "実装されていない判定です");
	return CollideHitInfo{ false };
}

CollideHitInfo ColliderBase::IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	assert(false && "実装されていない判定です");
	return CollideHitInfo{ false };
}

Vec3 ColliderBase::FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	assert(false && "実装されていない修正関数です");
	return colliderPos;
}

Vec3 ColliderBase::FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	assert(false && "実装されていない修正関数です");
	return colliderPos;
}
