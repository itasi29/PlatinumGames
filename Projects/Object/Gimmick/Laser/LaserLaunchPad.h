#pragma once
#include "Object/Object3DBase.h"

class FileBase;
class GateManager;
class LaserBullet;

/// <summary>
/// レーザー発射台クラス
/// </summary>
class LaserLaunchPad : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="gateMgr">ゲートマネージャー</param>
    /// <param name="reflectionNum">反射回数</param>
    LaserLaunchPad(const GateManager* gateMgr, const int reflectionNum);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~LaserLaunchPad();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dir">方向</param>
    void Init(const Vec3& dir);

    /// <summary>
    /// 終了処理
    /// </summary>
    void End() override;

    /// <summary>
    /// 再起動処理
    /// </summary>
    void Restart() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 弾を破壊する
    /// </summary>
    void DestoryBullet();

    /// <summary>
    /// クリア処理
    /// </summary>
    void OnClear() { m_isClear = true; }

private:
    // ゲートマネージャー
    const GateManager* m_gateMgr;  
    // 反射回数
    const int m_reflectionNum;  

    // レーザー弾
    std::shared_ptr<LaserBullet> m_bullet;
    // ショットサウンドエフェクト
    std::shared_ptr<FileBase> m_shotSe;  

    // 発射方向
    Vec3 m_firingDir;  

    // 作成カウント
    int m_createCount;  

    // クリアフラグ
    bool m_isClear;  
};
