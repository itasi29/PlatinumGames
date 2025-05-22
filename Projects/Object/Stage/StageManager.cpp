#include "StageManager.h"
#include <list>
#include "Utility.h"
#include "StringUtility.h"
#include "DebugDraw.h"
#include "Files.h"
#include "Application.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "AFile.h"
#include "CheckPoint.h"
#include "Collider/Colliders.h"
#include "Object/HandObject.h"
#include "Object/Glass.h"
#include "Object/Gimmick/Door.h"
#include "Object/Gimmick/Button.h"
#include "Object/Gimmick/Piston.h"
#include "Object/Gate/GateDelete.h"
#include "Object/Gimmick/Turret/Turret.h"
#include "Object/Gimmick/Laser/LaserCatcher.h"
#include "Object/Gimmick/Laser/LaserLaunchPad.h"
#include "Object/Gimmick/MoveFloor/MoveFloorManager.h"

namespace
{
	enum AFileKind : int
	{
		OBJECT,
		GIMMICK,
		MOVE_FLOOR,
		CP,
		LIGHT,
	};

	const wchar_t* const BASE_PATH = L"Data/Stage/";
	const wchar_t* const MODEL_INFO_PATH = L"StageModel/StageModelInfo";
	const wchar_t* const GIMMICK_INFO_PATH = L"StageGimmick/StageGimmickInfo";
	const wchar_t* const FLOOR_MOVE_INFO_PATH = L"FloorMove/FloorMoveInfo";
	const wchar_t* const CHECK_POINT_INFO_PATH = L"CheckPoint/CheckPointInfo";
	const wchar_t* const MODEL_ID_INFO_PATH = L"StageModelId/StageModelIdInfo";
	const wchar_t* const LIGHT_INFO_PATH = L"Lights/LightsInfo";
	const wchar_t* const EXTENSION = L".loc";

	const std::unordered_map<std::string, ObjectTag> OBJECT_TAG =
	{
		{ "Floor", ObjectTag::FLOOR },
		{ "Wall", ObjectTag::WALL },
		{ "SystemWall", ObjectTag::WALL },
		{ "Roof", ObjectTag::ROOF },
		{ "NoGateFloor", ObjectTag::NO_GATE_FLOOR },
		{ "NoGateWall", ObjectTag::NO_GATE_WALL },
		{ "NoGateRoof", ObjectTag::NO_GATE_ROOF },
		{ "PerforatedWall", ObjectTag::PERFORATED_WALL },
		{ "DoorArch", ObjectTag::DOOR_ARCH },
		{ "Exit", ObjectTag::NONE },
		{ "Pillar", ObjectTag::NONE },
		{ "Stairs", ObjectTag::STAIRS },
		{ "Catwalk", ObjectTag::CATWALK },
		{ "CatwalkShort", ObjectTag::CATWALK },
		{ "CatwalkFence", ObjectTag::CATWALK_FENCE },
		{ "CatwalkFenceShort", ObjectTag::CATWALK_FENCE },
		{ "CatwalkPillar", ObjectTag::CATWALK_PILLAR },
		{ "Bed" , ObjectTag::NONE },
		{ "Chair" , ObjectTag::NONE },
		{ "Torpedo" , ObjectTag::NONE },
	};
	const std::unordered_map<std::string, int> MODEL_ID =
	{
		{ "Floor", M_FLOOR },
		{ "Wall", M_WALL },
		{ "SystemWall", M_WALL },
		{ "Roof", M_ROOF },
		{ "NoGateFloor", M_NO_GATE_FLOOR },
		{ "NoGateWall", M_NO_GATE_WALL },
		{ "NoGateRoof", M_NO_GATE_ROOF },
		{ "PerforatedWall", M_PERFORATED_WALL },
		{ "Door", M_DOOR },
		{ "DoorArch", M_DOOR_ARCH },
		{ "Exit", M_EXIT },
		{ "Pillar", M_PILLAR },
		{ "Stairs", M_STAIRS },
		{ "Catwalk", M_CATWALK },
		{ "CatwalkShort", M_CATWALK_SHORT },
		{ "CatwalkFence", M_CATWALK_FENCE },
		{ "CatwalkFenceShort", M_CATWALK_FENCE_SHORT },
		{ "CatwalkPillar", M_CATWALK_PILLAR },
		{ "HandObj", M_HAND_OBJ },
		{ "Button", M_BUTTON },
		{ "Turret", M_TURRET },
		{ "LaserLaunchPad", M_LASER_LAUNCH_PAD },
		{ "LaserCatcher", M_LASER_CATHCER },
		{ "Piston", M_PISTON },
		{ "Glass", M_GLASS },
		{ "Bed" , M_BED },
		{ "Chair" , M_CHAIR },
		{ "Torpedo" , M_TORPEDO },
		{ "FloorMove" , M_FLOOR },
	};
}

StageManager::StageManager(const int stageNo) :
	m_lights(nullptr),
	m_lightNum(0),
	m_stageNo(stageNo),
	m_nowCp(0),
	m_isClear(false)
{
	m_createFunc[MyEngine::ColKind::BOX] = &StageManager::LoadBoxColInfo;
	m_createFunc[MyEngine::ColKind::SPHERE] = &StageManager::LoadSphereColInfo;
	m_createFunc[MyEngine::ColKind::CAPSULE] = &StageManager::LoadCapsuleColInfo;
}

StageManager::~StageManager()
{
	
}

void StageManager::AsyncInit()
{
	// オブジェクト
	std::wostringstream object;
	object << BASE_PATH << MODEL_INFO_PATH << m_stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(object.str().c_str()));
	// ギミック
	std::wostringstream gimmick;
	gimmick << BASE_PATH << GIMMICK_INFO_PATH << m_stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(gimmick.str().c_str()));
	// 動く床
	std::wostringstream move;
	move << BASE_PATH << FLOOR_MOVE_INFO_PATH << m_stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(move.str().c_str()));
	// チェックポイント
	std::wostringstream cp;
	cp << BASE_PATH << CHECK_POINT_INFO_PATH << m_stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(cp.str().c_str()));
	// ライト
	std::wostringstream light;
	light << BASE_PATH << LIGHT_INFO_PATH << m_stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(light.str().c_str()));
	// モデル読み込み
	std::list<int> loadList;
	// 非同期読み込みを無効に
	SetUseASyncLoadFlag(false);
	std::wostringstream modelId;
	modelId << BASE_PATH << MODEL_ID_INFO_PATH << m_stageNo << EXTENSION;
	int handle = FileRead_open(modelId.str().c_str());
	int size = 0;
	FileRead_read(&size, sizeof(size), handle);
	for (int i = 0; i < size; ++i)
	{
		// タグ読み込み
		uint8_t tagSize;
		FileRead_read(&tagSize, sizeof(uint8_t), handle);
		std::string tag;
		tag.resize(tagSize);
		FileRead_read(tag.data(), sizeof(char) * tagSize, handle);

		// GateDeleteはスキップする
		if (tag == "GateDelete") continue;
		// ロードリストに追加
		loadList.emplace_back(MODEL_ID.at(tag));
	}
	FileRead_close(handle);
	// 非同期読み込みを有効に
	SetUseASyncLoadFlag(true);
	auto& fileMgr = FileManager::GetInstance();
	// ファイル読み込み
	for (const auto& id : loadList) m_files.push_back(fileMgr.Load(id));
}

void StageManager::Init(Player* player, GateManager* gateMgr)
{
	LoadAndCreateObject(player, gateMgr);
	LoadGimmickLinkInfo();
	LoadFloorMoveInfo();
	LoadCheckPoint();
	LoadLightInfo();

	for (auto& info : m_afiles) info->Close();
	m_afiles.clear();
}

void StageManager::Update()
{
	for (const auto& obj : m_objs)	obj->Update();
	auto it = std::remove_if(m_objs.begin(), m_objs.end(),
		[](const auto& obj)
		{
			if (!obj->IsExist())
			{
				obj->End();
				return true;
			}
			return false;
		});
	m_objs.erase(it, m_objs.end());
}

void StageManager::Draw() const
{
	for (const auto& obj : m_objs)	obj->Draw();
#ifdef _DEBUG
	auto& debug = MyEngine::DebugDraw::GetInstance();
	for (int i = 0; i < m_lightPos.size(); ++i)
	{
#if true
		debug.DrawSphere(m_lightPos[i], 1.0f, 0xffff00, false);
		debug.DrawSphere(m_lightPos[i], m_lightRange[i], 0x00ffff, false);
#else
		DrawSphere3D(m_lightPos[i].VGet(), 1.0f, 16, 0xffff00, 0, false);
		DrawSphere3D(m_lightPos[i].VGet(), m_lightRange[i], 16, 0x00ffff, 0, false);
#endif
	}
#endif
}

void StageManager::End()
{
	for (auto& obj : m_objs)		obj->End();
	for (auto& cp : m_checkPoints)	cp->End();
	delete[] m_lights;
}

void StageManager::UpdateCheckPoint(int checkNo)
{
	if (m_isClear) return;

	// 現在のチェックポイントの方が先に進んでいたら更新しない
	if (m_nowCp > checkNo) return;
	// チェックポイントの更新
	m_nowCp = checkNo;
}

void StageManager::Restart()
{
	for (auto& obj : m_objs)	obj->Restart();
}

bool StageManager::CheckClear()
{
	m_isClear = m_nowCp + 1 >= m_checkPoints.size();
	return m_isClear;
}

void StageManager::AddObject(std::shared_ptr<Object3DBase> object)
{
	m_objs.emplace_back(object);
}

void StageManager::AppLights() const
{
	Application::GetInstance().SetLightBuffer(m_lights, m_lightNum);
}

const Vec3& StageManager::GetCheckPoint() const
{
	return m_checkPoints.at(m_nowCp)->GetPos();
}

const Vec3& StageManager::GetCheckPointDir() const
{
	return m_checkPoints.at(m_nowCp)->GetDir();
}

void StageManager::LoadAndCreateObject(Player* player, GateManager* gateMgr)
{
	auto& file = m_afiles.at(AFileKind::OBJECT);

	int size;
	file->Read(&size, sizeof(int));
	for (int i = 0; i < size; ++i)
	{
		// タグ読み込み
		uint8_t tagSize = 0;
		file->Read(&tagSize, sizeof(uint8_t));
		std::string tag;
		tag.resize(tagSize);
		file->Read(tag.data(), sizeof(char) * tagSize);
		// データ読み込み
		ModelData data;
		file->Read(&data, sizeof(ModelData));

		// 判定データ読み込み
		std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list;
		int colSize = 0;
		file->Read(&colSize, sizeof(int));
		for (int i = 0; i < colSize; ++i)
		{
			MyEngine::ColKind kind;
			file->Read(&kind, sizeof(MyEngine::ColKind));
			const auto& func = m_createFunc.at(kind);
			auto res = (this->*func)(file, data);
			list.emplace_back(Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>{ kind, res });
		}

		// オブジェクト生成
		std::shared_ptr<Object3DBase> obj;
		bool isGravity = false;
		bool isLoadModel = true;
		if (tag == "Turret")
		{
			// 向いている方向を取得
			Vec3 dir;
			file->Read(&dir, sizeof(Vec3));
			// 適用
			auto turret = std::make_shared<Turret>();
			turret->Init(dir, player);
			obj = turret;
			// 重力使用
			isGravity = true;
		}
		else if (tag == "LaserLaunchPad")
		{
			// 発射方向、反射回数取得
			Vec3 dir;
			int reflectNum = 0;
			file->Read(&dir, sizeof(Vec3));
			file->Read(&reflectNum, sizeof(int));
			// 適用
			auto launchPad = std::make_shared<LaserLaunchPad>(gateMgr, reflectNum);
			launchPad->Init(dir);
			obj = launchPad;
		}
		else if (tag == "HandObj")
		{
			obj = std::make_shared<HandObject>(*gateMgr);
			// 重力使用
			isGravity = true;
		}
		else if (tag == "GateDelete")
		{
			obj = std::make_shared<GateDelete>(*gateMgr);
			// モデル不使用
			isLoadModel = false;
		}
		else if (tag == "FloorMove")
		{
			obj = std::make_shared<MoveFloorManager>();
			// モデル不使用
			isLoadModel = false;
		}
		else if (tag == "LaserCatcher") obj = std::make_shared<LaserCatcher>();
		else if (tag == "Piston")		obj = std::make_shared<Piston>();
		else if (tag == "Button")		obj = std::make_shared<Button>();
		else if (tag == "Door")			obj = std::make_shared<Door>();
		else if (tag == "Glass")		obj = std::make_shared<Glass>();
		else							obj = std::make_shared<Object3DBase>(MyEngine::Collidable::Priority::STATIC, OBJECT_TAG.at(tag));

		// モデル適用
		if (isLoadModel) obj->LoadModel(MODEL_ID.at(tag));
		// 初期化
		obj->Init(data.pos, data.scale * 0.01f, data.rot, list, isGravity);
		// 追加
		m_objs.emplace_back(obj);
	}
}

void StageManager::LoadGimmickLinkInfo()
{
	auto& file = m_afiles.at(AFileKind::GIMMICK);
	int size;
	file->Read(&size, sizeof(int));
	for (int i = 0; i< size; ++i)
	{
		// 送る側のオブジェクト番号取得
		int gimmickNo;
		file->Read(&gimmickNo, sizeof(int));
		// 受け取る側のオブジェクト番号取得
		int linkNo;
		file->Read(&linkNo, sizeof(int));

		// ギミックをリンクさせる
		auto gimmick = std::dynamic_pointer_cast<GimmickSendObject>(m_objs.at(gimmickNo));
		auto link = std::dynamic_pointer_cast<GimmickLinkObject>(m_objs.at(linkNo));
		gimmick->SetLinkObject(link.get());
	}
}

void StageManager::LoadFloorMoveInfo()
{
	auto& file = m_afiles.at(AFileKind::MOVE_FLOOR);
	int size;
	file->Read(&size, sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		// ポジション数読み込み
		int num;
		file->Read(&num, sizeof(int));
		// ポジションデータ読み込み
		std::vector<Vec3> posTable(num);
		file->Read(posTable.data(), sizeof(Vec3) * num);
		// オブジェクト番号取得
		int index;
		file->Read(&index, sizeof(int));
		// ループフラグ取得
		bool isLoop;
		file->Read(&isLoop, sizeof(bool));

		auto obj = m_objs.at(index);
		if (obj->GetTag() == ObjectTag::PISTON)
		{
			auto piston = std::dynamic_pointer_cast<Piston>(obj);
			piston->SetMoveRange(posTable[0], posTable[1]);
		}
		else
		{
			auto moveFloorMgr = std::dynamic_pointer_cast<MoveFloorManager>(obj);
			moveFloorMgr->SetCheckPtTable(posTable);
			moveFloorMgr->SetLoop(isLoop);
		}
	}
}

void StageManager::LoadCheckPoint()
{
	auto& file = m_afiles.at(AFileKind::CP);
	int size;
	file->Read(&size, sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		// データ読み込み
		Vec3 pos;
		Vec3 dir;
		float size;
		float radius;
		Vec3 respawnDir;
		file->Read(&pos, sizeof(Vec3));
		file->Read(&dir, sizeof(Vec3));
		file->Read(&size, sizeof(float));
		file->Read(&radius, sizeof(float));
		file->Read(&respawnDir, sizeof(Vec3));

		// CP作成
		auto obj = std::make_shared<CheckPoint>(*this, i);
		obj->Init(pos, dir, size, radius, respawnDir);

		// リンクがある場合
		bool isLink;
		file->Read(&isLink, sizeof(bool));
		if (isLink)
		{
			// リンクオブジェクト番号取得
			int linkNo;
			file->Read(&linkNo, sizeof(int));
			// リンクオブジェクト設定
			auto link = dynamic_cast<GimmickLinkObject*>(m_objs.at(linkNo).get());
			obj->SetLinkObj(link);
		}

		m_checkPoints.emplace_back(obj);
	}
}

void StageManager::LoadLightInfo()
{
	auto& file = m_afiles.at(AFileKind::LIGHT);
	// ライト数取得
	file->Read(&m_lightNum, sizeof(int));
	// ライト作成
	m_lights = new Light[m_lightNum];
	// データ読み込み
	for (int i = 0; i < m_lightNum; ++i)
	{
		auto& light = m_lights[i];
		file->Read(&light.pos, sizeof(Vec3));
		file->Read(&light.range, sizeof(float));
		file->Read(&light.intensity, sizeof(float));
		file->Read(light.color, sizeof(float) * 3);
#ifdef _DEBUG
		m_lightPos.emplace_back(light.pos);
		m_lightRange.emplace_back(light.range);
#endif
	}
}

MyEngine::ColliderBase* StageManager::LoadBoxColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderBox;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->size, sizeof(Vec3));
	file->Read(&col->norm, sizeof(Vec3));
	file->Read(&col->rotation, sizeof(Quaternion));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}

MyEngine::ColliderBase* StageManager::LoadSphereColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderSphere;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->radius, sizeof(float));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}

MyEngine::ColliderBase* StageManager::LoadCapsuleColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderCapsule;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->dir, sizeof(Vec3));
	file->Read(&col->size, sizeof(float));
	file->Read(&col->radius, sizeof(float));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}
