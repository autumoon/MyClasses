/* ******* MfcStrFile.h **********
********* �ַ������ļ���Ŀ¼������������ ********** */

/* author: autumoon */

#pragma once

#ifdef _X86_
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <afxdlgs.h>        //���ļ�
#include <ShlObj.h>         //����ļ���
#include <string>

class CMfcStrFile
{
public:
	static std::string CString2string(const CString& strCstring);												//ʹ���������Ҫ�����ڴ��ͷŵ�����

	static CString BrowseDir(const CString& strTips = CString("��ѡ���ļ���"));                                //���һ���ļ���
	static CString BrowseDirNew(const CString& strTips = CString("��ѡ���ļ���"));                             //���һ���ļ��У����½���ť
	static CString OpenFile();
	static CString OpenSuffixFile(const std::string& strSuffix);
	static CString OpenSuffixFile(const int& nSuffix, ...);
	static CString SaveSuffixFile(const std::string& strSuffix);
	static size_t Split(const CString& str, CStringArray& Arr, const char& ch = ',');
	static size_t Split(const CString& str, CStringArray& Arr, const CString& pattern = _T(","));

private:
	static inline char* CStringToChar(const CString& strCstring);														//�ڲ��������ڴ�ռ䣬ע���ͷ�
};
