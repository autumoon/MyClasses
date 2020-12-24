#include "Log.h"

//得到可执行程序所在目录
//BOOL bIncludeSep -- 是否包含最后的分隔符"\"
CString GetExePath(BOOL bIncludeSep)
{
	// 得到当前的文件名
	TCHAR chpath[MAX_PATH];
	GetModuleFileName(NULL, chpath, sizeof(chpath));

	CString strFileName(chpath);
	// 得到当前目录
	strFileName=strFileName.Left(strFileName.ReverseFind(_T('\\'))+1);
	if(bIncludeSep)
		return strFileName;
	else
		return strFileName.Left(strFileName.GetLength()-1);
}

//-获取最后的文件名 如果给定文件不是全路径，就是相对于exe-
CString GetFileForExePath(LPCTSTR strCurFile)
{
	CString strPath = strCurFile;
	if(!strPath.IsEmpty())
	{
		//-相对路径-
		if(strPath.Find(_T(":"))<=0)     
		{    
			strPath.Format(_T("%s%s"), GetExePath(TRUE), strCurFile);
		}    
	}    
	return strPath; 
}

#define LOG_EVENT _T("ChyLogWrite") 
CString CLOG::s_strLogFile = _T(""); 
CString CLOG::s_strLogPrefix = _T(""); 
HANDLE CLOG::s_hWriteEvent = NULL; 

CLOG::CLOG() 
{
}   
CLOG::~CLOG() 
{
}   

short CLOG::SetLogFile(LPCTSTR strLogFileName)
{
	if(strLogFileName==NULL || strLogFileName[0]==0)
	{
		CString strTime, strDate; //获取系统时间 　　
		CTime tm = CTime::GetCurrentTime();
		strDate = tm.Format("%Y年%m月%d日");
		s_strLogFile = GetFileForExePath(strDate + _T(".log"));
	}
	else
		s_strLogFile = GetFileForExePath(strLogFileName);

	return 1;
}

CString CLOG::GetLogFile()
{
	return s_strLogFile;
}

short CLOG::ViewLogFile()
{
	CString strLogFile = GetLogFile();
	ShellExecute(NULL, _T("open"), strLogFile, NULL, NULL, SW_SHOW);
	return strLogFile.IsEmpty() ? 0 : 1;
}

short CLOG::SetPrefix(LPCTSTR strPrefix)
{
	if(strPrefix && strPrefix[0])
	{
		s_strLogPrefix = strPrefix;
	}
	return 1;
}

CString CLOG::sOutV(LPCTSTR strType, LPCTSTR strFormat, va_list valist)
{
	CString   strPart_Prefix;
	if(!s_strLogPrefix.IsEmpty())
	{
		strPart_Prefix.Format(_T("[%s]"), s_strLogPrefix);
	}
	CString   strPart_Time;
	{
		SYSTEMTIME sysTime  = {0};
		GetLocalTime(&sysTime);
		strPart_Time.Format(_T("[%2d-%2d %2d:%2d:%2d_%3d]"),
			sysTime.wMonth, sysTime.wDay, 
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
			sysTime.wMilliseconds); 
	}
	CString   strPart_Type;
	if(strType && strType[0])
	{
		strPart_Type.Format(_T("[%s]"), strType);
	}
	CString   strPart_Info;
	{  
		strPart_Info.FormatV(strFormat, valist);
	}
	CString str = strPart_Prefix + strPart_Time + strPart_Type+ strPart_Info;
	return str;
}

CString CLOG::sOut0(LPCTSTR strType, LPCTSTR strFormat,...)
{
	va_list  valist;
	va_start(valist, strFormat);   
	CString strInfo = sOutV(strType, strFormat, valist);
	va_end(valist);
	return strInfo;
}

CString CLOG::sOut(LPCTSTR strFormat,...)
{
	va_list  valist;
	va_start(valist, strFormat);
	CString strInfo = sOutV(NULL, strFormat, valist);
	va_end(valist);
	return strInfo;
}

short CLOG::Clean()
{
	if (s_hWriteEvent == NULL)
	{
		s_hWriteEvent = OpenEvent(0, FALSE, LOG_EVENT);
		if (s_hWriteEvent == NULL)
			s_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT);
	}
	WaitForSingleObject(s_hWriteEvent, INFINITE);
	//-打开关闭文件-
	if (s_strLogFile.IsEmpty())
		SetLogFile(NULL);
	CStdioFile file;
	if (file.Open(s_strLogFile, CFile::modeCreate | CFile::modeWrite))
	{
		setlocale(LC_CTYPE, "chs");//设定f
		file.Close();
		setlocale(LC_ALL, "C"); //还原区域设定
	}
	SetEvent(s_hWriteEvent);

	return 1;
}

short CLOG::End(const char* pInfo /*= "\n"*/)
{
	if (s_hWriteEvent == NULL)
	{
		s_hWriteEvent = OpenEvent(0, FALSE, LOG_EVENT);
		if (s_hWriteEvent == NULL)
			s_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT);
	}
	WaitForSingleObject(s_hWriteEvent, INFINITE);
	//-打开关闭文件-
	if (s_strLogFile.IsEmpty())
		SetLogFile(NULL);
	CStdioFile file;
	if (file.Open(s_strLogFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		setlocale(LC_CTYPE, "chs");//设定f
		CString str(pInfo);
		file.SeekToEnd();
		file.WriteString(str);
		file.Close();
		setlocale(LC_ALL, "C"); //还原区域设定
	}
	SetEvent(s_hWriteEvent);

	return 1;
}

short CLOG::OutV(LPCTSTR strType, LPCTSTR strFormat, va_list valist)
{
	//--
	if(s_hWriteEvent==NULL)
	{
		s_hWriteEvent = OpenEvent(0, FALSE,LOG_EVENT);
		if(s_hWriteEvent==NULL)
			s_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT);
	}
	WaitForSingleObject(s_hWriteEvent, INFINITE);
	//-打开关闭文件-
	if(s_strLogFile.IsEmpty())
		SetLogFile(NULL);
	CStdioFile file;
	if(file.Open(s_strLogFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
	{
		setlocale(LC_CTYPE, "chs");//设定f
		CString  strPart_NewLine = _T("\n");
		CString  strInfo = sOutV(strType, strFormat, valist); 
		CString  str = strInfo + strPart_NewLine;
		file.SeekToEnd();
		file.WriteString(str);
		file.Close();
		setlocale(LC_ALL, "C"); //还原区域设定
	}
	SetEvent(s_hWriteEvent);
	return 1;
}

short CLOG::Out0(LPCTSTR strType, LPCTSTR strFormat,...)
{
	va_list  valist;
	va_start(valist, strFormat);   
	short rtn = OutV(strType, strFormat, valist);
	va_end(valist);
	return rtn;
}

#ifdef UNICODE
short CLOG::Out(const char* pInfo)
{
	if (s_hWriteEvent == NULL)
	{
		s_hWriteEvent = OpenEvent(0, FALSE, LOG_EVENT);
		if (s_hWriteEvent == NULL)
			s_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT);
	}
	WaitForSingleObject(s_hWriteEvent, INFINITE);
	//-打开关闭文件-
	if (s_strLogFile.IsEmpty())
		SetLogFile(NULL);
	CStdioFile file;
	if (file.Open(s_strLogFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		setlocale(LC_CTYPE, "chs");//设定f
		CString  strPart_NewLine = _T("\n");
		CString   strPart_Prefix;
		if (!s_strLogPrefix.IsEmpty())
		{
			strPart_Prefix.Format(_T("[%s]"), s_strLogPrefix);
		}
		CString   strPart_Time;
		{
			SYSTEMTIME sysTime = { 0 };
			GetLocalTime(&sysTime);
			strPart_Time.Format(_T("[%2d-%2d %2d:%2d:%2d_%3d]"),
				sysTime.wMonth, sysTime.wDay,
				sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				sysTime.wMilliseconds);
		}

		CString str = strPart_Prefix + strPart_Time + pInfo + strPart_NewLine;
		file.SeekToEnd();
		file.WriteString(str);
		file.Close();
		setlocale(LC_ALL, "C"); //还原区域设定
	}
	SetEvent(s_hWriteEvent);
	return 1;
}
#endif // UNICODE

short CLOG::Out(LPCTSTR strFormat,...)
{
	va_list  valist;
	va_start(valist, strFormat);
	short rtn = OutV(NULL, strFormat, valist);
	va_end(valist);

	return rtn;
}

