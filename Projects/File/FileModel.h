#pragma once
#include "FileBase.h"

/// <summary>
/// モデルファイルクラス
/// </summary>
class FileModel : public FileBase
{
public:
	FileModel(FileManager& mgr);
	virtual ~FileModel();

private:
	/// <summary>
	/// ハンドルを削除
	/// </summary>
	void Delete() override;
};

