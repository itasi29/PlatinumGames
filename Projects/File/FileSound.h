#pragma once
#include "FileBase.h"

/// <summary>
/// サウンドファイルクラス
/// </summary>
class FileSound : public FileBase
{
public:
	FileSound(FileManager& mgr);
	virtual ~FileSound();

private:
	/// <summary>
	/// ハンドルを削除
	/// </summary>
	void Delete() override;
};

