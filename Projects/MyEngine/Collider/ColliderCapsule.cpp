#include "Colliders.h"
#include "CollideHelp.h"

using namespace MyEngine;

ColliderCapsule::ColliderCapsule() :
	ColliderBase(ColKind::CAPSULE),
	dir(Vec3(0, 0, 1)),
	radius(0.0f),
	size(0.0f)
{
}

MyEngine::ColliderCapsule::ColliderCapsule(const Vec3& center, const Vec3& dir, float radius, float size, bool isTrigger) :
	ColliderBase(ColKind::CAPSULE, center, isTrigger),
	dir(dir),
	radius(radius),
	size(size)
{
}

void ColliderCapsule::Init(const Vec3& center, const Vec3& dir, float radius, float size, bool isTrigger)
{
	this->center = center;
	this->dir = dir;
	this->radius = radius;
	this->size = size;
	this->isTrigger = isTrigger;
}

CollideHitInfo ColliderCapsule::IsCollideSphere(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return IsCollideSphereAndCapsule(colliderPos, pos, col, this);
}

CollideHitInfo ColliderCapsule::IsCollideCapsule(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return IsCollideCapsuleAndCapsule(pos, colliderPos, this, col);
}

CollideHitInfo MyEngine::ColliderCapsule::IsCollideBox(const Vec3& pos, const Rigidbody& rigid, ColliderBase* colliderCol, const Vec3& colliderPos, const Rigidbody& colliderRigid, const PreHitInfo& preInfo)
{
	auto col = dynamic_cast<ColliderBox*>(colliderCol);
	return IsCollideCapsuleAndBox(pos, colliderPos, this, col, rigid, preInfo);
}

Vec3 MyEngine::ColliderCapsule::FixToSphere(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderSphere*>(colliderCol);
	return FixCapsuleToSphere(pos, colliderPos, this, col, hitInfo);
}

Vec3 ColliderCapsule::FixToCapsule(const Vec3& pos, ColliderBase* colliderCol, const Vec3& colliderPos, const CollideHitInfo& hitInfo)
{
	auto col = dynamic_cast<ColliderCapsule*>(colliderCol);
	return FixCapsuleToCapsule(pos, colliderPos, this, col, hitInfo);
}
