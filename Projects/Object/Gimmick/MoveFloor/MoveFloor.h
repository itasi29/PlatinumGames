#pragma once
#include "Object/Object3DBase.h"

class MoveFloorManager;

/// <summary>
/// 移動床クラス
/// </summary>
class MoveFloor : public Object3DBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="mgr">移動床マネージャー</param>
    MoveFloor(MoveFloorManager& mgr);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~MoveFloor();

    /// <summary>
    /// 方向を初期化する
    /// </summary>
    /// <param name="dir">方向</param>
    void InitDir(const Vec3& dir);

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;

    /// <summary>
    /// 関数を設定する
    /// </summary>
    /// <param name="isLoop">ループするかどうか</param>
    void SetFunc(bool isLoop);

    /// <summary>
    /// 終了したかどうかを取得する
    /// </summary>
    /// <returns>true:終了 / false:終了していない</returns>
    bool IsEnd() const { return m_isEnd; }

    /// <summary>
    /// 移動を停止する
    /// </summary>
    void StopMove();

    /// <summary>
    /// 移動を再開する
    /// </summary>
    void ReStartMove();

private:
    /// <summary>
    /// ループモードの更新処理
    /// </summary>
    void LoopModeUpdate();

    /// <summary>
    /// ノーループモードの更新処理
    /// </summary>
    void NoLoopModeUpdate();

private:
    // 更新関数
    using UpdateFunc_t = void(MoveFloor::*)();
    UpdateFunc_t m_updateFunc;  

    // 移動床マネージャー
    MoveFloorManager& m_mgr;  

    // 保存された速度
    Vec3 m_saveVel;  

    // 現在のチェックポイント
    int m_nowCheckPt;  
    // 次のチェックポイント
    int m_nextCheckPt;  
    // カウント
    int m_count;  

    // 終了フラグ
    bool m_isEnd;  
    // 停止フラグ
    bool m_isStop;  
    // 到着フラグ
    bool m_isArrival;  
};