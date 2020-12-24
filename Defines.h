#pragma once

//author:autumoon
//2018.01.18
//xcopy /d /y .\src\*.dll $(TargetDir)

#define INI_PRESUFFIX		_T("INI_PRESUFFIX")
#define INI_REMPATH			_T("INI_REMPATH")
#define INI_PATH_DIRS		_T("INI_PATH_DIRS")
#define INI_PATH_FILES		_T("INI_PATH_FILES")
#define INI_FILE_SUFFIXS	_T("INI_FILE_SUFFIXS")

#ifdef _UNICODE

#ifndef _tstring
#define _tstring std::wstring
#endif // _tstring

#else

#ifndef _tstring
#define _tstring std::string
#endif // _tstring

#endif
