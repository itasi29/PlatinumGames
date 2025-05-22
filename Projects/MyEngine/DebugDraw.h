#pragma once
#ifdef _DEBUG
#include <list>
#include <unordered_map>
#include "Geometry/Vec3.h"
#include "Geometry/Quaternion.h"

namespace MyEngine
{
    /// <summary>
    /// デバッグ描画クラス
    /// </summary>
    class DebugDraw
    {
    public:
        struct CircleInfo
        {
            Vec3 center = Vec3();           // 中心座標
            float radius = 0.0f;            // 半径
            unsigned int color = 0xff00ff;  // 色
        };
        struct LineInfo
        {
            Vec3 start = Vec3();            // 始点
            Vec3 end = Vec3();              // 終点
            unsigned int color = 0xff00ff;  // 色
        };
        struct SphereInfo
        {
            Vec3 center = Vec3();           // 中心座標
            float radius = 0.0f;            // 半径
            unsigned int color = 0xff00ff;  // 色
            bool isFill = false;            // 塗りつぶしフラグ
        };
        struct CapsuleInfo
        {
            Vec3 center = Vec3();           // 中心座標
            Vec3 dir = Vec3();              // 方向ベクトル
            float radius = 0.0f;            // 半径
            float size = 0.0f;              // サイズ
            unsigned int color = 0xff00ff;  // 色
            bool isFill = false;            // 塗りつぶしフラグ
        };
        struct BoxInfo
        {
            Vec3 center = Vec3();           // 中心座標
            Vec3 size = Vec3();             // サイズ
            Quaternion rot;                 // 回転
            unsigned int color = 0xff00ff;  // 色
            bool isFill = false;            // 塗りつぶしフラグ
        };
        struct TerrainInfo
        {
            Vec3 center = Vec3();           // 中心座標
            Vec2 size = Vec2();             // サイズ
            Vec3 norm = Vec3();             // 法線
            Quaternion rot;                 // 回転
            unsigned int color = 0xff00ff;  // 色
            bool isFill = false;            // 塗りつぶしフラグ
        };

    private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
        DebugDraw();
		/// <summary>
		/// デストラクタ
		/// </summary>
        ~DebugDraw();

		DebugDraw(const DebugDraw&) = delete;
		DebugDraw(const DebugDraw&&) = delete;
		DebugDraw& operator= (const DebugDraw&) = delete;
		DebugDraw& operator= (const DebugDraw&&) = delete;

    public:
        /// <summary>
        /// インスタンスを取得する
        /// </summary>
        /// <returns>DebugDrawのインスタンス</returns>
        static DebugDraw& GetInstance();

        /// <summary>
        /// ガイドを描画する
        /// </summary>
        void Gulid() const;

        /// <summary>
        /// ツリーを描画する
        /// </summary>
        /// <param name="level">レベル</param>
        /// <param name="min">最小座標</param>
        /// <param name="max">最大座標</param>
        void Tree(int level, const Vec3& min, const Vec3& max) const;

        /// <summary>
        /// 描画を停止するか設定する
        /// </summary>
        /// <param name="isStop">true:停止 / false:再開</param>
        void SetStop(bool isStop) { m_isStop = isStop; }

        /// <summary>
        /// 描画情報をクリアする
        /// </summary>
        void Clear();

        /// <summary>
        /// 描画する
        /// </summary>
        void Draw() const;

        /// <summary>
        /// 円を描画する
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="radius">半径</param>
        /// <param name="color">色</param>
        void DrawCircle(const Vec3& center, float radius, unsigned int color);

        /// <summary>
        /// 線を描画する
        /// </summary>
        /// <param name="start">始点</param>
        /// <param name="end">終点</param>
        /// <param name="color">色</param>
        void DrawLine(const Vec3& start, const Vec3& end, unsigned int color);

        /// <summary>
        /// 球を描画する
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="radius">半径</param>
        /// <param name="color">色</param>
        /// <param name="isFill">塗りつぶしフラグ</param>
        void DrawSphere(const Vec3& center, float radius, unsigned int color, bool isFill);

        /// <summary>
        /// カプセルを描画する
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="dir">方向ベクトル</param>
        /// <param name="radius">半径</param>
        /// <param name="size">サイズ</param>
        /// <param name="color">色</param>
        /// <param name="isFill">塗りつぶしフラグ</param>
        void DrawCapsule(const Vec3& center, const Vec3& dir, float radius, float size, unsigned int color, bool isFill);

        /// <summary>
        /// キューブを描画する
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="size">サイズ</param>
        /// <param name="rot">回転</param>
        /// <param name="color">色</param>
        /// <param name="isFill">塗りつぶしフラグ</param>
        void DrawCube(const Vec3& center, const Vec3& size, const Quaternion& rot, unsigned int color, bool isFill);

        /// <summary>
        /// 地形を描画する
        /// </summary>
        /// <param name="center">中心座標</param>
        /// <param name="norm">法線</param>
        /// <param name="size">サイズ</param>
        /// <param name="rot">回転</param>
        /// <param name="color">色</param>
        /// <param name="isFill">塗りつぶしフラグ</param>
        void DrawTerrain(const Vec3& center, const Vec3& norm, const Vec2& size, const Quaternion& rot, unsigned int color, bool isFill);

    public:
        static constexpr unsigned int COL_BEFOR = 0x0000ff;  // 前の色
        static constexpr unsigned int COL_NEXT = 0xff00ff;   // 次の色
        static constexpr unsigned int COL_AFFTER = 0xff0000; // 後の色
        static constexpr unsigned int COL_HIT = 0x00ffff;    // ヒットの色

    private:
        // 描画を停止するかどうか
        bool m_isStop;  

		// 描画情報
        std::list<CircleInfo> m_circleInfo;   // 円の描画情報
        std::list<LineInfo> m_lineInfo;       // 線の描画情報
        std::list<SphereInfo> m_sphereInfo;   // 球の描画情報
        std::list<CapsuleInfo> m_capsuleInfo; // カプセルの描画情報
        std::list<BoxInfo> m_boxInfo;         // ボックスの描画情報
        std::list<TerrainInfo> m_terrainInfo; // 地形の描画情報
    };
}
#endif
