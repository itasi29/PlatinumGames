#include "FileShader.h"
#include <DxLib.h>

FileShader::FileShader(FileManager& mgr) :
	FileBase(mgr)
{
}

FileShader::~FileShader()
{
}

void FileShader::Delete()
{
	DeleteShader(m_handle);
}
