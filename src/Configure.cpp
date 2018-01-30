//author:autumoon
//mail:autumoon@vip.qq.com
#include "Configure.h"

inline _tstring VectorToString(const std::vector<_tstring>& vStrings)
{
	_tstring strRes;

	const size_t num = vStrings.size();

	if (num > 0)
	{
		for (size_t i = 0; i < num - 1; ++i)
		{
			const _tstring& strCurItems = vStrings[i];
			strRes += strCurItems + _T('|');
		}

		strRes += vStrings[vStrings.size() - 1];
	}

	return strRes;
}

int ReadIniFromFile(const _tstring& strIniPath, config_s& _conf)
{
	bool bRes = CStdFile::IfAccessFile(strIniPath.c_str());

	CSimpleIni Ini;
	Ini.SetUnicode();
	if (bRes)
	{
		//读取信息
		Ini.LoadFile(strIniPath.c_str());
		_conf.bRemPath = Ini.GetBoolValue(INI_PRESUFFIX, INI_REMPATH, _conf.bRemPath);

		_tstring strRootDirs = VectorToString(_conf.vDirPaths);
		strRootDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_DIRS, strRootDirs.c_str());
		_conf.vDirPaths = CStdStr::Split(strRootDirs, _T("|"));

		_tstring strAllDirs = VectorToString(_conf.vFilePaths);
		strAllDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_FILES, strAllDirs.c_str());
		_conf.vFilePaths = CStdStr::Split(strAllDirs, _T("|"));
	}
	else
	{
		WriteIniToFile(strIniPath, _conf);
	}
	Ini.Reset();

	return 0;
}

int WriteIniToFile(const _tstring& strIniPath, const config_s& _conf)
{
	CSimpleIni Ini;
	Ini.SetUnicode();

	Ini.SetBoolValue(INI_PRESUFFIX, INI_REMPATH, _conf.bRemPath);
	Ini.SetValue(INI_PRESUFFIX, INI_PATH_DIRS, VectorToString(_conf.vDirPaths).c_str());
	Ini.SetValue(INI_PRESUFFIX, INI_PATH_FILES, VectorToString(_conf.vFilePaths).c_str());

	Ini.SaveFile(strIniPath.c_str());
	Ini.Reset();

	return 0;
}
