#pragma once

#include "StdStrFile.h"
#include "MfcStrFile.h"
#include "GetDirFiles.h"


bool CloneFileTime(LPCWSTR strSrcFile, LPCWSTR strDstFile);
bool GenerateTimeCheck(const _tstring& strFilePath, SYSTEMTIME& sElapsedTime);
bool IsFileExifSuffix(const _tstring& strFilePath);
bool RemoveReadOnlyAttrib(const _tstring& strFilePath);

CString GetModifyTime(LPCWSTR strFilePath);
CString GetModifyYearMonth(LPCWSTR strFilePath);