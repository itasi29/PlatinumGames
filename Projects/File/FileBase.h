#pragma once
#include <string>

class FileManager;

/// <summary>
/// ファイルベースクラス
/// </summary>
class FileBase
{
    friend FileManager;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="mgr">ファイルマネージャーの参照</param>
    FileBase(FileManager& mgr);
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~FileBase();

    /// <summary>
    /// ハンドルの取得
    /// </summary>
    /// <returns>ハンドル</returns>
    int GetHandle() const { return m_handle; }
    /// <summary>
    /// ファイルIDの取得(パスでない為注意)
    /// </summary>
    /// <returns>ファイルID</returns>
    const int GetID() const { return m_id; }

private:
    /// <summary>
    /// ハンドルの削除
    /// </summary>
    virtual void Delete() = 0;
    /// <summary>
    /// 終了処理
    /// </summary>
    void End() { m_isEnd = true; }

protected:
    // ファイルマネージャーの参照
    FileManager& m_mgr;  
    // ファイルID
    int m_id;  
    // ファイルハンドル
    int m_handle;  
    // 参照カウント
    int m_refCount;  
    // 永続フラグ
    bool m_isEternal;  
    // 終了フラグ
    bool m_isEnd;  
};
