//author:autumoon
//mail:autumoon@vip.qq.com
#include "Configure.h"

inline std::wstring VectorToString(const std::vector<std::string>& vStrings)
{
	std::wstring strRes;

	const size_t num = vStrings.size();

	if (num > 0)
	{
		for (size_t i = 0; i < num - 1; ++i)
		{
			const std::string& strCurItems = vStrings[i];
			strRes += CStdStr::s2ws(strCurItems + '|');
		}

		strRes += CStdStr::s2ws(vStrings[vStrings.size() - 1]);
	}

	return strRes;
}

int ReadIniFromFile(const std::string& strIniPath, config_s& _conf)
{
	bool bRes = CStdFile::IfAccessFile(strIniPath.c_str());

	CSimpleIni Ini;
	Ini.SetUnicode();
	if (bRes)
	{
		//¶ÁÈ¡ÐÅÏ¢
		Ini.LoadFile(strIniPath.c_str());
		_conf.bRemPath = Ini.GetBoolValue(INI_PRESUFFIX, INI_REMPATH, _conf.bRemPath);

		std::wstring strRootDirs = VectorToString(_conf.vDirPaths);
		strRootDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_DIRS, strRootDirs.c_str());
		_conf.vDirPaths = CStdStr::Split(CStdStr::ws2s(strRootDirs), "|");

		std::wstring strAllDirs = VectorToString(_conf.vFilePaths);
		strAllDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_FILES, strAllDirs.c_str());
		_conf.vFilePaths = CStdStr::Split(CStdStr::ws2s(strAllDirs), "|");
	}
	else
	{
		WriteIniToFile(strIniPath, _conf);
	}
	Ini.Reset();

	return 0;
}

int WriteIniToFile(const std::string& strIniPath, const config_s& _conf)
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
