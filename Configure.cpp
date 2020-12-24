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

int ReadIniFile(const _tstring& strIniPath, config_s& _cfg)
{
	bool bRes = CStdFile::IfAccessFile(strIniPath.c_str());

	CSimpleIni Ini;
	Ini.SetUnicode();
	if (bRes)
	{
		//¶ÁÈ¡ÐÅÏ¢
		Ini.LoadFile(strIniPath.c_str());
		_cfg.bRemPath = Ini.GetBoolValue(INI_PRESUFFIX, INI_REMPATH, _cfg.bRemPath);

		_tstring strRootDirs = VectorToString(_cfg.vDstPaths);
		strRootDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_DIRS, strRootDirs.c_str());
		_cfg.vDstPaths = CStdStr::Split(strRootDirs, _T("|"));

		_tstring strAllDirs = VectorToString(_cfg.vItemPaths);
		strAllDirs = Ini.GetValue(INI_PRESUFFIX, INI_PATH_FILES, strAllDirs.c_str());
		_cfg.vItemPaths = CStdStr::Split(strAllDirs, _T("|"));

		_tstring strSuffixs = VectorToString(_cfg.vSuffixs);
		strSuffixs = Ini.GetValue(INI_PRESUFFIX, INI_FILE_SUFFIXS, strSuffixs.c_str());
		_cfg.vSuffixs = CStdStr::Split(strSuffixs, _T("|"));
	}
	else
	{
		WriteIniFile(strIniPath, _cfg);
	}
	Ini.Reset();

	return 0;
}

int WriteIniFile(const _tstring& strIniPath, const config_s& _cfg)
{
	CSimpleIni Ini;
	Ini.SetUnicode();

	Ini.SetBoolValue(INI_PRESUFFIX, INI_REMPATH, _cfg.bRemPath);
	Ini.SetValue(INI_PRESUFFIX, INI_PATH_DIRS, VectorToString(_cfg.vDstPaths).c_str());
	Ini.SetValue(INI_PRESUFFIX, INI_PATH_FILES, VectorToString(_cfg.vItemPaths).c_str());
	Ini.SetValue(INI_PRESUFFIX, INI_FILE_SUFFIXS, VectorToString(_cfg.vSuffixs).c_str());

	Ini.SaveFile(strIniPath.c_str());
	Ini.Reset();

	return 0;
}
