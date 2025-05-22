#include "Colliders.h"
#include "CollideHelp.h"

using namespace MyEngine;

ColliderBox::ColliderBox() :
	ColliderBase(ColKind::BOX)
{
}

MyEngine::ColliderBox::ColliderBox(const Vec3& center, const Vec3& size, const Quaternion& rot, const Vec3& norm, bool isTrigger) :
	ColliderBase(ColKind::BOX, center, isTrigger),
	size(size),
	rotation(rot),
	norm(norm)
{
}

CollideHitInfo ColliderBox::IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return IsCollideSphereAndBox(colliderPos, pos, col, this, colliderRigid, preInfo);
}

CollideHitInfo MyEngine::ColliderBox::IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return IsCollideCapsuleAndBox(colliderPos, pos, col, this, colliderRigid, preInfo);
}

Vec3 ColliderBox::FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return FixBoxToSphere(pos, colliderPos, this, col, hitInfo);
}

Vec3 MyEngine::ColliderBox::FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return FixBoxToCapsule(pos, colliderPos, this, col, hitInfo);
}
