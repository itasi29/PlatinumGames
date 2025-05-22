#pragma once
#include <unordered_map>
#include <list>
#include <memory>

class Vec3;
class Quaternion;
class FileBase;

/// <summary>
/// Effekseer管理クラス
/// </summary>
class EffekseerManager final
{
public:
    using PlayInfo_t = std::list<int>;

private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    EffekseerManager();
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~EffekseerManager();

    EffekseerManager(const EffekseerManager&) = delete;
    EffekseerManager(const EffekseerManager&&) = delete;
    EffekseerManager& operator= (const EffekseerManager&) = delete;
    EffekseerManager& operator= (const EffekseerManager&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>EffekseerManagerのインスタンス</returns>
    static EffekseerManager& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="particle">画面に出すパーティクル数</param>
    void Init(int particle);
    /// <summary>
    /// 終了処理
    /// </summary>
    void End();

    /// <summary>
    /// ファイルのロード
    /// </summary>
    /// <param name="id">エフェクトID</param>
    void LoadFile(int id);
    /// <summary>
    /// ロードしたファイルの解放
    /// </summary>
    void ReleaseFile();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    /// <summary>
    /// カメラと情報を同期させる
    /// </summary>
    void SyncCamera() const;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() const;

    /// <summary>
    /// エフェクトを再生する
    /// </summary>
    /// <param name="id">再生したいエフェクトのファイルID</param>
    /// <returns>再生ハンドル</returns>
    int Play(int id);
    /// <summary>
    /// エフェクトを停止する
    /// </summary>
    /// <param name="handle">停止させるエフェクトのハンドル</param>
    void Stop(int handle);
    /// <summary>
    /// 現在再生しているエフェクトを全て停止させる
    /// </summary>
    void AllStop();

    /// <summary>
    /// エフェクトに座標・回転を適用する
    /// </summary>
    /// <param name="handle">エフェクトハンドル</param>
    /// <param name="pos">座標</param>
    /// <param name="rot">回転</param>
    void SetInfo(int handle, const Vec3& pos, const Quaternion& rot) const;
    /// <summary>
    /// エフェクトに座標・回転・スケールを適用する
    /// </summary>
    /// <param name="handle">エフェクトハンドル</param>
    /// <param name="pos">座標</param>
    /// <param name="rot">回転</param>
    /// <param name="scale">スケール</param>
    void SetInfo(int handle, const Vec3& pos, const Quaternion& rot, const Vec3& scale) const;

    /// <summary>
    /// エフェクトが再生中か調べる
    /// </summary>
    /// <param name="handle">確認したいハンドル</param>
    /// <returns>true: 再生中 /false: 再生していない</returns>
    bool IsPlay(int handle) const;

private:
    // エフェクトファイルのマップ
    std::unordered_map<int, std::shared_ptr<FileBase>> m_files;
    // 再生中のエフェクトのリスト
    PlayInfo_t m_playList;
};