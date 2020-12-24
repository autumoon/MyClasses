#include "GetDirFiles.h"

//判断是否是".."目录和"."目录
inline bool isSpecialDir(const char *path)
{
	return strcmp(path, "..") == 0 || strcmp(path, ".") == 0;
}

static inline _tstring addSlashIfNeeded(const _tstring strDir, const char& cDir = '\\')
{
	_tstring strDirBak(strDir);
	if (strDir.length() > 0 && strDir[strDir.length() - 1] != cDir)
	{
		strDirBak += cDir;
	}
	else
	{
		//可能是空字符
		return strDir;
	}

	return strDirBak;
}

std::string ws2s(const std::wstring& ws)
{
	size_t convertedChars=0;
	std::string curLocale= setlocale(LC_ALL,NULL);
	setlocale(LC_ALL,"chs");
	const wchar_t* wcs=ws.c_str();
	size_t dByteNum=sizeof(wchar_t)*ws.size()+1;

	char* dest=new char[dByteNum];
	wcstombs_s(&convertedChars,dest,dByteNum,wcs,_TRUNCATE);
	std::string result=dest;
	delete[] dest;
	setlocale(LC_ALL,curLocale.c_str());

	return result;
}

std::wstring s2ws(const std::string& s)
{
	size_t convertedChars=0;
	std::string curLocale = setlocale(LC_ALL,NULL);
	setlocale(LC_ALL,"chs");
	const char* source=s.c_str();
	size_t charNum=sizeof(char)*s.size()+1;

	wchar_t* dest=new wchar_t[charNum];
	mbstowcs_s(&convertedChars,dest,charNum,source,_TRUNCATE);
	std::wstring result=dest;
	delete[] dest;
	setlocale(LC_ALL,curLocale.c_str());

	return result;
}

//显示删除失败原因
inline void showError(const char *file_name = NULL)
{
	errno_t err;
	_get_errno(&err);
	switch(err)
	{
	case ENOTEMPTY:
		printf("Given path is not a directory, the directory is not empty, or the directory is either the current working directory or the root directory.\n");
		break;
	case ENOENT:
		printf("Path is invalid.\n");
		break;
	case EACCES:
		printf("%s had been opend by some application, can't delete.\n", file_name);
		break;
	}
}

//递归搜索目录中文件并删除
bool deleteDir(const _tstring& path, const bool& bRemoveSelf /*= true*/)
{
#ifdef _UNICODE
	std::string pathBak = ws2s(addSlashIfNeeded(path));
#else
	std::string pathBak = addSlashIfNeeded(path);
#endif // _UNICODE
	_finddata_t fileinfo;
	intptr_t hFile;
	std::string p;
	if((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		while(_findnext(hFile, &fileinfo) == 0)
		{
			if(isSpecialDir(fileinfo.name))
				continue;
			std::string sCurDir = p.assign(pathBak).append(fileinfo.name);
			if( fileinfo.attrib & _A_SUBDIR )
			{
				//如果是目录，开始递归删除目录中的内容
#ifdef _UNICODE
				deleteDir(s2ws(sCurDir));
#else
				deleteDir(sCurDir);
#endif // _UNICODE
			}
			else
			{
				if(remove(sCurDir.c_str()) == -1)
				{
					showError(fileinfo.name);

					return false;
				}

			}
		}
		_findclose(hFile);//关闭打开的文件句柄，并释放关联资源，否则无法删除空目录
	}

	//删除空目录，必须在递归返回前调用_findclose,否则无法删除目录
	if(bRemoveSelf && _rmdir(pathBak.c_str()) == -1)
	{
		showError();//目录非空则会显示出错原因

		return false;
	}

	return true;
}

size_t getDirs(const _tstring& path, std::vector<_tstring>& dirs, const bool& bIncludeSub/* = true*/)
{
#ifdef _UNICODE
	std::string pathBak = ws2s(addSlashIfNeeded(path));
#else
	std::string pathBak = addSlashIfNeeded(path);
#endif // _UNICODE
	//文件句柄
	intptr_t  hFile = 0;
	//文件信息  
	_finddata_t fileinfo;
	std::string p;

	//查找目录
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			if (fileinfo.attrib &  _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 &&
					strcmp(fileinfo.name, "..") != 0 && !(fileinfo.attrib & _A_SYSTEM))
				{
					if (bIncludeSub)
					{
#ifdef _UNICODE
						getDirs(s2ws(p.assign(pathBak).append(fileinfo.name)), dirs, bIncludeSub);
#else
						getDirs(p.assign(pathBak).append(fileinfo.name), dirs, bIncludeSub);
#endif // _UNICODE
					}
#ifdef _UNICODE
					dirs.push_back(s2ws(p.assign(pathBak).append(fileinfo.name)));
#else
					dirs.push_back(p.assign(pathBak).append(fileinfo.name));
#endif // _UNICODE
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	return dirs.size();
}

/******************************************以下为严格控制后缀的代码，速度稍慢******************************************/
//只保留后缀，可以处理*.*类似的过滤器
_tstring LeftSuffixOnly(const _tstring& stPathOrFilter, const bool& bWithDot = false, const TCHAR& cDir = '\\')
{
	//找到最后一个目录符合的位置
	int index = (int)stPathOrFilter.rfind(cDir);
	_tstring strFileName = stPathOrFilter.substr(index + 1, stPathOrFilter.length() - index - 1);

	size_t idx = strFileName.rfind(_T("."));

	if (idx != _tstring::npos)
	{
		if (bWithDot)
		{
			return strFileName.substr(idx);
		}
		else
		{
			return strFileName.substr(idx + 1);
		}
	}
	else
	{
		return _T("");
	}
}

//转换大小写
_tstring ToUpperLower(const _tstring& strOri, const bool& bToLower = true)
{
	size_t len = strOri.length() + 1;
	TCHAR* szDst = new TCHAR[len];
	memset(szDst, 0, len);

	_tcscpy_s(szDst, len, strOri.c_str());

	int i = -1;

	if (bToLower)
	{
		//全部转换为小写
		while(szDst[++i])
		{
			szDst[i] = tolower(szDst[i]);
		}
	}
	else
	{
		//全部转换为大写
		while(szDst[++i])
		{
			szDst[i] = toupper(szDst[i]);
		}
	}

	_tstring strDst(szDst);

	delete[] szDst;
	szDst = nullptr;

	return strDst;
}

bool IsStrictSuffix(const char* strFilePath, const char *filespec /*= "*.*"*/)
{
	if (strcmp(filespec, "*.*") == 0 || strcmp(filespec, "*") == 0 )
	{
		return true;
	}

	//包含指定的suffix
#ifdef _UNICODE
	_tstring strCurSuffix = ToUpperLower(LeftSuffixOnly(s2ws(strFilePath)));
	_tstring strCurSpe = ToUpperLower(LeftSuffixOnly(s2ws(filespec)));
#else
	_tstring strCurSuffix = ToUpperLower(LeftSuffixOnly(strFilePath));
	_tstring strCurSpe = ToUpperLower(LeftSuffixOnly(filespec));
#endif // _UNICODE

	return strCurSuffix == strCurSpe;
}

size_t getFiles(const _tstring& path, std::vector<_tstring>& files, const char *filespec /*= "*.*"*/, const bool& bIncludeSub /*= true*/)
{
	std::string sFileSepc = filespec;
	//对各种后缀情况进行处理
	if (sFileSepc.length() && sFileSepc[0] != '*')
	{
		if (sFileSepc[0] == '.')
		{
			sFileSepc = '*' + sFileSepc;
		}
		else
		{
			sFileSepc = "*." + sFileSepc;
		}
	}

#ifdef _UNICODE
	std::string pathBak = ws2s(addSlashIfNeeded(path));
#else
	std::string pathBak = addSlashIfNeeded(path);
#endif // _UNICODE
	//文件句柄
	intptr_t  hFile = 0;
	//文件信息  
	_finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(pathBak).append(sFileSepc).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果不是目录,加入列表  
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				//必须严格确定后缀
				if (IsStrictSuffix(fileinfo.name, filespec))
				{
#ifdef _UNICODE
					files.push_back(s2ws(p.assign(pathBak).append(fileinfo.name)));
#else
					files.push_back(p.assign(pathBak).append(fileinfo.name));
#endif // _UNICODE
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	if (!bIncludeSub)
	{
		return files.size();
	}

	//上面只是查找了文件，这里查找目录
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			if (fileinfo.attrib &  _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 &&
					strcmp(fileinfo.name, "..") != 0 && !(fileinfo.attrib & _A_SYSTEM))
#ifdef _UNICODE
					getFiles(s2ws(p.assign(pathBak).append(fileinfo.name)), files, filespec, true);
#else
					getFiles(p.assign(pathBak).append(fileinfo.name), files, filespec, true);
#endif // _UNICODE
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	return files.size();
}

size_t getFiles(const _tstring& path, std::vector<_tstring>& files, const std::vector<_tstring>& vFileSpecs, const bool& bIncludeSub /*= true*/)
{
	std::vector<_tstring> vSpecs;
	bool bGetAll = false;
	size_t nFSNum = vFileSpecs.size();

	for (size_t i = 0; i < nFSNum; ++i)
	{
		_tstring strCurSpe = vFileSpecs[i];
		if (strCurSpe == _T("*.*") || strCurSpe == _T("*"))
		{
			bGetAll = true;
			break;
		}
	}

	//读取所有文件
	if (bGetAll)
	{
		vSpecs.push_back(_T("*.*"));
	}
	else
	{
		vSpecs = vFileSpecs;
	}

	//对每一个后缀按照顺序进行读取
	size_t nSpecNum = vSpecs.size();
	for (size_t i = 0; i < nSpecNum; ++i)
	{
		_tstring strCurSpec = vSpecs[i];
#ifdef _UNICODE
		std::string sCurSpec = ws2s(strCurSpec);
		getFiles(path, files, sCurSpec.c_str(), bIncludeSub);
#else
		getFiles(path, files, strCurSpec.c_str(), bIncludeSub);
#endif // _UNICODE
	}

	return files.size();
}

