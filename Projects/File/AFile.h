#pragma once
#include <vector>

/// <summary>
/// 非同期ファイル(外部バイナリデータ読み込み)管理クラス
/// </summary>
class AFile
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="path">ファイルのパス</param>
    AFile(const wchar_t* path);

    /// <summary>
    /// ファイルが読み取り可能かどうかを確認する
    /// </summary>
    /// <returns>true: 読み取り可能 / false: 読み取り不可</returns>
    bool IsReadble() const;
    /// <summary>
    /// ファイルからデータを読み込む
    /// </summary>
    /// <param name="address">読み込み先のアドレス</param>
    /// <param name="size">読み込むサイズ</param>
    /// <returns>true: 読み込み成功 / false: 読み込み失敗</returns>
    bool Read(void* address, int size);
    /// <summary>
    /// ファイルを閉じる
    /// </summary>
    void Close();

private:
    // ファイルハンドル
    int m_handle;  
    // 現在のカーソル位置
    int m_cursor;  
    // ファイルデータ
    std::vector<uint8_t> m_data;  
};