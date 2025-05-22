#pragma once
#include <unordered_map>
#include <vector>
#include <string>

/// <summary>
/// フォント管理クラス
/// </summary>
class FontManager
{
private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    FontManager();

	FontManager(const FontManager&) = delete;
	FontManager(const FontManager&&) = delete;
	FontManager& operator= (const FontManager&) = delete;
	FontManager& operator= (const FontManager&&) = delete;

public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    ~FontManager();

    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>FontManagerのインスタンス</returns>
    static FontManager& GetInstance();

    /// <summary>
    /// フォントデータの作成
    /// </summary>
    void Init();
    /// <summary>
    /// フォントデータの削除
    /// </summary>
    void End();

    /// <summary>
    /// サイズに対応したハンドルを取得
    /// </summary>
	/// <param name="id">フォントのID</param>
    /// <param name="size">フォントのサイズ</param>
    /// <returns>ハンドル</returns>
    int GetHandle(int id, int size) const;

private:
    // フォントのハンドル
    // Key: フォントID{Key:フォントのサイズ　Value:フォントハンドル}
    std::vector<std::unordered_map<int, int>> m_handle;
	// フォントファイルのパス
	std::vector<std::wstring> m_fontPathTable;
};
