#include "AFile.h"
#include <DxLib.h>
#include <cassert>

AFile::AFile(const wchar_t* path) :
    m_cursor(0)
{
	// ファイルサイズ取得＆バッファ確保
    int size = static_cast<int>(FileRead_size(path));
    m_data.resize(size);
	// ファイルオープン
    m_handle = FileRead_open(path);
	// ファイル読み込み
    FileRead_read(m_data.data(), size, m_handle);
}

bool AFile::IsReadble() const
{
    auto result = CheckHandleASyncLoad(m_handle);
    assert(result != -1);
    return result;
}

bool AFile::Read(void* address, int size)
{
    if (IsReadble()) return false;

	// データをコピー
    std::copy_n(&m_data[m_cursor], size, static_cast<uint8_t*>(address));
	// カーソルを進める
    m_cursor += size;

    return true;
}

void AFile::Close()
{
    FileRead_close(m_handle);
    m_data.clear();
}
