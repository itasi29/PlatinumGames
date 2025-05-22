#pragma once
#include <string>

/// <summary>
/// ノベルテキストクラス
/// </summary>
class Novel
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Novel();
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Novel();

    /// <summary>
    /// ノベルのテキスト表示を開始する
    /// </summary>
    /// <param name="str">表示するテキスト</param>
    void Start(const std::wstring& str);
    /// <summary>
    /// テキスト表示をスキップする
    /// </summary>
    void Skip();

    /// <summary>
    /// ノベルの更新処理
    /// </summary>
    /// <param name="interval">テキスト表示の間隔</param>
    void Update(const int interval);
    /// <summary>
    /// テキストを描画する
    /// </summary>
    /// <param name="spaceWidthNum">スペースの幅</param>
    /// <param name="startY">描画開始位置のY座標</param>
    /// <param name="color">テキストの色</param>
    void Draw(int spaceWidthNum, int startY, unsigned int color) const;
    /// <summary>
    /// テキストを描画する（フォントサイズと倍率指定）
    /// </summary>
    /// <param name="spaceWidthNum">スペースの幅</param>
    /// <param name="startY">描画開始位置のY座標</param>
    /// <param name="color">テキストの色</param>
    /// <param name="fontSize">フォントサイズ</param>
    /// <param name="rateW">幅の倍率</param>
    /// <param name="rateH">高さの倍率</param>
    void Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW, float rateH) const;
    /// <summary>
    /// 画像を描画する
    /// </summary>
    /// <param name="x">描画位置のX座標</param>
    /// <param name="y">描画位置のY座標</param>
    /// <param name="size">画像のサイズ</param>
    /// <param name="handle">画像のハンドル</param>
    void DrawGraph(int x, int y, float size, int handle);

    /// <summary>
    /// 終了フラグを取得する
    /// </summary>
    /// <returns>true: 終了/ false: 描画中</returns>
    bool IsEnd() const { return m_isEnd; }

private:
    // 表示するテキスト
    std::wstring m_str;  

    // フレーム数
    int m_frame;         
    // 現在描画する位置
    int m_current;       

    // 終了フラグ
    bool m_isEnd;        
};