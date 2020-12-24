#pragma once

#include <direct.h>
#include <io.h>
#include <string>
#include <tchar.h>
#include <vector>

#ifdef _UNICODE

#ifndef _tstring
#define _tstring std::wstring
#endif // _tstring

#else

#ifndef _tstring
#define _tstring std::string
#endif // _tstring

#endif

bool deleteDir(const _tstring& path, const bool& bRemoveSelf = true);
size_t getDirs(const _tstring& path, std::vector<_tstring>& dirs, const bool& bIncludeSub = true);
size_t getFiles(const _tstring& path, std::vector<_tstring>& files, const char *filespec = "*.*", const bool& bIncludeSub = true);
size_t getFiles(const _tstring& path, std::vector<_tstring>& files, const std::vector<_tstring>& vFileSpecs, const bool& bIncludeSub = true);
