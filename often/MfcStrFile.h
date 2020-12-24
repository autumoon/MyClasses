/* ******* MfcStrFile.h **********
********* 字符串、文件、目录操作函数声明 ********** */

/* author: autumoon */

#pragma once

#include <afxdlgs.h>        //打开文件
#include <ShlObj.h>         //浏览文件夹
#include <string>			//std::string
#include <tchar.h>			//_T

#ifdef _UNICODE

#ifndef _tstring
#define _tstring std::wstring
#endif // _tstring

#else

#ifndef _tstring
#define _tstring std::string
#endif // _tstring

#endif

class CMfcStrFile
{
public:
	static bool StartProcess(const _tstring& stCommand, bool bShow);										//启动一个进程
	static bool StartProcess(const _tstring& stProgram, const _tstring& stFileOrCmd, bool bShow, bool bWait);

	static _tstring CString2string(const CString& strCstring);												//使用这个则不需要考虑内存释放的问题

	static CString BrowseDir(bool bNewButton = false, const CString& strTips = _T("请选择文件夹"));			//浏览一个文件夹
	static CString OpenFile();
	static CString OpenSuffixFile(const _tstring& strSuffix);
	static CString OpenSuffixFile(const int nSuffix, ...);
	static CString SaveSuffixFile(const _tstring& strSuffix, const _tstring& strDefaultName = _T("autumoon"));

	////-1代表文件 0代表不存在 1代表文件夹或者驱动器
	static int IsFileDir(const CString& strFilePath);

	static size_t OpenMultiFiles(CStringArray&arrFiles, const int nSuffix, ...);							//打开多个文件，每次打开一种类型
	static size_t OpenMultiKindFiles(CStringArray&arrFiles, const int nSuffix, ...);						//同时打开多个文件
	static size_t Split(const CString& str, CStringArray& Arr, const TCHAR& ch = ',');
	static size_t Split(const CString& str, CStringArray& Arr, const CString& pattern = _T(","));

private:
    //这样命名是为了方便调用CString2string
	static inline TCHAR* CStringToChar(const CString& strCstring);											//内部申请了内存空间，注意释放
};
