#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class Novel;
class FileBase;

/// <summary>
/// ノベルテキスト管理クラス
/// </summary>
class NovelManager final
{
public:
    /// <summary>
    /// ノベルの状態を表す列挙型
    /// </summary>
    enum class State
    {
        DRAWING,  // 描画中
        WAIT,     // 全文字描画して、次の文字列描画まで待機中
        END,      // 全文字描画終了
    };

private:
    /// <summary>
    /// ノベル情報を格納する構造体
    /// </summary>
    struct Info
    {
        std::wstring str = L"";  // 表示する文字列
        int interval = -1;       // 表示間隔
        int nextId = -1;         // 次のID
        bool isNext = false;     // 次の文字列があるか
        int graphId = -1;        // 画像ID
        bool isGraph = false;    // 画像かどうか
    };

private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    NovelManager();
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~NovelManager();

    NovelManager(const NovelManager&) = delete;
    NovelManager(const NovelManager&&) = delete;
    NovelManager& operator= (const NovelManager&) = delete;
    NovelManager& operator= (const NovelManager&&) = delete;

public:
    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>NovelManagerのインスタンス</returns>
    static NovelManager& GetInstance();

    /// <summary>
    /// 文字列データの読み込み
    /// </summary>
    void Load();
    /// <summary>
    /// 文字列の開始
    /// </summary>
    /// <param name="id">使用文字列ID</param>
    void Start(int id);
    /// <summary>
    /// 更新
    /// </summary>
    void Update();
    /// <summary>
    /// 描画(フォントなし)
    /// </summary>
    /// <param name="spaceWidthNum">左右空き文字数</param>
    /// <param name="startY">スタートY座標</param>
    /// <param name="color">文字カラー</param>
    void Draw(int spaceWidthNum, int startY, unsigned int color) const;
    /// <summary>
    /// 描画(フォントあり)
    /// </summary>
    /// <param name="spaceWidthNum">左右空き文字数</param>
    /// <param name="startY">スタートY座標</param>
    /// <param name="color">文字カラー</param>
    /// <param name="fontSize">フォントサイズ</param>
    /// <param name="rateW">左右の詰め具合(def = 0.8f)</param>
    /// <param name="rateH">上下の詰め具合(def = 1.0f)</param>
    void Draw(int spaceWidthNum, int startY, unsigned int color, int fontSize, float rateW = BASE_RATE_W_FONT, float rateH = BASE_RATE_H_FONT) const;
    /// <summary>
    /// 描画(画像)
    /// </summary>
    /// <param name="x">X座標</param>
    /// <param name="y">Y座標</param>
    /// <param name="size">画像サイズ</param>
    void DrawGraph(int x, int y, float size) const;

    /// <summary>
    /// 停止フラグを取得する
    /// </summary>
    /// <returns>true: 停止中/ false: 動作中</returns>
    bool IsStop() const { return m_isStop; }
    /// <summary>
    /// 全データ更新(描画)フラグ
    /// ※次の文字列があるものの場合はfalseを返す
    /// </summary>
    /// <returns>true: 全文字更新(描画)済み/ false: 更新(描画)途中or全文字更新(描画)終了したが次の文字列がある</returns>
    bool IsPlay() const;
    /// <summary>
    /// 現在の状態を取得
    /// </summary>
    /// <returns>DRAWING:描画中, WAIT:全文字描画して、次の文字列描画まで待機中, END:全文字描画</returns>
    State GetState() const;

public:
    static constexpr float BASE_RATE_W_FONT = 1.0f;  // フォントの幅の基本倍率
    static constexpr float BASE_RATE_H_FONT = 1.0f;  // フォントの高さの基本倍率

private:
    // 情報リスト
    std::vector<Info> m_infoList;
    // 画像データ
    std::unordered_map<int, std::shared_ptr<FileBase>> m_files;
    // 描画データ
    std::shared_ptr<Novel> m_novel;
    // 現在のID
    int m_nowId;
    // 確認フラグ
    bool m_isNowStart;
    bool m_isStop;
};