#include "stdafx.h"
#include "_ConfigFile.h"



_CConfigFile::_CConfigFile()
{
	InitConfigFile();
}


_CConfigFile::~_CConfigFile()
{
}

BOOL _CConfigFile::InitConfigFile()
{
	CHAR FileFullPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, FileFullPath, MAX_PATH);   //��õ�ǰ��ִ���ļ��ľ���·��

	CHAR* v1 = NULL;


	v1 = strstr(FileFullPath, ".");
	if (v1 != NULL)
	{
		*v1 = '\0';
		strcat(FileFullPath, ".ini");
	}

	//����һ��ini�ļ�
	HANDLE FileHandle = CreateFileA(FileFullPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE,   //��ռ
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN, NULL);  //ͬ��  �첽
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	m_FileFullPath = FileFullPath;

	ULONG HighLength = 0;
	ULONG LowLength = GetFileSize(FileHandle, &HighLength);  //����ļ���С
	if (LowLength > 0 || HighLength > 0)    //���ǿ��ļ�
	{
		CloseHandle(FileHandle);
		return FALSE;
	}
	CloseHandle(FileHandle);
	WritePrivateProfileStringA("Settings", "ListenPort", "2356", m_FileFullPath.c_str());
	WritePrivateProfileStringA("Settings", "MaxConnection", "10", m_FileFullPath.c_str());
	return TRUE;

}
int  _CConfigFile::GetInt(string MainKey, string SubKey)   //"Setting"
{
	return ::GetPrivateProfileIntA(MainKey.c_str(), SubKey.c_str(), 0, m_FileFullPath.c_str());   //ini�ļ��ľ���·��
}
BOOL _CConfigFile::SetInt(string MainKey, string SubKey, int BufferData)
{
	string v1;
	sprintf((char*)v1.c_str(), "%d", BufferData);
	return ::WritePrivateProfileStringA(MainKey.c_str(), SubKey.c_str(), v1.c_str(), m_FileFullPath.c_str());
}