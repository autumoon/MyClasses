//如果出现error C1189: #error : WINDOWS.H already included.请把头文件放到最前面

#pragma once
#include <afxdlgs.h>        //打开文件
#include <ShlObj.h>         //浏览文件夹
#include <string>			//std::string

class CLOG
{
public:
	CLOG();
	virtual ~CLOG();

public:
	//-日志文件-

	//----如果没有指定，则为exe所在路径下的以日期命名的文件----
	static CString  GetLogFile();
	static short  SetLogFile(LPCTSTR strLogFile);
	static short  ViewLogFile();

	//-前缀-
	//----如果多个进程往同一个文件输出日志，可以为每个进程设置一个前缀----
	//----前缀出现在日期时间之前----
	static short  SetPrefix(LPCTSTR strPrefix);

	//-日志信息-

	//-获取日志字符串,可以另外-
	static CString  sOutV(LPCTSTR strType, LPCTSTR strFormat = NULL, va_list valist = NULL);
	static CString  sOut0(LPCTSTR strType, LPCTSTR strFormat = NULL,...);
	static CString  sOut ( LPCTSTR strFormat = NULL,...);

	//-清理日志-
	static short  Clean();
	//-将日志信息输出到文件-
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
