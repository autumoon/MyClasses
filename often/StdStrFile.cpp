#include "StdStrFile.h"
#include <fstream>

// ..\..\Common\often;

_tstring CStdStr::AddQuoteIfContains(const _tstring& strCmd, const TCHAR& cChar /*= ' '*/, bool bAdd /*= true*/)
{
	_tstring strRes(strCmd);

	TrimLeft(strRes, '\"');
	TrimRight(strRes, '\"');

	if (bAdd && strRes.find(cChar) != _tstring::npos)
	{
		strRes = _T("\"") + strRes + _T("\"");
	}

	return strRes;
}

_tstring CStdStr::AddSlashIfNeeded(const _tstring& strDir, const TCHAR& cDir/* = '\\'*/, bool bAdd /*= true*/)
{
	_tstring strRes(strDir);

	TrimRight(strRes, cDir);

	if (bAdd && !strRes.empty())
	{
		strRes += cDir;
	}

	return strRes;
}

_tstring CStdStr::GetDirOfDir(const _tstring& strDir, const TCHAR& cDir/* = '\\'*/)
{
	_tstring strDirPath(strDir);

	strDirPath = TrimRight(strDirPath, cDir);

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

	strFilePath = TrimRight(strFilePath, cDir);

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

_tstring CStdStr::GetCommonDir(const std::vector<_tstring>& vFullPaths)
{
	_tstring strCommonDir;
	if (vFullPaths.size() == 0)
	{
		return strCommonDir;
	}

	strCommonDir = vFullPaths[0];
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

	} while (!IsFilesContainsDir(vFullPaths, strCommonDir));

	return strCommonDir;
}

_tstring CStdStr::GetFileDirSavePath(const _tstring& strFilePath, const _tstring& strDirName, const _tstring& strAddSuffix /*= _T("")*/)
{
	_tstring stDstDir = CStdStr::AddSlashIfNeeded(CStdStr::GetDirOfFile(strFilePath)) + strDirName;

	//如果目录不存在，而且新建目录失败，则返回空
	if (!CStdDir::IfAccessDir(stDstDir) && !CStdDir::CreateDir(stDstDir))
	{
		return _T("");
	}

	return CStdStr::AddSlashIfNeeded(stDstDir) + CStdStr::GetNameOfFile(strFilePath, false) + strAddSuffix + CStdStr::GetSuffixOfFile(strFilePath);
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
	return LeftSuffixOnly(strFile, bWithDot);
}

_tstring CStdStr::GetNameOfDir(const _tstring& strDir, const TCHAR& cDir /*= '\\'*/)
{
	_tstring strDirPath(strDir);
	strDirPath = TrimRight(strDirPath, cDir);
	strDirPath = TrimRight(strDirPath, _T(':'));
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

_tstring CStdStr::TrimLeft(_tstring& stOri, const TCHAR& cToTrim/* = ' '*/)
{
	size_t i = 0;
	for (; i < stOri.length(); ++i)
	{
		if (stOri[i] != ' ' && stOri[i] != '\t'&&stOri[i] != '\r'&&stOri[i] != '\n' && stOri[i] != cToTrim)
			break;
	}
	stOri = stOri.substr(i, stOri.length() - i);

	return stOri;
}

_tstring CStdStr::TrimRight(_tstring& stOri, const TCHAR& cToTrim/* = ' '*/)
{
	int i = (int)stOri.length() - 1;
	for (; i > 0; --i)
	{
		if (stOri[i] != ' ' && stOri[i] != '\t'&&stOri[i] != '\r'&&stOri[i] != '\n' && stOri[i] != cToTrim)
			break;
	}

	stOri = stOri.substr(0, i + 1);

	return stOri;
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

_tstring CStdStr::LeftSuffixOnly(const _tstring& stPathOrFilter, const bool& bWithDot /*= false*/, const TCHAR& cDir /*= '\\'*/)
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

_tstring CStdStr::RemoveDrivePath(const _tstring& stDirOrFile, const TCHAR& cDir /*= '\\'*/)
{
	_tstring stPathBak(stDirOrFile);

	_tstring::size_type pos(0);
	if ((pos=stPathBak.find(':')) != _tstring::npos && stPathBak.length() >= pos + 2)
	{
		stPathBak = stPathBak.substr(pos + 2);
	}

	stPathBak = CStdStr::TrimLeft(stPathBak, cDir);

	return stPathBak;
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

inline bool CStdStr::IsDigital(const _tstring& strCstring, const bool& bRegardDotAsNum /*= true*/)
{
	size_t n = strCstring.size();
	for(size_t i = 0; i < n; i++)
	{
		if (bRegardDotAsNum)
		{
			if (strCstring[i] != '.' && (strCstring[i] < '0'|| strCstring[i] > '9'))
				return false;
		}
		else
		{
			if (strCstring[i] < '0'|| strCstring[i] > '9')
				return false;
		}
	}

	return true;
}

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
	for (size_t i = 0; i < vFiles.size(); ++i)
	{
		const _tstring strCurFile = vFiles[i];
		if (!IsFileContainsDir(strCurFile, strDir))
		{
			return false;
		}
	}

	return true;
}

int CStdStr::SetDigitalTimes(const int& nInputNum, const int& nBaseNum /*= 2*/, const bool& bUpper /*= true*/)
{
	if (nBaseNum == 0 || nInputNum % nBaseNum == 0)
	{
		return nInputNum;
	}

	int nDstNum = nInputNum;
	int nAdd = 0;
	if (bUpper)
	{
		nAdd = nBaseNum - nInputNum % nBaseNum;
	}
	else
	{
		nAdd = nInputNum % nBaseNum - nBaseNum;
	}

	return nDstNum + nAdd;
}

int CStdStr::DigitalIndex(const _tstring& str, const bool& bRegardDotAsNum /*= true*/, const int& len /*= 1*/)
{
	int stlen = str.length();
	int try_len = stlen - len;
	for (int i = 0; i <= try_len; ++i)
	{
		_tstring stSub = str.substr(i, len);
		if (IsDigital(stSub, bRegardDotAsNum))
		{
			return i;
		}
	}

	return -1;
}

std::string CStdStr::ws2s(const std::wstring& ws)
{
	size_t convertedChars = 0;
	std::string curLocale = setlocale(LC_ALL,NULL);
	setlocale(LC_ALL,"chs");
	const wchar_t* wcs = ws.c_str();
	size_t dByteNum = sizeof(wchar_t) * ws.size() + 1;

	char* dest = new char[dByteNum];
	wcstombs_s(&convertedChars, dest, dByteNum, wcs, _TRUNCATE);
	std::string result = dest;
	delete[] dest;
	setlocale(LC_ALL,curLocale.c_str());

	return result;
}

std::wstring CStdStr::s2ws(const std::string& s)
{
	size_t convertedChars = 0;
	std::string curLocale = setlocale(LC_ALL,NULL);
	setlocale(LC_ALL,"chs");
	const char* source = s.c_str();
	size_t charNum = sizeof(char) * s.size() + 1;

	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
	std::wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL,curLocale.c_str());

	return result;
}

std::vector<_tstring> CStdStr::Split(const _tstring& str, const _tstring& pattern)
{
	_tstring strBak(str);
	_tstring::size_type pos = _tstring::npos;
	std::vector<_tstring> result;

	if (str.length() == 0)
	{
		return result;
	}

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
		std::string st2 = CStdStr::ws2s(str2);
		n = _access(st2.c_str(), 0 ); //判断该目录是否存在
#else
		n = _access( str2.c_str(), 0 ); //判断该目录是否存在
#endif
		if( n == -1 )
		{
#ifdef _UNICODE
			if (_mkdir(st2.c_str()) != 0)
#else
			if (_mkdir(str2.c_str()) != 0)
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

__int64 CStdFile::GetFileSize(const _tstring& strFilePath)
{
	_finddatai64_t filefind;
	size_t handle;
	__int64 fs = -1;
#ifdef _UNICODE
	std::string sFilePath = CStdStr::ws2s(strFilePath);
	if((handle=_findfirsti64(sFilePath.c_str() ,&filefind)) == -1)
#else
	if((handle=_findfirsti64(strFilePath.c_str() ,&filefind)) == -1)
#endif // _UNICODE
		return -1;

	if   (!(_A_SUBDIR == (_A_SUBDIR & filefind.attrib)))
	{                
		fs = filefind.size ;

	}
	_findclose(handle);

	return fs;
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
	std::string sLeftFile = CStdStr::ws2s(strLeftFile);
	std::string sRightFile = CStdStr::ws2s(strRightFile);
	errno_t erl = fopen_s(&fpl, sLeftFile.c_str(),"rb");
	errno_t err = fopen_s(&fpr, sRightFile.c_str(),"rb");
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

bool CStdFile::CopyAFile(const _tstring& stSrcFileName, const _tstring& stDstFileName, const bool& bFailIfExists)
{
	//如果源文件和目标文件相同，则直接返回true
	if (stSrcFileName == stDstFileName)
	{
		return true;
	}

	//如果目标目录不存在，则创建目录
	_tstring strDir = CStdStr::GetDirOfFile(stDstFileName);
	if (!CStdDir::CreateDir(strDir))
	{
		return false;
	}

	std::ifstream in;
	std::ofstream out;
	in.open(stSrcFileName, std::ios::binary);//打开源文件
	if (in.fail())//打开源文件失败
	{
		in.close();
		out.close();
		return false;
	}
	if (bFailIfExists && IfAccessFile(stDstFileName))
	{
		out.close();
		in.close();
		return false;
	}
	out.open(stDstFileName, std::ios::binary);//创建目标文件
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

bool CStdFile::IfAccessFile(const _tstring& strFilePath)
{
	std::fstream _file;
	_file.open(strFilePath, std::ios::in);
#if _MSC_VER > 1600
	bool bRes = ((bool)_file != 0);
#else
	bool bRes = (_file != 0);
#endif // _MSC_VER > 1600
	_file.close();

	return bRes;
}

int CStdFile::IsFileDir(const _tstring& stFilePath, const TCHAR& cDir /*= '\\'*/)
{
	_finddata_t fileinfo;
	intptr_t hFile;
	_tstring stPathBak(stFilePath);
	CStdStr::TrimRight(stPathBak, cDir);
#ifdef _UNICODE
	std::string pathBak = CStdStr::ws2s(stPathBak);
#else
	std::string pathBak = stPathBak;
#endif // _UNICODE

	if((hFile = _findfirst(pathBak.c_str(), &fileinfo)) != -1)
	{
		_findclose(hFile);
		return fileinfo.attrib & _A_SUBDIR ? 1 : -1;
	}

	return 0;
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

int CStdFile::SaveTXTLine(const _tstring& strTxtPath, const _tstring& strLine, bool bAppend /*= false*/)
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

int CStdFile::SaveTXTFile(std::vector<_tstring>& vContent, const _tstring& strTxtPath, bool bAppend /*= false*/)
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
		file << CStdStr::ws2s(*it) << std::endl;
#else
		file << *it << std::endl;
#endif
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTUtf8(const _tstring& strTxtPath, std::vector<_tstring>& vContent, bool bAppend /*= false*/)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
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
		std::string narrowStr = conv.to_bytes(*it);
		file << narrowStr;
#else
		file << *it;
#endif
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTUtf8(std::vector<_tstring>& vContent, const _tstring& strTxtPath, bool bAppend /*= false*/)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
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
		std::string narrowStr = conv.to_bytes(*it);
		file << narrowStr << std::endl;
#else
		file << *it << std::endl;
#endif
	}
	file.close();

	return 0;
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
	if (GetFileEncoding(strFilePath) == _T("UTF-8"))
	{
		return ParseTXTUtf8(strFilePath, vContentInFile);
	}

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

size_t CStdFile::ParseTXTUtf8(const _tstring& strFilePath, std::vector<_tstring>& vContentInFile)
{
	vContentInFile.clear();
	std::ifstream in(strFilePath);
	std::string line;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
#ifdef _UNICODE
			vContentInFile.push_back(conv.from_bytes(line));
#else
			vContentInFile.push_back(CStdStr::ws2s(conv.from_bytes(line)));
#endif // _UNICODE
		}
	}

	in.close();

	return vContentInFile.size();
}

_tstring CStdFile::GetFileEncoding(const _tstring& strFile)
{
	std::ifstream fin(strFile, std::ios::binary);
	unsigned char  s2;
	fin.read((char*)&s2, sizeof(s2));//读取第一个字节,然后左移8位
	int p = s2<<8;
	fin.read((char*)&s2, sizeof(s2));//读取第二个字节
	p +=s2;
	_tstring code;
	switch(p)//判断文本前两个字节
	{
	case 0xfffe://65534
		code = _T("Unicode");    
		break;
	case 0xfeff://65279
		code = _T("Unicode big endian");
		break;
	case 0xefbb://61371
		code = _T("UTF-8");     
		break;
	default: 
		code = _T("ANSI"); 
	}
	fin.close(); 

	return code;
}