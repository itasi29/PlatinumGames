#include "SceneBase.h"
#include "SceneManager.h"

SceneBase::SceneBase(SceneKind kind) :
	m_scnMgr(SceneManager::GetInstance()),
	m_nowSceneKind(kind)
{
}
