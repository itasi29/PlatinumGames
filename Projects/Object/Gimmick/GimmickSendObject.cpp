#include "GimmickSendObject.h"
#include "Collider/ColliderBox.h"

GimmickSendObject::GimmickSendObject(Priority priority, ObjectTag tag) :
	Object3DBase(priority, tag),
	m_linkObj(nullptr)
{
}

GimmickSendObject::~GimmickSendObject()
{
}
