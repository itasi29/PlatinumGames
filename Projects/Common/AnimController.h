#pragma once
#include <vector>
#include <string>

/// <summary>
/// アニメーションコントローラークラス
/// </summary>
class AnimController final
{
private:
    /// <summary>
    /// アニメーション情報を格納する構造体
    /// </summary>
    struct Info
    {
        std::wstring name;  // アニメーション名
        float speed = 0.0f; // アニメーション速度
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    AnimController();
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~AnimController();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="path">IDテーブルがあるファイルパス</param>
    /// <param name="modelH">モデルハンドル</param>
    /// <param name="id">使用ID</param>
    void Init(const wchar_t* const path, int modelH, int id);
    /// <summary>
    /// アニメーションの更新
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="rate">割合</param>
    /// <returns>true:アニメーションが1ループした/ false: 更新中</returns>
    void Update(float speed = 1.0f, float rate = 0.5f);
    /// <summary>
    /// アニメーションの変更
    /// </summary>
    /// <param name="id">使用ID</param>
    /// <param name="isTopStart">true: アニメーションを初めに戻す/ false: 初めに戻さない</param>
    /// <param name="isStopEnd">true: アニメーション終了時に初めに戻らなくする/ false: 通常アニメーション</param>
    /// <param name="isUnique">true: Updateで割合をアニメーション上での再生位置とする/ false: 割合をアニメーションスピード割合とする</param>
    /// <param name="isBlendChange">true: アニメーションのブレンドを行う/ false: ブレンドを行わない</param>
    /// <param name="blendSpeed">ブレンドスピード(Def = 1/16)</param>
    /// <returns>true:成功/ false: 失敗or同じAnimIndex</returns>
    bool Change(int id, bool isTopStart = true, bool isStopEnd = false, bool isUnique = false, bool isBlendChange = true, float blendSpeed = -1.0f);
    /// <summary>
    /// 現在流れているアニメーションが一度でもループしているか
    /// </summary>
    /// <returns>true:ループしている / false:していない</returns>
    bool IsLoop() const { return m_isLoop; }

private:
    /// <summary>
    /// IDテーブルを読み込む
    /// </summary>
    /// <param name="path">IDテーブルがあるファイルパス</param>
    void LoadIdTable(const wchar_t* const path);

    /// <summary>
    /// 通常アップデート
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="rate">割合</param>
    void NormalUpdate(float speed, float rate);
    /// <summary>
    /// ブレンドアップデート
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="rate">割合</param>
    void BlendUpdate(float speed, float rate);
    /// <summary>
    /// アニメーションの更新(通常)	
    /// </summary>
    /// <param name="id">使用ID</param>
    /// <param name="attachIndex">アニメーションのアタッチ番号</param>
    /// <param name="speed">速度</param>
    /// <param name="isNow">現在のアニメーションか</param>
    /// <param name="isStopEnd">アニメーションを最後で停止させるか</param>
    void AnimUpdate(int id, int attachIndex, float speed, bool isNow, bool isStopEnd);
    /// <summary>
    /// アニメーションの更新(特殊)
    /// </summary>
    /// <param name="id">使用ID</param>
    /// <param name="attachIndex">アニメーションのアタッチ番号</param>
    /// <param name="rate">割合</param>
    void AnimUniqueUpdate(int id, int attachIndex, float rate);

    /// <summary>
    /// ブレンドありでアニメーション変更
    /// </summary>
    /// <param name="animIndex">アニメ番号</param>
    /// <param name="blendSpeed">ブレンドスピード</param>
    void BlendChange(int animIndex, float blendSpeed);
    /// <summary>
    /// ブレンドなしでアニメーション変更
    /// </summary>
    /// <param name="animIndex">アニメ番号</param>
    void NoBlendChange(int animIndex);

private:
    // 更新メンバ関数
    using UpdateFunc_t = void(AnimController::*)(float, float);
    UpdateFunc_t m_updateFunc;

    // IDテーブル
    std::vector<Info> m_idTable;

    // モデルハンドル
    int m_modelH;
    // 現在のアニメ番号
    int m_nowAnimIndex;
    // ID
    int m_nowId;
    int m_preId;
    // アタッチしている番号
    int m_nowAttachIndex;
    int m_preAttachIndex;

    // ブレンド率
    float m_blendRate;
    // ブレンドスピード
    float m_blendSpeed;

    // ループフラグ
    bool m_isLoop;
    // アニメーション最後で停止するか
    bool m_isStopEnd;
    bool m_isPreStopEnd;
    // 特殊アニメーションか
    bool m_isUnique;
    bool m_isPreUnique;
};