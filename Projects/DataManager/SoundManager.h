#pragma once
#include <list>
#include <memory>

namespace MyEngine
{
    class Collidable;
}

/// <summary>
/// サウンド管理システム
/// </summary>
class SoundManager
{
private:
    /// <summary>
    /// SEデータを保存する構造体
    /// </summary>
    struct SaveSeData
    {
        int handle;  // SEハンドル
        long long savePos;  // 保存位置
        std::weak_ptr<MyEngine::Collidable> master;  // SEの発信源
    };

private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SoundManager();

    /// <summary>
    /// コピーコンストラクタを削除
    /// </summary>
    SoundManager(const SoundManager& snd) = delete;
    /// <summary>
    /// 代入演算子を削除
    /// </summary>
    void operator= (const SoundManager& snd) = delete;

public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SoundManager();

    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>SoundManagerのインスタンス</returns>
    static SoundManager& GetInstance();

    /// <summary>
    /// SEの中心を設定する
    /// </summary>
    /// <param name="center">中心となるオブジェクト</param>
    void SetSeCenter(const std::weak_ptr<MyEngine::Collidable>& center) { m_seCenter = center; }
    /// <summary>
    /// SEの中心をリセットする
    /// </summary>
    void ResetSeCenter() { m_seCenter.reset(); }

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 現在流れているSEを一時的に停止する
    /// </summary>
    void StopNowPlaySe();
    /// <summary>
    /// 一時的に止めたSEを再び流す
    /// </summary>
    void RestartStopedSe();

    /// <summary>
    /// BGMを流す
    /// </summary>
    /// <param name="soundHnadle">BGMハンドル</param>
    /// <param name="isLoop">ループするか(デフォでtrue)</param>
    /// <param name="isSoundPosSave">再生場所の保存後の再生か</param>
    void PlayBgm(int soundHnadle = -1, bool isLoop = true, bool isSoundPosSave = false);

    /// <summary>
    /// Fade設定ありBGM
    /// </summary>
    /// <param name="soundHandle">BGMハンドル : -値だと現在流しているBGMをそのまま変更する</param>
    /// <param name="rate">音量パーセント</param>
    /// <param name="isLoop">ループするか</param>
    void PlayFadeBgm(int soundHandle = -1, float rate = 1.0f, bool isLoop = true);

    /// <summary>
    /// SEを流す
    /// </summary>
    /// <param name="seHandle">SEハンドル</param>
    /// <param name="isOption">オプション</param>
    /// <param name="isOnPlaying">true: 事前になっていた場合追加では鳴らさない /false: 事前になっていても追加で鳴らす</param>
    void PlaySe(int seHandle, bool isOption = false);
    /// <summary>
    /// 3D空間でSEを流す
    /// </summary>
    /// <param name="seHandle">SEハンドル</param>
    /// <param name="master">SEの発信源</param>
    /// <param name="isOption">オプションで鳴らしているものか</param>
    void PlaySe3D(int seHandle, const std::weak_ptr<MyEngine::Collidable> master, bool isOption = false);

    /// <summary>
    /// 音声の停止
    /// </summary>
    /// <param name="soundHandle">BGMハンドル</param>
    /// <param name="isSoundPosSave">再生場所の保存を行うか</param>
    void StopBGM(int soundHandle = -1, bool isSoundPosSave = false);
    /// <summary>
    /// 渡されたハンドルの音楽を停止
    /// </summary>
    /// <param name="handle">ハンドル</param>
    void Stop(int handle);

    /// <summary>
    /// 再生中のBGMの音量を変更する
    /// </summary>
    void ChangePlayBgmVolume();

    /// <summary>
    /// 指定されたハンドルの音が再生中かどうかを確認する
    /// </summary>
    /// <param name="handle">確認したいハンドル</param>
    /// <returns>true: 再生中 / false: 再生していない</returns>
    bool IsNowPlay(int handle) const;

private:
    void ChangePlayVolume(int handle, float rate, bool isBgm);

private:
    // SEの中心となるオブジェクト
    std::weak_ptr<MyEngine::Collidable> m_seCenter;  
    // 保存されたSEデータのリスト
    std::list<SaveSeData> m_saveSeList;  

    // 現在再生中のBGM
    int m_nowPlayBgm;
    // 最後に鳴らしたSE
    int m_soundHandle;

    // 再生位置の保存
    long long m_soundSavePos;

    // 停止フラグ
    bool m_isStop;  
};
