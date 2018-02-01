#include "StdStrFile.h"
#include <fstream>

inline bool CStdStr::IsFileContainsDir(const _tstring& strFile, const _tstring& strDir)
{
	_tstring strFilePath = ToUpperLower(strFile);
	strFilePath = Trim(strFilePath);
	_tstring strDirPath = ToUpperLower(strDir);
	strDirPath = Trim(strDirPath);

	return strFilePath.find(strDirPath) == 0;
}

inline bool CStdStr::IsFilesContainsDir(const std::vector<_tstring>& vFiles, const _tstring& strDir)
{
	for (int i = 0; i < vFiles.size(); ++i)
	{
		const _tstring strCurFile = vFiles[i];
		if (!IsFileContainsDir(strCurFile, strDir))
		{
			return false;
		}
	}

	return true;
}

_tstring CStdStr::AddSlashIfNeeded(const _tstring& strDir, const TCHAR& cDir/* = '\\'*/)
{
	_tstring strDirNew(strDir);
	if (strDir.length() > 0 && strDir[strDir.length() - 1] != cDir)
	{
		strDirNew += cDir;
	}
	else
	{
		//可能是空字符
		return strDir;
	}

	return strDirNew;
}

_tstring CStdStr::GetDirOfDir(const _tstring& strDir, const TCHAR& cDir/* = '\\'*/)
{
	_tstring strDirPath(strDir);

	strDirPath = TrimTail(strDirPath, cDir);

	size_t index = strDirPath.rfind(cDir);

	if (index != _tstring::npos)
	{
		return strDirPath.substr(0, index);
	}
	else
	{
		return strDirPath;
	}
}

_tstring CStdStr::GetDirOfFile(const _tstring& strFile, const TCHAR& cDir /*= '\\'*/)
{
	_tstring strFilePath(strFile);

	strFilePath = TrimTail(strFilePath, cDir);

	size_t index = strFilePath.rfind(cDir);

	if (index != _tstring::npos)
	{
		return strFilePath.substr(0, index);
	}
	else
	{
		return strFilePath;
	}
}

_tstring CStdStr::GetFilesCommonDir(const std::vector<_tstring>& vFilesFullPath)
{
	_tstring strCommonDir;
	if (vFilesFullPath.size() == 0)
	{
		return strCommonDir;
	}

	strCommonDir = vFilesFullPath[0];
	do 
	{
		_tstring strTmpDir = GetDirOfDir(strCommonDir);
		if (strTmpDir != strCommonDir)
		{
			strCommonDir = strTmpDir;
		}
		else
		{
			break;
			strCommonDir = _T("");
		}

	} while (!IsFilesContainsDir(vFilesFullPath, strCommonDir));

	return strCommonDir;
}

_tstring CStdStr::GetRootDir(const _tstring& strDirOrFile, const TCHAR& cDir /*= '\\'*/)
{
	_tstring strPath(strDirOrFile);
	size_t nNot = strPath.find_first_not_of(cDir);
	size_t index = strPath.find(cDir, nNot);

	if (index != _tstring::npos)
	{
		return strPath.substr(0, index);
	}
	else
	{
		return strPath;
	}
}

_tstring CStdStr::GetSuffixOfFile(const _tstring& strFile, bool bWithDot /*= true*/)
{
	_tstring strFileName = GetNameOfFile(strFile);
	size_t index = strFileName.rfind(_T("."));

	if (index != _tstring::npos)
	{
		if (bWithDot)
		{
			return strFileName.substr(index);
		}
		else
		{
			return strFileName.substr(index + 1);
		}
	}
	else
	{
		return _T("");
	}
}

_tstring CStdStr::GetNameOfDir(const _tstring& strDir, const TCHAR& cDir /*= '\\'*/)
{
	_tstring strDirPath(strDir);
	strDirPath = TrimTail(strDirPath, cDir);
	int index = (int)strDirPath.rfind('\\');

	return strDirPath.substr(index + 1, strDirPath.length() - index - 1);
}

_tstring CStdStr::Trim(const _tstring& strOri, const TCHAR& cToTrim/* = ' '*/)
{
	_tstring text(strOri);
	if (!text.empty())
	{
		text.erase(0, text.find_first_not_of(_T(" \n\r\t")));
		text.erase(text.find_last_not_of(_T(" \n\r\t")) + 1);
		text.erase(0, text.find_first_not_of(cToTrim));
		text.erase(text.find_last_not_of(cToTrim) + 1);
	}

	return text;
}

_tstring CStdStr::TrimHead(const _tstring& strOri, const TCHAR& cToTrim/* = ' '*/)
{
	_tstring s(strOri);
	size_t i = 0;
	for (; i < s.length(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t'&&s[i] != '\r'&&s[i] != '\n' && s[i] != cToTrim)
			break;
	}
	s = s.substr(i, s.length() - i);

	return s;
}

_tstring CStdStr::TrimTail(const _tstring& strOri, const TCHAR& cToTrim/* = ' '*/)
{
	_tstring s(strOri);
	int i = (int)s.length() - 1;
	for (; i > 0; --i)
	{
		if (s[i] != ' ' && s[i] != '\t'&&s[i] != '\r'&&s[i] != '\n' && s[i] != cToTrim)
			break;
	}

	s = s.substr(0, i + 1);

	return s;
}

_tstring CStdStr::GetNameOfFile(const _tstring& strFile, const bool& bWithSuffix /*= true*/, const TCHAR& cDir/* = '\\'*/)
{
	int index = (int)strFile.rfind(cDir);
	_tstring strFileName = strFile.substr(index + 1, strFile.length() - index - 1);

	if (bWithSuffix)
	{
		return strFileName;
	}
	else
	{
		size_t nIndexOfDot = strFileName.rfind('.');
		if (nIndexOfDot == _tstring::npos)
		{
			return strFileName;
		}
		else
		{
			return strFileName.substr(0, nIndexOfDot);
		}
	}
}

_tstring CStdStr::RepalceAll(const _tstring& strInput,const _tstring& old_value,const _tstring& new_value /*= _T("")*/)
{
	if (old_value == _T(""))
	{
		return strInput;
	}

	_tstring str(strInput);
	while(true)
	{   
		_tstring::size_type pos(0);   
		if((pos=str.find(old_value)) != _tstring::npos)   
			str.replace(pos,old_value.length(),new_value);
		else
			break;   
	}

	return str;   
}   

_tstring CStdStr::ReplaceAllDistinct(const _tstring& strInput,const _tstring& old_value,const _tstring& new_value /*= _T("")*/)
{
	if (old_value == _T(""))
	{
		return strInput;
	}

	_tstring str(strInput);
	for(_tstring::size_type pos(0); pos != _tstring::npos; pos += new_value.length())
	{   
		if((pos=str.find(old_value,pos)) != _tstring::npos)   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}

	return   str;   
}  

_tstring CStdStr::ReplaceSuffix(const _tstring& strFilePath, const _tstring& strNewSuffix)
{
	size_t nIndex = strFilePath.rfind('.');

	if (nIndex != _tstring::npos)
	{
		return strFilePath.substr(0, nIndex) + strNewSuffix;
	}
	else
	{
		return strFilePath + strNewSuffix;
	}
}


_tstring CStdStr::ToUpperLower(const _tstring& strOri, const bool& bToLower/* = true*/)
{
	size_t len = strOri.length() + 1;
	char* szDst = new char[len];
	memset(szDst, 0, len);
	
#ifdef _UNICODE
	strcpy_s(szDst, len, ws2s(strOri).c_str());
#else
	strcpy_s(szDst, len, strOri.c_str());
#endif

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

#ifdef _UNICODE
		_tstring strDst(CStdStr::s2ws(szDst));
#else
		_tstring strDst(szDst);
#endif

	delete[] szDst;
	szDst = nullptr;

	return strDst;
}

std::vector<_tstring> CStdStr::Split(const _tstring& str, const _tstring& pattern)
{
	_tstring strBak(str);
	_tstring::size_type pos = _tstring::npos;
	std::vector<_tstring> result;
	//扩展字符串以方便操作
	strBak += pattern;
	size_t size = strBak.size();

	for (size_t i = 0; i < size; i++)
	{
		pos = strBak.find(pattern, i);
		if (pos < size && pos != _tstring::npos)
		{
			_tstring s = strBak.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
		else
		{
			break;
		}
	}

	return result;
}

std::string CStdStr::ws2s(const std::wstring& ws)
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

std::wstring CStdStr::s2ws(const std::string& s)
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

bool CStdDir::CreateDir(const _tstring& strDir, const TCHAR& cDir/* = '\\'*/)
{
	int m = 0, n;
	_tstring str1(strDir), str2;
	
	str2 = str1.substr( 0, 2 );

	if (str1.length() > 3)
	{
		str1 = str1.substr( 3, str1.size() );
	}
	else
	{
		return true;
	}

	while( m >= 0 )
	{
		m = int(str1.find(cDir));

		str2 += cDir + str1.substr( 0, m );    
#ifdef _UNICODE
		n = _access(CStdStr::ws2s(str2).c_str(), 0 ); //判断该目录是否存在
#else
		n = _access( str2.c_str(), 0 ); //判断该目录是否存在
#endif
		if( n == -1 )
		{
#ifdef _UNICODE
			if (_mkdir( CStdStr::ws2s(str2).c_str()) != 0)
#else
			if (_mkdir( str2.c_str()) != 0)
#endif
			{
				//创建失败
				return false;
			}
		}

		str1 = str1.substr( m+1, str1.size() );
	}
	//创建成功
	return true;
}

bool CStdFile::IfAccessFile(const _tstring& strFilePath)
{
	std::fstream _file;
	_file.open(strFilePath, std::ios::in);
	bool bRes = (_file != 0);
	_file.close();

	return bRes;
}

bool CStdFile::CompareFileDistinct(const _tstring strLeftFile, const _tstring strRightFile)
{
	__int64 nSizel = GetFileSize(strLeftFile);
	__int64 nSizer = GetFileSize(strRightFile);

	//文件大小不同，直接返回false
	if (nSizel != nSizer)
	{
		return false;
	}

	//文件大小相同的时候，逐个字节比较
	int chl = -1, chr = -1;
	FILE* fpl = nullptr, *fpr = nullptr;

#ifdef UNICODE
	errno_t erl = fopen_s(&fpl, CStdStr::ws2s(strLeftFile).c_str(),"rb");
	errno_t err = fopen_s(&fpr, CStdStr::ws2s(strRightFile).c_str(),"rb");
#else
	errno_t erl = fopen_s(&fpl, strLeftFile.c_str(),"rb");
	errno_t err = fopen_s(&fpr, strRightFile.c_str(),"rb");
#endif

	//任意文件无法打开则返回false
	if (erl != 0 || erl != 0)
	{
		return false;
	}

	while ((chl = fgetc(fpl)) != EOF && (chr = fgetc(fpr)) != EOF)
	{
		if (chl != chr)
		{
			return false;
		}
	}

	fclose(fpl);
	fclose(fpr);

	return true;
}

bool CStdFile::CopyAFile(const _tstring& strSrcFileName, const _tstring& strDstFileName, const bool& bFailIfExists)
{
	//如果源文件和目标文件相同，则直接返回true
	if (strSrcFileName == strDstFileName)
	{
		return true;
	}

	//如果目标目录不存在，则创建目录
	_tstring strDir = CStdStr::GetDirOfFile(strDstFileName);
	if (CStdDir::CreateDir(strDir) == false)
	{
		return false;
	}

	std::ifstream in;
	std::ofstream out;
	in.open(strSrcFileName, std::ios::binary);//打开源文件
	if (in.fail())//打开源文件失败
	{
		in.close();
		out.close();
		return false;
	}
	if (bFailIfExists && IfAccessFile(strDstFileName))
	{
		out.close();
		in.close();
		return false;
	}
	out.open(strDstFileName, std::ios::binary);//创建目标文件
	if (out.fail())//创建文件失败
	{
		out.close();
		in.close();
		return false;
	}

	//复制文件
	out << in.rdbuf();
	out.close();
	in.close();

	return true;
}

size_t CStdFile::ParseTXTFile(const _tstring& strFilePath, std::list<_tstring>& lContentInFile)
{
	lContentInFile.clear();
	std::ifstream in(strFilePath);
	std::string line;

	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
#ifdef _UNICODE
			lContentInFile.push_back(CStdStr::s2ws(line));
#else
			lContentInFile.push_back(line);
#endif
		}
	}
	in.close();

	return lContentInFile.size();
}

size_t CStdFile::ParseTXTFile(const _tstring& strFilePath, std::vector<_tstring>& vContentInFile)
{
	vContentInFile.clear();
	std::ifstream in(strFilePath);
	std::string line;

	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
#ifdef _UNICODE
			vContentInFile.push_back(CStdStr::s2ws(line));
#else
			vContentInFile.push_back(line);
#endif // _UNICODE
		}
	}

	in.close();

	return vContentInFile.size();
}

int CStdFile::SaveTXTFile(const _tstring& strTxtPath, std::list<_tstring>& lContent, bool bAppend /*= false*/)
{
	std::ofstream file;
	if (bAppend)
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::app | std::ios::ate);
	}
	else
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::trunc);
	}

	for (std::list<_tstring>::iterator it = lContent.begin(); it != lContent.end(); ++it)
	{
#ifdef _UNICODE
	file << CStdStr::ws2s(*it);
#else
	file << *it;
#endif
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTFile(const _tstring& strTxtPath, std::vector<_tstring>& vContent, bool bAppend /*= false*/)
{
	std::ofstream file;
	if (bAppend)
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::app | std::ios::ate);
	}
	else
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::trunc);
	}

	for (std::vector<_tstring>::iterator it = vContent.begin(); it != vContent.end(); ++it)
	{
#ifdef _UNICODE
	file << CStdStr::ws2s(*it);
#else
	file << *it;
#endif
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTFile(const _tstring& strTxtPath, const _tstring& strLine, bool bAppend /*= false*/)
{
	std::ofstream file;
	if (bAppend)
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::app | std::ios::ate);
	}
	else
	{
		file.open(strTxtPath, std::ios::in | std::ios::out | std::ios::trunc);
	}

#ifdef UNICODE
	file << CStdStr::ws2s(strLine);
#else
	file << strLine;
#endif

	file.close();

	return 0;
}

__int64 CStdFile::GetFileSize(const _tstring& strFilePath)
{
	_finddatai64_t filefind;
	size_t handle;
	__int64 fs = -1;
#ifdef _UNICODE
	if((handle=_findfirsti64(CStdStr::ws2s(strFilePath).c_str() ,&filefind)) == -1)
#else
	if((handle=_findfirsti64(strFilePath.c_str() ,&filefind)) == -1)
#endif // _UNICODE
		return -1;

	if   (!(_A_SUBDIR== (_A_SUBDIR & filefind.attrib)))
	{                
		fs = filefind.size ;

	}
	_findclose(handle);

	return fs;
}
