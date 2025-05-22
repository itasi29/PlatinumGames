#pragma once
#include "FileBase.h"

/// <summary>
/// Effekseerファイルクラス
/// </summary>
class FileEffekseer : public FileBase
{
public:
	FileEffekseer(FileManager& mgr);
	virtual ~FileEffekseer();

private:
	/// <summary>
	/// ハンドルの削除
	/// </summary>
	void Delete() override;
};

