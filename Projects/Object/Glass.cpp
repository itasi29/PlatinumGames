#include "Glass.h"
#include <DxLib.h>

Glass::Glass() :
	Object3DBase(Priority::STATIC, ObjectTag::GLASS)
{
}

Glass::~Glass()
{
}

void Glass::Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity)
{
	Object3DBase::Init(pos, scale, rot, list, isGravity);
	// Zバッファを使用しない
	MV1SetWriteZBuffer(m_modelHandle, false);
}
