//author:autumoon
//mail:autumoon@vip.qq.com

#pragma once

#include <tchar.h>
#include "Defines.h"
#include "SimpleIni.h"
#include "StdStrFile.h"

typedef struct _config_s 
{
	bool bRemPath;
	std::vector<_tstring> vDstPaths;
	std::vector<_tstring> vItemPaths;
	std::vector<_tstring> vSuffixs;

	_config_s()
	{
		//内部初始化
		bRemPath = true;
		vSuffixs.push_back(_T("*"));
	}

}config_s;

int ReadIniFile(const _tstring& strIniPath, config_s& _cfg);
int WriteIniFile(const _tstring& strIniPath, const config_s& _cfg);

_tstring VectorToString(const std::vector<_tstring>& vStrings);

//_tstring GetIniPath()
//{
//TCHAR chpath[MAX_PATH];
//GetModuleFileName(NULL, chpath, sizeof(chpath));

//_tstring strModulePath = CMfcStrFile::CString2string(chpath);
//_tstring strIniPath = CStdStr::ReplaceSuffix(strModulePath, _T(".ini"));

//return strIniPath;
//}