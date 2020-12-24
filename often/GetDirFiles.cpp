#include "GetDirFiles.h"

//�ж��Ƿ���".."Ŀ¼��"."Ŀ¼
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
		//�����ǿ��ַ�
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

//��ʾɾ��ʧ��ԭ��
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

//�ݹ�����Ŀ¼���ļ���ɾ��
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
				//�����Ŀ¼����ʼ�ݹ�ɾ��Ŀ¼�е�����
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
		_findclose(hFile);//�رմ򿪵��ļ���������ͷŹ�����Դ�������޷�ɾ����Ŀ¼
	}

	//ɾ����Ŀ¼�������ڵݹ鷵��ǰ����_findclose,�����޷�ɾ��Ŀ¼
	if(bRemoveSelf && _rmdir(pathBak.c_str()) == -1)
	{
		showError();//Ŀ¼�ǿ������ʾ����ԭ��

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
	//�ļ����
	intptr_t  hFile = 0;
	//�ļ���Ϣ  
	_finddata_t fileinfo;
	std::string p;

	//����Ŀ¼
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
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

/******************************************����Ϊ�ϸ���ƺ�׺�Ĵ��룬�ٶ�����******************************************/
//ֻ������׺�����Դ���*.*���ƵĹ�����
_tstring LeftSuffixOnly(const _tstring& stPathOrFilter, const bool& bWithDot = false, const TCHAR& cDir = '\\')
{
	//�ҵ����һ��Ŀ¼���ϵ�λ��
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

//ת����Сд
_tstring ToUpperLower(const _tstring& strOri, const bool& bToLower = true)
{
	size_t len = strOri.length() + 1;
	TCHAR* szDst = new TCHAR[len];
	memset(szDst, 0, len);

	_tcscpy_s(szDst, len, strOri.c_str());

	int i = -1;

	if (bToLower)
	{
		//ȫ��ת��ΪСд
		while(szDst[++i])
		{
			szDst[i] = tolower(szDst[i]);
		}
	}
	else
	{
		//ȫ��ת��Ϊ��д
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

	//����ָ����suffix
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
	//�Ը��ֺ�׺������д���
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
	//�ļ����
	intptr_t  hFile = 0;
	//�ļ���Ϣ  
	_finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(pathBak).append(sFileSepc).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�������Ŀ¼,�����б�  
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				//�����ϸ�ȷ����׺
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

	//����ֻ�ǲ������ļ����������Ŀ¼
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
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

	//��ȡ�����ļ�
	if (bGetAll)
	{
		vSpecs.push_back(_T("*.*"));
	}
	else
	{
		vSpecs = vFileSpecs;
	}

	//��ÿһ����׺����˳����ж�ȡ
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

