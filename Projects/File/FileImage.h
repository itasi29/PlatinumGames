#pragma once
#include "FileBase.h"

/// <summary>
/// 画像ファイルクラス
/// </summary>
class FileImage :  public FileBase
{
public:
	FileImage(FileManager& mgr);
	virtual ~FileImage();

private:
	/// <summary>
	/// ハンドルの削除
	/// </summray>
	void Delete() override;
};

