#pragma once
#include <unordered_map>
#include <string>
#include "Geometry/Vec2.h"

/// <summary>
/// リピートの種類を表す列挙型
/// </summary>
enum class RepeatKind
{
    TRIGGER,    // 押した瞬間のtrue
    REPEAT,     // 押し続けたことによるtrue
    COOL_DOWN,  // 押し続けている間のfalse
    NONE,       // 押していない時のfalse
};

/// <summary>
/// トリガーデータを格納する構造体
/// </summary>
struct TriggerData
{
    // 左トリガー
    float LT = 0.0f;
    // 右トリガー
    float RT = 0.0f;
    // 左スティック
    Vec2 LStick = {};
    // 右スティック
    Vec2 RStick = {};
};

using InputTable_t = std::unordered_map<std::string, int>;
using InputData_t = int;
using InputTriggerData_t = TriggerData;

/// <summary>
/// 入力管理クラス
/// </summary>
class Input
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Input();

    Input(const Input&) = delete;
    Input(const Input&&) = delete;
    Input operator= (const Input&) = delete;
    Input operator= (const Input&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>Inputのインスタンス</returns>
    static Input& GetInstance();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Init();
    /// <summary>
    /// 入力情報の更新
    /// </summary>
    void Update();

    /// <summary>
    /// 押されているか
    /// </summary>
    /// <param name="command">コマンド</param>
    /// <returns>true : 押している / false : 押していない</returns>
    bool IsPress(const char* const command) const;
    /// <summary>
    /// 押した瞬間
    /// </summary>
    /// <param name="command">コマンド</param>
    /// <returns>true : 押した瞬間 / false : 押していないor押し続けている</returns>
    bool IsTriggerd(const char* const command) const;
    /// <summary>
    /// 離した瞬間
    /// </summary>
    /// <param name="command">コマンド</param>
    /// <returns>true : 離した / false : 押していないor押し続けている</returns>
    bool IsReleased(const char* const command) const;
    /// <summary>
    /// 押し続けている間一定間隔でtrueに
    /// </summary>
    /// <param name="command">コマンド</param>
    /// <param name="repeatInterval">リピート間隔(def = 5)</param>
    /// <param name="startInterval">最初のリピート間隔　-値ならリピート間隔をそのまま入れる</param>
    /// <returns>true: 押した瞬間or一定間隔経った/ false: 押していないor一定時間経っていない</returns>
    bool IsRepeat(const char* const command, int repeatInterval = 5, int startInterval = -1);
    /// <summary>
    /// 押し続けている間一定間隔でtrueに
    /// </summary>
    /// <param name="command">コマンド</param>
    /// <param name="kind">判定の種類</param>
    /// <param name="repeatInterval">リピート間隔(def = 5)</param>
    /// <param name="startInterval">最初のリピート間隔　-値ならリピート間隔をそのまま入れる</param>
    /// <returns>true: 押した瞬間or一定間隔経った/ false: 押していないor一定時間経っていない</returns>
    bool IsRepeat(const char* const command, RepeatKind& kind, int repeatInterval = 5, int startInterval = -1);

    /// <summary>
    /// トリガーデータを取得する
    /// </summary>
    /// <returns>トリガーデータ</returns>
    InputTriggerData_t GetTriggerData() const { return m_trigger; }

#ifdef _DEBUG
    /// <summary>
    /// キーが押されているか
    /// </summary>
    /// <param name="key">キーコード</param>
    /// <returns>true : 押している / false : 押していない</returns>
    bool IsPress(int key) const;
    /// <summary>
    /// キーが押された瞬間か
    /// </summary>
    /// <param name="key">キーコード</param>
    /// <returns>true : 押した瞬間 / false : 押していないor押し続けている</returns>
    bool IsTriggerd(int key) const;
#endif

private:
    /// <summary>
    /// トリガーの入力率を取得する
    /// </summary>
    /// <param name="trigger">トリガーの入力値</param>
    /// <returns>入力率</returns>
    float GetTriggerRate(unsigned char trigger);
    /// <summary>
    /// スティックの入力率を取得する
    /// </summary>
    /// <param name="stick">スティックの入力値</param>
    /// <returns>入力率</returns>
    float GetStickRate(short stick);
    /// <summary>
    /// デッドゾーンを適用する
    /// </summary>
    /// <param name="rate">入力率</param>
    /// <returns>デッドゾーン適用後の入力率</returns>
    float ApplyDeadZone(float rate);

private:
    // コマンドと入力のペア
    InputTable_t m_commandTable;

    // 入力の記憶
    InputData_t m_padstate;     // 現在の入力
    InputData_t m_prePadstate;  // 直前の入力
	InputTriggerData_t m_trigger;   // トリガーデータ
    // リピート用のフレーム
	std::unordered_map<const char*, int> m_repeatFrame; 

#ifdef _DEBUG
    char m_keystate[256];       // 現在のキー入力状態
    char m_preKeystate[256];    // 直前のキー入力状態
#endif
};