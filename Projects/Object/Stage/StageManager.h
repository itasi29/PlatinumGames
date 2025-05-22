#pragma once
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include "Vec3.h"
#include "Quaternion.h"

class AFile;
class FileBase;
class GateManager;
class Object3DBase;
class CheckPoint;
class Player;
struct Light;

namespace MyEngine
{
    class ColliderBase;
    enum class ColKind;
}

/// <summary>
/// ステージマネージャークラス
/// </summary>
class StageManager
{
private:
    struct ModelData
    {
        Vec3 pos;
        Vec3 scale;
        Quaternion rot;
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="stageNo">ステージ番号</param>
    StageManager(const int stageNo);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~StageManager();

    /// <summary>
    /// 非同期初期化
    /// </summary>
    void AsyncInit();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="player">プレイヤー</param>
    /// <param name="gateMgr">ゲートマネージャー</param>
    void Init(Player* player, GateManager* gateMgr);

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const;

    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// チェックポイントの更新
    /// </summary>
    /// <param name="checkNo">チェックポイント番号</param>
    void UpdateCheckPoint(int checkNo);

    /// <summary>
    /// リスタート処理
    /// </summary>
    void Restart();

    /// <summary>
    /// クリア判定
    /// </summary>
    /// <returns>true:クリア / false:未クリア</returns>
    bool CheckClear();

	/// <summary>
	/// オブジェクトの追加
	/// </summary>
    void AddObject(std::shared_ptr<Object3DBase> object);

    /// <summary>
    /// ライト情報の設定
    /// </summary>
    void AppLights() const;

    /// <summary>
    /// 現在のチェックポイント番号を取得する
    /// </summary>
    /// <returns>現在のチェックポイント番号</returns>
    int GetNowCp() const { return m_nowCp; }
    /// <summary>
    /// チェックポイントの位置を取得する
    /// </summary>
    /// <returns>チェックポイントの位置</returns>
    const Vec3& GetCheckPoint() const;
	/// <summary>
	/// 現在のチェックポイントの方向を取得する
    /// </summary>
	/// <return>現在のチェックポイントの方向</returns>
	const Vec3& GetCheckPointDir() const;

private:
    /// <summary>
    /// オブジェクトの読み込みと作成
    /// </summary>
    /// <param name="player">プレイヤー</param>
    /// <param name="gateMgr">ゲートマネージャー</param>
    void LoadAndCreateObject(Player* player, GateManager* gateMgr);
    /// <summary>
    /// ギミックリンク情報の読み込み
    /// </summary>
    void LoadGimmickLinkInfo();
    /// <summary>
    /// 移動床情報の読み込み
    /// </summary>
    void LoadFloorMoveInfo();
    /// <summary>
    /// チェックポイント情報の読み込み
    /// </summary>
    void LoadCheckPoint();
    /// <summary>
	/// ライト情報の読み込み
    /// </summary>
    void LoadLightInfo();

    /// <summary>
    /// ボックスコライダー情報の読み込み
    /// </summary>
    /// <param name="file">ファイル</param>
    /// <param name="data">モデルデータ</param>
    /// <returns>コライダー</returns>
    MyEngine::ColliderBase* LoadBoxColInfo(const std::shared_ptr<AFile>& file, const ModelData& data);
    /// <summary>
    /// スフィアコライダー情報の読み込み
    /// </summary>
    /// <param name="file">ファイル</param>
    /// <param name="data">モデルデータ</param>
    /// <returns>コライダー</returns>
    MyEngine::ColliderBase* LoadSphereColInfo(const std::shared_ptr<AFile>& file, const ModelData& data);
    /// <summary>
    /// カプセルコライダー情報の読み込み
    /// </summary>
    /// <param name="file">ファイル</param>
    /// <param name="data">モデルデータ</param>
    /// <returns>コライダー</returns>
    MyEngine::ColliderBase* LoadCapsuleColInfo(const std::shared_ptr<AFile>& file, const ModelData& data);

private:
    // コライダー作成関数マップ
    using CreateColliderFunc_t = MyEngine::ColliderBase* (StageManager::*)(const std::shared_ptr<AFile>&, const ModelData&);
    std::unordered_map<MyEngine::ColKind, CreateColliderFunc_t> m_createFunc;  

    // ファイルリスト
    std::list<std::shared_ptr<FileBase>> m_files;  
    // 非同期ファイルリスト
    std::vector<std::shared_ptr<AFile>> m_afiles;  
    // オブジェクトリスト
    std::vector<std::shared_ptr<Object3DBase>> m_objs;  
    // チェックポイントリスト
    std::vector<std::shared_ptr<CheckPoint>> m_checkPoints;  
    // ライト情報
    Light* m_lights;
    int m_lightNum;
#ifdef _DEBUG
    std::vector<Vec3> m_lightPos;
	std::vector<float> m_lightRange;
#endif

    // ステージ番号
    const int m_stageNo;

    // 現在のチェックポイント番号
    int m_nowCp;  

    // クリアフラグ
    bool m_isClear;  
};
