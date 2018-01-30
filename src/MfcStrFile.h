/* ******* MfcStrFile.h **********
********* 字符串、文件、目录操作函数声明 ********** */

/* author: autumoon */

#pragma once

#ifdef _X86_
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

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
	static _tstring CString2string(const CString& strCstring);												//使用这个则不需要考虑内存释放的问题

	static CString BrowseDir(const CString& strTips = CString("请选择文件夹"));                                //浏览一个文件夹
	static CString BrowseDirNew(const CString& strTips = CString("请选择文件夹"));                             //浏览一个文件夹，带新建按钮
	static CString OpenFile();
	static CString OpenSuffixFile(const _tstring& strSuffix);
	static CString OpenSuffixFile(const int& nSuffix, ...);
	static CString SaveSuffixFile(const _tstring& strSuffix, const _tstring& strDefaultName = _T("autumoon"));
	static size_t Split(const CString& str, CStringArray& Arr, const TCHAR& ch = ',');
	static size_t Split(const CString& str, CStringArray& Arr, const CString& pattern = _T(","));

private:
	static inline TCHAR* CStringToChar(const CString& strCstring);														//内部申请了内存空间，注意释放
};
