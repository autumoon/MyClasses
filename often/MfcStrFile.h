/* ******* MfcStrFile.h **********
********* �ַ������ļ���Ŀ¼������������ ********** */

/* author: autumoon */

#pragma once

#include <afxdlgs.h>        //���ļ�
#include <ShlObj.h>         //����ļ���
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
	static bool StartProcess(const _tstring& stCommand, bool bShow);										//����һ������
	static bool StartProcess(const _tstring& stProgram, const _tstring& stFileOrCmd, bool bShow, bool bWait);

	static _tstring CString2string(const CString& strCstring);												//ʹ���������Ҫ�����ڴ��ͷŵ�����

	static CString BrowseDir(bool bNewButton = false, const CString& strTips = _T("��ѡ���ļ���"));			//���һ���ļ���
	static CString OpenFile();
	static CString OpenSuffixFile(const _tstring& strSuffix);
	static CString OpenSuffixFile(const int nSuffix, ...);
	static CString SaveSuffixFile(const _tstring& strSuffix, const _tstring& strDefaultName = _T("autumoon"));

	////-1�����ļ� 0�������� 1�����ļ��л���������
	static int IsFileDir(const CString& strFilePath);

	static size_t OpenMultiFiles(CStringArray&arrFiles, const int nSuffix, ...);							//�򿪶���ļ���ÿ�δ�һ������
	static size_t OpenMultiKindFiles(CStringArray&arrFiles, const int nSuffix, ...);						//ͬʱ�򿪶���ļ�
	static size_t Split(const CString& str, CStringArray& Arr, const TCHAR& ch = ',');
	static size_t Split(const CString& str, CStringArray& Arr, const CString& pattern = _T(","));

private:
    //����������Ϊ�˷������CString2string
	static inline TCHAR* CStringToChar(const CString& strCstring);											//�ڲ��������ڴ�ռ䣬ע���ͷ�
};
