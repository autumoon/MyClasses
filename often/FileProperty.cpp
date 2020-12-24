#include "FileProperty.h"

//----------- Error Handling Function -------------------
void error(LPSTR lpszFunction)
{
	CHAR szBuf[80];
	DWORD dw = GetLastError();

	sprintf(szBuf, "%s failed: GetLastError returned %u\n",
		lpszFunction, dw);

	MessageBox(NULL, CString(szBuf), _T("Error"), MB_OK);
	ExitProcess(dw);
}
//--------------------------------------------------------

bool CloneFileTime(LPCWSTR strSrcFile, LPCWSTR strDstFile)
{
	if (!CStdFile::IfAccessFile(strSrcFile) || !CStdFile::IfAccessFile(strDstFile))
	{
		return FALSE;
	}

	FILETIME ftCreate, ftAccess, ftWrite;

	// -------->获取 FileTime
	HANDLE hSrcFile = CreateFile(strSrcFile, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE || !GetFileTime(hSrcFile, &ftCreate, &ftAccess, &ftWrite))
	{
		//error("GetFileTime()");
		return FALSE;
	}

	HANDLE hDstFile = CreateFile(strDstFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDstFile == INVALID_HANDLE_VALUE || !SetFileTime(hDstFile, &ftCreate, &ftAccess, &ftWrite))
	{
		//error("GetFileTime()");
		return FALSE;
	}

	CloseHandle(hSrcFile);
	CloseHandle(hDstFile);

	return TRUE;
}

bool GetFileTime(HANDLE hFile, LPWSTR lpszCreationTime, LPWSTR lpszLastAccessTime, LPWSTR lpszLastWriteTime, LPCWSTR lpFormat = _T("%d%02d%02d-%02d%02d"))
{
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC1, stLocal1, stUTC2, stLocal2, stUTC3, stLocal3;

	// -------->获取 FileTime
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
	{
		//error("GetFileTime()");
		return false;
	}
	//---------> 转换: FileTime --> LocalTime
	FileTimeToSystemTime(&ftCreate, &stUTC1);
	FileTimeToSystemTime(&ftAccess, &stUTC2);
	FileTimeToSystemTime(&ftWrite, &stUTC3);

	SystemTimeToTzSpecificLocalTime(NULL, &stUTC1, &stLocal1);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC2, &stLocal2);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC3, &stLocal3);

	// ---------> Show the  date and time.
	wsprintf(lpszCreationTime, lpFormat,
		stLocal1.wYear, stLocal1.wMonth, stLocal1.wDay,
		stLocal1.wHour, stLocal1.wMinute);
	wsprintf(lpszLastAccessTime, lpFormat,
		stLocal2.wYear, stLocal2.wMonth, stLocal2.wDay,
		stLocal2.wHour, stLocal2.wMinute);
	wsprintf(lpszLastWriteTime, lpFormat,
		stLocal3.wYear, stLocal3.wMonth, stLocal3.wDay,
		stLocal3.wHour, stLocal3.wMinute);

	return false;
}

bool GenerateTimeCheck(const _tstring& strFilePath, SYSTEMTIME& sElapsedTime)
{
	if (!CStdFile::IfAccessFile(strFilePath))
	{
		return false;
	}

	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC1, stLocal1, /*stUTC2,*/ stLocal2, stUTC3, stLocal3;

	// -------->获取 FileTime
	HANDLE hSrcFile = CreateFile(strFilePath.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE || !GetFileTime(hSrcFile, &ftCreate, &ftAccess, &ftWrite))
	{
		//error("GetFileTime()");
		return false;
	}

	//---------> 转换: FileTime --> LocalTime
	FileTimeToSystemTime(&ftCreate, &stUTC1);
	//FileTimeToSystemTime(&ftAccess, &stUTC2);
	FileTimeToSystemTime(&ftWrite, &stUTC3);

	SystemTimeToTzSpecificLocalTime(NULL, &stUTC1, &stLocal1);
	//SystemTimeToTzSpecificLocalTime(NULL, &stUTC2, &stLocal2);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC3, &stLocal3);

	//sElapsedTime = stLocal2 - stLocal1;

	if (stLocal3.wYear != stLocal1.wYear || stLocal3.wMonth != stLocal1.wMonth)
	{
		//一般月和年应当相同，不同则认为有问题（比如被修改过）
		return false;
	}

	//stLocal2临时变量
	stLocal2 = stLocal3;
	//手动处理，先全部借位
	stLocal2.wDay -= 1;
	stLocal2.wHour += 24 - 1;
	stLocal2.wMinute += 60 -1;
	stLocal2.wSecond += 60 -1;
	stLocal2.wMilliseconds += 1000;

	int nDay = (int)stLocal2.wDay - stLocal1.wDay;
	int nHour = (int)stLocal2.wHour - stLocal1.wHour;
	int nMin = (int)stLocal2.wMinute - stLocal1.wMinute;
	int nSecond = (int)stLocal2.wSecond - stLocal1.wSecond;
	int nMiliseconds = (int)stLocal2.wMilliseconds - stLocal1.wMilliseconds;

	//还原
	if (nMiliseconds >= 1000)
	{
		nMiliseconds -= 1000;
		nSecond += 1;
	}

	if (nSecond >= 60)
	{
		nSecond -= 60;
		nMin += 1;
	}

	if (nMin >= 60)
	{
		nMin -= 60;
		nHour += 1;
	}

	if (nHour >= 24)
	{
		nHour-= 24;
		nDay += 1;
	}

	sElapsedTime.wYear = 0;
	sElapsedTime.wMonth = 0;

	if (nDay < 0)
	{
		return false;
	}

	sElapsedTime.wDay = nDay;
	sElapsedTime.wHour = nHour;
	sElapsedTime.wMinute = nMin;
	sElapsedTime.wSecond = nSecond;
	sElapsedTime.wMilliseconds = nMiliseconds;

	return true;
}

bool IsFileExifSuffix(const _tstring& strFilePath)
{
	std::string dstExtension = CStdStr::ws2s(CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(strFilePath)));

	return 0 == strcmp(dstExtension.c_str(), ".jpg") || 0 == strcmp(dstExtension.c_str(), ".jpeg")/* || 0 == strcmp(dstExtension.c_str(), ".tif")*/;
}

bool RemoveReadOnlyAttrib(const _tstring& strFilePath)
{
	//-------------------将只读文件的只读属性去掉-----------------------
	CFileStatus rStatus;
	//获得文件的属性设置
	if (CFile::GetStatus(strFilePath.c_str(),rStatus) == FALSE)
	{
		return false;
	}
	rStatus.m_attribute=rStatus.m_attribute & 0x3E ;
	CFile::SetStatus(strFilePath.c_str(), rStatus );//更改文件的属性设置

	return true;
}

CString GetModifyTime(LPCWSTR strFilePath)
{
	HANDLE hFile;
	TCHAR szCreationTime[30], szLastAccessTime[30], szLastWriteTime[30];
	hFile = CreateFile(strFilePath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(hFile, szCreationTime, szLastAccessTime, szLastWriteTime, _T("%d年%02d月%02d日-%02d时%02d分"));
	CString strWriteTime(szLastWriteTime);
	int nLength = strWriteTime.GetLength();
	if (hFile == INVALID_HANDLE_VALUE || nLength == 0)
	{
		//error("GetLastWriteTime()");
		return CString();
	}
	CloseHandle(hFile);

	return strWriteTime;
}

CString GetModifyYearMonth(LPCWSTR strFilePath)
{
	HANDLE hFile;
	TCHAR szCreationTime[30], szLastAccessTime[30], szLastWriteTime[30];
	hFile = CreateFile(strFilePath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(hFile, szCreationTime, szLastAccessTime, szLastWriteTime);
	CString strWriteTime(szLastWriteTime);
	int nLength = strWriteTime.GetLength();
	if (hFile == INVALID_HANDLE_VALUE || nLength == 0)
	{
		//error("GetLastWriteTime()");
		return CString();
	}
	CloseHandle(hFile);

	return nLength >= 6 ? strWriteTime.Mid(0, 6) : strWriteTime;
}

