#include "EffekseerManager.h"
#include <EffekseerForDXLib.h>
#include <algorithm>
#include "Geometry.h"
#include "SceneManager.h"
#include "FileManager.h"
#include "FileBase.h"

Effekseer::Matrix43 GetEffMat(const Matrix4x4& mat);
Effekseer::Vector3D GetEffVec(const Vec3& vec);

EffekseerManager::EffekseerManager()
{
}

EffekseerManager::~EffekseerManager()
{
}

EffekseerManager& EffekseerManager::GetInstance()
{
	static EffekseerManager instance;
	return instance;
}

void EffekseerManager::Init(int particle)
{
	// 引数：パーティクル数
	Effekseer_Init(particle);
	// 歪みエフェクトの初期化
	Effekseer_InitDistortion();
	// フルスク切り替え時におかしくならないように設定
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

void EffekseerManager::End()
{
	Effkseer_End();
}

void EffekseerManager::LoadFile(int id)
{
	auto isFind = m_files.find(id) != m_files.end();
	// ロード済みの場合、警告
	if (isFind) 
	{
		assert(false && "既にロードしています");
		return;
	}
	// ロードしていない場合、ロードする
	else
	{
		m_files[id] = FileManager::GetInstance().Load(id);
	}
}

void EffekseerManager::ReleaseFile()
{
	m_files.clear();
}

void EffekseerManager::Update()
{
	// オプションを開いていたら更新しない
	if (SceneManager::GetInstance().IsOpenOption()) return;

	// 更新
	UpdateEffekseer3D();
	// 再生終了したものを消す
	m_playList.remove_if(
		[](const auto& handle)
		{
			return IsEffekseer3DEffectPlaying(handle) == -1;
		});
}

void EffekseerManager::SyncCamera() const
{
	Effekseer_Sync3DSetting();
}

void EffekseerManager::Draw() const
{
	DrawEffekseer3D();
}

int EffekseerManager::Play(int id)
{
	// ハンドル取得
	auto handle = m_files.at(id)->GetHandle();
	// エフェクトの再生
	auto play = PlayEffekseer3DEffect(handle);
	// mapのプレイリストに追加
	m_playList.emplace_back(play);
	// ハンドルを返す
	return play;
}

void EffekseerManager::Stop(int handle)
{
	StopEffekseer3DEffect(handle);
}

void EffekseerManager::AllStop()
{
	// エフェクトの全停止
	for (auto& handle : m_playList)
	{
		StopEffekseer3DEffect(handle);
	}
	// listから全削除
	m_playList.clear();
}

void EffekseerManager::SetInfo(int handle, const Vec3& pos, const Quaternion& rot) const
{
	SetInfo(handle, pos, rot, Vec3(1.0f));
}

void EffekseerManager::SetInfo(int handle, const Vec3& pos, const Quaternion& rot, const Vec3& scale) const
{
	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y, pos.z);
	SetScalePlayingEffekseer3DEffect(handle, scale.x, scale.y, scale.z);
	GetEffekseer3DManager()->SetRotation(handle, GetEffVec(rot.GetAxis()), rot.GetRadian());
}

bool EffekseerManager::IsPlay(int handle) const
{
	return IsEffekseer3DEffectPlaying(handle) == 0;
}

Effekseer::Matrix43 GetEffMat(const Matrix4x4& mat)
{
	Effekseer::Matrix43 ret;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			ret.Value[y][x] = mat.m[y][x];
		}
	}

	return ret;
}

Effekseer::Vector3D GetEffVec(const Vec3& vec)
{
	return Effekseer::Vector3D(vec.x, vec.y, vec.z);
}