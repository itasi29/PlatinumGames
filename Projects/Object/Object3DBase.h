#pragma once
#include "Collidable.h"
#include <memory>
#include <unordered_map>
#include "Vec3.h"
#include "Quaternion.h"
#include "Utility.h"

class FileBase;

namespace MyEngine
{
    class ColliderBase;
    enum class ColKind;
}

/// <summary>
/// 3Dオブジェクトの基底クラス
/// </summary>
class Object3DBase : public MyEngine::Collidable
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="priority">優先度</param>
    /// <param name="tag">オブジェクトタグ</param>
    Object3DBase(Priority priority, ObjectTag tag);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~Object3DBase();

    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Init() {}

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="pos">位置</param>
    /// <param name="scale">スケール</param>
    /// <param name="rot">回転</param>
    /// <param name="list">コライダーリスト</param>
    /// <param name="isGravity">重力を使用するかどうか</param>
    virtual void Init(const Vec3& pos, const Vec3& scale, const Quaternion& rot, std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list, bool isGravity);

    /// <summary>
    /// 終了処理
    /// </summary>
    virtual void End() override;

    /// <summary>
    /// 再起動処理
    /// </summary>
    virtual void Restart() {}

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() {}

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw() const;

    /// <summary>
    /// モデルのロード
    /// </summary>
    /// <param name="fileId">ファイルID</param>
    /// <returns>true:成功 /false:失敗</returns>
    virtual bool LoadModel(int fileId);

    /// <summary>
    /// モデルがロードされているかどうかを取得する
    /// </summary>
    /// <returns>true:ロードされている / false:ロードされていない</returns>
    bool IsLoadModel() const { return m_isLoadModel; }
    /// <summary>
    /// 存在するかどうかを取得する
    /// </summary>
    /// <returns>true:存在する / false:存在しない</returns>
    bool IsExist() const { return m_isExist; }

protected:
    /// <summary>
    /// モデル情報を適用する
    /// </summary>
    virtual void AppModelInfo() const;

    /// <summary>
    /// モデルを描画する
    /// </summary>
    void DrawModel() const;

private:
    /// <summary>
    /// ボックスコライダーを作成する
    /// </summary>
    /// <param name="base">コライダーベース</param>
    void CreateColldierBox(MyEngine::ColliderBase* base);
    /// <summary>
    /// スフィアコライダーを作成する
    /// </summary>
    /// <param name="base">コライダーベース</param>
    void CreateColldierSphere(MyEngine::ColliderBase* base);
    /// <summary>
    /// カプセルコライダーを作成する
    /// </summary>
    /// <param name="base">コライダーベース</param>
    void CreateColldierCapsule(MyEngine::ColliderBase* base);

protected:
    // モデルハンドル
    int m_modelHandle;  
    // 頂点シェーダハンドル
    int m_vsH;  
    // ピクセルシェーダハンドル
    int m_psH;  

    // ピボット(モデルの中心をずらす)
    Vec3 m_pivot;  
    // スケール
    Vec3 m_scale;  
    // 回転
    Quaternion m_rotation;  

    // 生存フラグ
    bool m_isExist;  

private:
    // コライダー作成関数マップ
    using CreateColliderFunc_t = void(Object3DBase::*)(MyEngine::ColliderBase*);
    std::unordered_map<MyEngine::ColKind, CreateColliderFunc_t> m_createFunc;  
    // モデルファイル
    std::shared_ptr<FileBase> m_modelFile;  
    // モデルをロードしているか
    bool m_isLoadModel;  
};
