//author:autumoon
//mail:autumoon@vip.qq.com

#pragma once

#include "SimpleIni.h"
#include "StdStrFile.h"
#include <tchar.h>
#include "Defines.h"

typedef struct _config_s 
{
	bool bRemPath;
	std::vector<_tstring> vDirPaths;
	std::vector<_tstring> vFilePaths;

	_config_s()
	{
		//内部初始化
		bRemPath = true;
	}

}config_s;

int ReadIniFromFile(const _tstring& strIniPath, config_s& _conf);
int WriteIniToFile(const _tstring& strIniPath, const config_s& _conf);

_tstring VectorToString(const std::vector<_tstring>& vStrings);

//_tstring GetIniPath()
//{
//TCHAR chpath[MAX_PATH];
//GetModuleFileName(NULL, chpath, sizeof(chpath));

//_tstring strModulePath = CMfcStrFile::CString2string(chpath);
//_tstring strIniPath = CStdStr::ReplaceSuffix(strModulePath, _T(".ini"));

//return strIniPath;
//}
