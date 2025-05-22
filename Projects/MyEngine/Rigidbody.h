#pragma once
#include "Vec3.h"

namespace MyEngine
{
    /// <summary>
    /// 物理挙動クラス
    /// </summary>
    class Rigidbody final
    {
    public:
        Rigidbody();
        ~Rigidbody();

        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="pos">初期位置</param>
        void Init(const Vec3& pos = Vec3());

        /// <summary>
        /// 座標取得
        /// </summary>
        /// <returns>座標</returns>
        const Vec3& GetPos() const { return m_pos; }

        /// <summary>
        /// 次の座標を取得
        /// </summary>
        /// <returns>次の座標</returns>
        const Vec3& GetNextPos() const { return m_nextPos; }

        /// <summary>
        /// 移動ベクトルを取得
        /// </summary>
        /// <returns>移動ベクトル</returns>
        const Vec3& GetVelocity() const { return m_velocity; }

        /// <summary>
        /// 移動方向を取得
        /// </summary>
        /// <returns>移動方向</returns>
        const Vec3& GetDir() const { return m_dir; }

        /// <summary>
        /// 重量停止フレームを取得
        /// </summary>
        /// <returns>重力停止フレーム</returns>
        int GetStayGravityFrame() const { return m_stayGravityFrame; }

        /// <summary>
        /// 重力を使用するか
        /// </summary>
        /// <returns>true:使用 /false: 不使用</returns>
        bool IsGravity() const { return m_isGravity; }

        /// <summary>
        /// 場所の設定
        /// </summary>
        /// <param name="pos">座標</param>
        void SetPos(const Vec3& pos) { m_pos = pos; }

        /// <summary>
        /// 次の座標の設定
        /// </summary>
        /// <param name="nextPos">次の座標</param>
        void SetNextPos(const Vec3& nextPos) { m_nextPos = nextPos; }

        /// <summary>
        /// 移動ベクトルを設定
        /// </summary>
        /// <param name="velocity">移動ベクトル</param>
        void SetVelocity(const Vec3& velocity);

        /// <summary>
        /// 重量停止フレームの設定
        /// </summary>
        /// <param name="frame">停止フレーム</param>
        void SetStayGravityFrame(int frame) { m_stayGravityFrame = frame; }

        /// <summary>
        /// 重量の設定
        /// </summary>
        /// <param name="isGravity">true:使用 /false: 不使用</param>
        void SetGravity(bool isGravity) { m_isGravity = isGravity; }

        /// <summary>
        /// ある方向に力を加える
        /// </summary>
        /// <param name="dir">力の方向</param>
        /// <param name="power">力の強さ</param>
        /// <param name="isLimit">true:速度に限界値を設定する /false:限界値を設定しない</param>
        /// <param name="limit">限界値</param>
        /// <param name="isMaxUp">true:速度を限界値に修正する /false:修正しない</param>
        void AddForce(const Vec3& dir, float power, bool isLimit = false, float limit = 0.0f, bool isMaxUp = false);

    private:
        // 現在の座標
        Vec3 m_pos;             
        // 次の座標
        Vec3 m_nextPos;         
        // 移動ベクトル
        Vec3 m_velocity;        
        // 移動方向
        Vec3 m_dir;             

        // 重力停止フレーム
        int m_stayGravityFrame; 

        // 重力を使用するかどうか
        bool m_isGravity;       
    };
}
