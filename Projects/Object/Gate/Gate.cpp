#include "Gate.h"
#include "Collider/ColliderCapsule.h"
#include "Physics.h"
#include "Files.h"
#include "SoundManager.h"
#include "Matrix4x4.h"
#include "MathHelp.h"
#include "Game.h"
#include "GateCamera.h"
#include "Object/Player.h"
#include "DebugDraw.h"

namespace
{
	// カプセルの長さ
	constexpr float CAPSULE_SIZE = 4.0f;
	// 半径
	constexpr float RADIUS = 2.0f;
	// ワープ時の加速調整係数
	constexpr float ADD_WARP_VEL_RATE = 0.75f;
}

Gate::Gate(const std::shared_ptr<GateCamera>& camera, const std::shared_ptr<GateCamera>& cameraFromPair, GateKind kind) :
	Object3DBase(Priority::STATIC, ObjectTag::GATE),
	m_updateFunc(&Gate::NormalUpdate),
	m_camera(camera),
	m_cameraFromPair(cameraFromPair),
	m_kind(kind),
	m_hitObjTag(ObjectTag::WALL)
{
	// シェーダー定数バッファ作成
	m_cbuffH = CreateShaderConstantBuffer(sizeof(UserData));
	m_userData = static_cast<UserData*>(GetBufferShaderConstantBuffer(m_cbuffH));
}

Gate::~Gate()
{
	DeleteShaderConstantBuffer(m_cbuffH);
}

void Gate::Init(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir, const std::weak_ptr<Player>& player)
{
	// Physicsに登録
	OnEntryPhysics();

	// ファイル読み込み
	auto& fileMgr = FileManager::GetInstance();
	m_dissolveFile = fileMgr.Load(I_DISSOLVE);
	m_warpSe = fileMgr.Load(S_WARP);
	// 種類によってモデルを読み込む
	int id = M_GATE_ORANGE;
	if (m_kind == GateKind::Blue) id = M_GATE_BLUE;
	LoadModel(id);

	// 重力設定
	m_rigid.SetGravity(false);
	// コライダー設定
	m_collider = std::dynamic_pointer_cast<MyEngine::ColliderCapsule>(CreateCollider(MyEngine::ColKind::CAPSULE));
	m_collider->isTrigger = true;
	m_collider->size = CAPSULE_SIZE;
	m_collider->radius = RADIUS;

	// プレイヤー設定
	m_player = player;

	// モデル情報初期化
	ChangePos(hitObjTag, pos, norm, dir);
	m_scale *= 0.01f * CAPSULE_SIZE;
}

void Gate::End()
{
	Object3DBase::End();
	m_pairGate.reset();
	m_pairGate = nullptr;
}

void Gate::ChangePos(ObjectTag hitObjTag, const Vec3& pos, const Vec3& norm, const Vec3& dir)
{
	// ヒットオブジェクトタグ設定
	m_hitObjTag = hitObjTag;
	// 位置、法線、方向設定
	m_norm = norm.GetNormalized();
	m_rigid.SetPos(pos);
	m_collider->dir = dir;
	m_right = Vec3::Cross(dir, m_norm);
	m_updateFunc = &Gate::OpenUpdate;

	m_rotation = Quaternion::GetQuaternion(m_norm, Vec3::Front());
	// 壁に設置していない場合
	if (dir.y == 0.0f)
	{
		m_rotation = Quaternion::GetQuaternion(Vec3::Front(), dir) * m_rotation;
	}

	// シェーダに送る情報設定
	m_userData->center = pos;
	m_userData->dir = dir;
	m_userData->size = CAPSULE_SIZE;
	m_userData->radius = RADIUS * 0.95f;
	m_userData->frame = 0;
}

void Gate::SetCameraInfo()
{
	if (!m_pairGate) return;

	// カメラの位置を更新
	const auto& pos = m_rigid.GetPos();
	m_camera->SetTargetPos(pos);
	m_cameraFromPair->SetTargetPos(pos);

	// ペアのゲートのY軸の法線方向に合わせて角度を取得
	const auto& pairNorm = m_pairGate->GetNorm();
	const float vertexAngle = std::abs(pairNorm.y) * -1.0f * 90.0f;

	// 見る方向は法線方向
	m_camera->SetBase(m_norm, vertexAngle);
	// 見る方向は自身からペアの方向に向かうベクトルを法線方向に回転させて実行
	auto dir = m_pairGate->GetPos() - pos;
	dir.Normalize();
	auto angle = std::acosf(Vec3::Dot(-m_norm, pairNorm)) * Game::RAD_2_DEG;
	auto axis = Vec3::Cross(pairNorm, -m_norm);
	if (!axis.SqLength()) axis = m_collider->dir;
	m_cameraFromPair->SetBase(Quaternion::AngleAxis(angle, axis) * dir, vertexAngle);
}

void Gate::Update()
{
	// シェーダーに送る情報更新
	++m_userData->frame;
	m_userData->isCreate = m_pairGate != nullptr;

	// ペアのゲートが存在しない場合終了
	if (!m_pairGate) return;

	// カメラの向き更新
	const auto& pairNorm = m_pairGate->GetNorm();
	const auto& playerCameraInfo = m_player.lock()->GetCamera()->GetInfo();
	m_camera->Update(playerCameraInfo, pairNorm);
	m_cameraFromPair->Update(playerCameraInfo, pairNorm);

#if false
	// カメラの場所を更新
	const auto& pairToPlayer = m_pairGate->GetPos() - m_player.lock()->GetPos();
	const auto& projNorm = Vec3::Projection(pairToPlayer, pairNorm);
	const auto& projRight = Vec3::Projection(pairToPlayer, m_pairGate->GetRight());
	auto sizeNorm = projNorm.Length();
	auto sizeRight = projRight.Length();
	if (std::signbit(projNorm.y)) sizeNorm *= -1;
	if (std::signbit(projRight.y)) sizeRight *= -1;
	const auto& pos = m_rigid.GetPos();
	m_camera->SetTargetPos(pos + m_norm * sizeNorm - m_right * sizeRight);
#endif
}

void Gate::DrawGate(int tex) const
{
	// シェーダーに送る情報更新
	UpdateShaderConstantBuffer(m_cbuffH);
	SetShaderConstantBuffer(m_cbuffH, DX_SHADERTYPE_PIXEL, 5);

	// -1の場合はデフォルトのテクスチャを使用
	if (tex < 0) tex = MV1GetTextureGraphHandle(m_modelHandle, 0);
	// シェーダにテクスチャを設定
	SetUseTextureToShader(1, tex);
	SetUseTextureToShader(2, m_dissolveFile->GetHandle());
	// 描画
	Object3DBase::Draw();
	// テクスチャを解除
	SetUseTextureToShader(1, -1);
	SetUseTextureToShader(2, -1);

#ifdef _DEBUG
	auto& debug = MyEngine::DebugDraw::GetInstance();
	debug.DrawLine(m_rigid.GetPos(), m_rigid.GetPos() + m_norm * 5, 0x00ff00);
#endif
}

bool Gate::CheckWarp(const Vec3& targetPos)
{
	// ゲートから対象に向くベクトルとゲートの法線の内積が-になったらワープ可能
	const auto& gateTotarget = targetPos - (m_rigid.GetPos() + m_collider->center);
	auto dot = Vec3::Dot(m_norm, gateTotarget);
	return dot < 0.0f;
}

void Gate::OnWarp(const Vec3& targetPos, MyEngine::Rigidbody& targetRigid, bool isChangeVel, float height)
{
	// 音鳴らす
	SoundManager::GetInstance().PlaySe(m_warpSe->GetHandle());

	/* 場所変換 */
	const auto& gateToTarget = targetPos - m_rigid.GetPos();
	// ゲート同士の法線、上下方向の回転具合を取得
	const auto& normRot = Quaternion::GetQuaternion(m_norm, m_pairGate->GetNorm());
	const auto& upRot = Quaternion::GetQuaternion(m_collider->dir, m_pairGate->GetCol()->dir);
	// ゲート上での右方向・上方向動いている大きさを取得
	const auto& right = normRot * Vec3::Projection(gateToTarget, m_right);
	const auto& up    = upRot * Vec3::Projection(gateToTarget, m_collider->dir);
	// 修正位置の取得
	const auto& pairNorm = m_pairGate->GetNorm();
	constexpr float MIN_FIX_SIZE = 0.5f;
	float size = std::max<float>(MIN_FIX_SIZE, std::abs(pairNorm.y * height));
	auto fixPos = m_pairGate->GetPos() - right + up + pairNorm * size;
	targetRigid.SetPos(fixPos);
	
	/* 速度変換 */
	// 速度を変更する場合
	if (isChangeVel)
	{
		const auto& vel = targetRigid.GetVelocity();
		const auto& velN = vel.GetNormalized();

		auto rot1 = Quaternion::GetQuaternion(-m_norm, m_pairGate->GetNorm(), m_collider->dir);
		auto newVel = rot1 * velN * std::abs(vel.y * ADD_WARP_VEL_RATE);
		targetRigid.SetVelocity(newVel);
	}
	// 変更しない場合
	else
	{
		const auto& vel = targetRigid.GetVelocity();

		auto rot1 = Quaternion::GetQuaternion(-m_norm, m_pairGate->GetNorm(), m_collider->dir);
		auto newVel = rot1 * vel;
		targetRigid.SetVelocity(newVel);
	}
	
}

void Gate::OpenUpdate()
{
}

void Gate::NormalUpdate()
{
}

void Gate::CloseUpdate()
{
}
