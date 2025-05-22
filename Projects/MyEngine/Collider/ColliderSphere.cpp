#include "Colliders.h"
#include "CollideHelp.h"

using namespace MyEngine;

ColliderSphere::ColliderSphere() :
	ColliderBase(ColKind::SPHERE),
	radius(0.0f)
{
}

MyEngine::ColliderSphere::ColliderSphere(const Vec3& center, float radius, bool isTrigger) :
	ColliderBase(ColKind::SPHERE, center, isTrigger),
	radius(radius)
{
}

CollideHitInfo ColliderSphere::IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return IsCollideSphereAndSphere(pos, colliderPos, this, col);
}

CollideHitInfo ColliderSphere::IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return IsCollideSphereAndCapsule(pos, colliderPos, this, col);
}

CollideHitInfo MyEngine::ColliderSphere::IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderBox*>(colliderCol);
	return IsCollideSphereAndBox(pos, colliderPos, this, col, rigid, preInfo);
}

Vec3 ColliderSphere::FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return FixSphereToSphere(pos, colliderPos, this, col, hitInfo);
}

Vec3 MyEngine::ColliderSphere::FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return FixSphereToCapsule(pos, colliderPos, this, col, hitInfo);
}
