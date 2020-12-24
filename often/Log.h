//�������error C1189: #error : WINDOWS.H already included.���ͷ�ļ��ŵ���ǰ��

#pragma once
#include <afxdlgs.h>        //���ļ�
#include <ShlObj.h>         //����ļ���
#include <string>			//std::string

class CLOG
{
public:
	CLOG();
	virtual ~CLOG();

public:
	//-��־�ļ�-

	//----���û��ָ������Ϊexe����·���µ��������������ļ�----
	static CString  GetLogFile();
	static short  SetLogFile(LPCTSTR strLogFile);
	static short  ViewLogFile();

	//-ǰ׺-
	//----������������ͬһ���ļ������־������Ϊÿ����������һ��ǰ׺----
	//----ǰ׺����������ʱ��֮ǰ----
	static short  SetPrefix(LPCTSTR strPrefix);

	//-��־��Ϣ-

	//-��ȡ��־�ַ���,��������-
	static CString  sOutV(LPCTSTR strType, LPCTSTR strFormat = NULL, va_list valist = NULL);
	static CString  sOut0(LPCTSTR strType, LPCTSTR strFormat = NULL,...);
	static CString  sOut ( LPCTSTR strFormat = NULL,...);

	//-������־-
	static short  Clean();
	//-����־��Ϣ������ļ�-
	static short  End(const char* pInfo = "\n");
	static short  OutV(LPCTSTR strType, LPCTSTR strFormat = NULL, va_list valist = NULL);
	static short  Out0(LPCTSTR strType, LPCTSTR strFormat = NULL,...);
#ifdef UNICODE
	static short  Out(const char* pInfo);
#endif // UNICODE
	static short  Out(LPCTSTR strFormat = NULL, ...);

protected:
	static CString  s_strLogFile;
	static CString  s_strLogPrefix;
	static HANDLE  s_hWriteEvent;
};
