#include "StdStrFile.h"
#include <fstream>

inline bool CStdStr::IsFileContainsDir(const std::string& strFile, const std::string& strDir)
{
	std::string strFilePath = ToUpperLower(strFile);
	strFilePath = Trim(strFilePath);
	std::string strDirPath = ToUpperLower(strDir);
	strDirPath = Trim(strDirPath);

	return strFilePath.find(strDirPath) == 0;
}

inline bool CStdStr::IsFilesContainsDir(const std::vector<std::string>& vFiles, const std::string& strDir)
{
	for (int i = 0; i < vFiles.size(); ++i)
	{
		const std::string strCurFile = vFiles[i];
		if (!IsFileContainsDir(strCurFile, strDir))
		{
			return false;
		}
	}

	return true;
}

std::string CStdStr::AddSlashIfNeeded(const std::string& strDir, const char& cDir/* = '\\'*/)
{
	std::string strDirNew(strDir);
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

std::string CStdStr::GetDirOfDir(const std::string& strDir, const char& cDir/* = '\\'*/)
{
	std::string strDirPath(strDir);

	strDirPath = TrimTail(strDirPath, cDir);

	size_t index = strDirPath.rfind(cDir);

	if (index != std::string::npos)
	{
		return strDirPath.substr(0, index);
	}
	else
	{
		return strDirPath;
	}
}

std::string CStdStr::GetDirOfFile(const std::string& strFile, const char& cDir /*= '\\'*/)
{
	std::string strFilePath(strFile);

	strFilePath = TrimTail(strFilePath, cDir);

	size_t index = strFilePath.rfind(cDir);

	if (index != std::string::npos)
	{
		return strFilePath.substr(0, index);
	}
	else
	{
		return strFilePath;
	}
}

std::string CStdStr::GetFilesCommonDir(const std::vector<std::string>& vFilesFullPath)
{
	std::string strCommonDir;
	if (vFilesFullPath.size() == 0)
	{
		return strCommonDir;
	}

	strCommonDir = vFilesFullPath[0];
	do 
	{
		std::string strTmpDir = GetDirOfDir(strCommonDir);
		if (strTmpDir != strCommonDir)
		{
			strCommonDir = strTmpDir;
		}
		else
		{
			break;
			strCommonDir = "";
		}

	} while (!IsFilesContainsDir(vFilesFullPath, strCommonDir));

	return strCommonDir;
}

std::string CStdStr::GetRootDir(const std::string& strDirOrFile, const char& cDir /*= '\\'*/)
{
	std::string strPath(strDirOrFile);
	size_t nNot = strPath.find_first_not_of(cDir);
	size_t index = strPath.find(cDir, nNot);

	if (index != std::string::npos)
	{
		return strPath.substr(0, index);
	}
	else
	{
		return strPath;
	}
}

std::string CStdStr::GetSuffixOfFile(const std::string& strFile, bool bWithDot /*= true*/)
{
	std::string strFileName = GetNameOfFile(strFile);
	size_t index = strFileName.rfind(".");

	if (index != std::string::npos)
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
		return "";
	}
}

std::string CStdStr::GetNameOfDir(const std::string& strDir, const char& cDir /*= '\\'*/)
{
	std::string strDirPath(strDir);
	strDirPath = TrimTail(strDirPath, cDir);
	int index = (int)strDirPath.rfind('\\');

	return strDirPath.substr(index + 1, strDirPath.length() - index - 1);
}

std::string CStdStr::Trim(const std::string& strOri, const char& cToTrim/* = ' '*/)
{
	std::string text(strOri);
	if (!text.empty())
	{
		text.erase(0, text.find_first_not_of(" \n\r\t"));
		text.erase(text.find_last_not_of(" \n\r\t") + 1);
		text.erase(0, text.find_first_not_of(cToTrim));
		text.erase(text.find_last_not_of(cToTrim) + 1);
	}

	return text;
}

std::string CStdStr::TrimHead(const std::string& strOri, const char& cToTrim/* = ' '*/)
{
	std::string s(strOri);
	size_t i = 0;
	for (; i < s.length(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t'&&s[i] != '\r'&&s[i] != '\n' && s[i] != cToTrim)
			break;
	}
	s = s.substr(i, s.length() - i);

	return s;
}

std::string CStdStr::TrimTail(const std::string& strOri, const char& cToTrim/* = ' '*/)
{
	std::string s(strOri);
	int i = (int)s.length() - 1;
	for (; i > 0; --i)
	{
		if (s[i] != ' ' && s[i] != '\t'&&s[i] != '\r'&&s[i] != '\n' && s[i] != cToTrim)
			break;
	}

	s = s.substr(0, i + 1);

	return s;
}

std::string CStdStr::GetNameOfFile(const std::string& strFile, const bool& bWithSuffix /*= true*/, const char& cDir/* = '\\'*/)
{
	int index = (int)strFile.rfind(cDir);
	std::string strFileName = strFile.substr(index + 1, strFile.length() - index - 1);

	if (bWithSuffix)
	{
		return strFileName;
	}
	else
	{
		size_t nIndexOfDot = strFileName.rfind('.');
		if (nIndexOfDot == std::string::npos)
		{
			return strFileName;
		}
		else
		{
			return strFileName.substr(0, nIndexOfDot);
		}
	}
}

std::string CStdStr::RepalceAll(const std::string& strInput,const std::string& old_value,const std::string& new_value /*= ""*/)
{
	if (old_value == "")
	{
		return strInput;
	}

	std::string str(strInput);
	while(true)
	{   
		std::string::size_type pos(0);   
		if((pos=str.find(old_value)) != std::string::npos)   
			str.replace(pos,old_value.length(),new_value);
		else
			break;   
	}

	return str;   
}   

std::string CStdStr::ReplaceAllDistinct(const std::string& strInput,const std::string& old_value,const std::string& new_value /*= ""*/)
{
	if (old_value == "")
	{
		return strInput;
	}

	std::string str(strInput);
	for(std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{   
		if((pos=str.find(old_value,pos)) != std::string::npos)   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}

	return   str;   
}  

std::string CStdStr::ReplaceSuffix(const std::string& strFilePath, const std::string& strNewSuffix)
{
	size_t nIndex = strFilePath.rfind('.');

	if (nIndex != std::string::npos)
	{
		return strFilePath.substr(0, nIndex) + strNewSuffix;
	}
	else
	{
		return strFilePath + strNewSuffix;
	}
}


std::string CStdStr::ToUpperLower(const std::string& strOri, const bool& bToLower/* = true*/)
{
	size_t len = strOri.length() + 1;
	char* szDst = new char[len];
	memset(szDst, 0, len);
	strcpy_s(szDst, len, strOri.c_str());

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

	std::string strDst(szDst);

	delete[] szDst;
	szDst = nullptr;

	return strDst;
}

std::vector<std::string> CStdStr::Split(const std::string& str, const std::string& pattern)
{
	std::string strBak(str);
	std::string::size_type pos = std::string::npos;
	std::vector<std::string> result;
	//扩展字符串以方便操作
	strBak += pattern;
	size_t size = strBak.size();

	for (size_t i = 0; i < size; i++)
	{
		pos = strBak.find(pattern, i);
		if (pos < size && pos != std::string::npos)
		{
			std::string s = strBak.substr(i, pos - i);
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
	std::string curLocale=setlocale(LC_ALL,NULL);
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

bool CStdDir::CreateDir(const std::string& strDir, const char& cDir/* = '\\'*/)
{
	int m = 0, n;
	std::string str1, str2;

	str1 = strDir;
	str2 = str1.substr( 0, 2 );
	str1 = str1.substr( 3, str1.size() );

	while( m >= 0 )
	{
		m = int(str1.find(cDir));

		str2 += cDir + str1.substr( 0, m );    
		n = _access( str2.c_str(), 0 ); //判断该目录是否存在
		if( n == -1 )
		{
			if (_mkdir( str2.c_str()) != 0)
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

bool CStdFile::IfAccessFile(const std::string& strFilePath)
{
	std::fstream _file;
	_file.open(strFilePath, std::ios::in);
	bool bRes = (_file != 0);
	_file.close();

	return bRes;
}

bool CStdFile::CompareFileDistinct(const std::string strLeftFile, const std::string strRightFile)
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
	errno_t erl = fopen_s(&fpl, strLeftFile.c_str(),"rb");
	errno_t err = fopen_s(&fpr, strRightFile.c_str(),"rb");

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

bool CStdFile::CopyAFile(const std::string& strSrcFileName, std::string& strDstFileName, const bool& bFailIfExists)
{
	//如果目标目录不存在，则创建目录
	std::string strDir = CStdStr::GetDirOfFile(strDstFileName);
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

size_t CStdFile::ParseTXTFile(const std::string& strFilePath, std::list<std::string>& lContentInFile)
{
	lContentInFile.clear();
	std::ifstream in(strFilePath);
	std::string line;

	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
			lContentInFile.push_back(line);
		}
	}
	in.close();

	return lContentInFile.size();
}

size_t CStdFile::ParseTXTFile(const std::string& strFilePath, std::vector<std::string>& vContentInFile)
{
	vContentInFile.clear();
	std::ifstream in(strFilePath);
	std::string line;

	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
			vContentInFile.push_back(line);
		}
	}

	in.close();

	return vContentInFile.size();
}

int CStdFile::SaveTXTFile(const std::string& strTxtPath, std::list<std::string>& lContent, bool bAppend /*= false*/)
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

	for (std::list<std::string>::iterator it = lContent.begin(); it != lContent.end(); ++it)
	{
		file << *it;
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTFile(const std::string& strTxtPath, std::vector<std::string>& vContent, bool bAppend /*= false*/)
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

	for (std::vector<std::string>::iterator it = vContent.begin(); it != vContent.end(); ++it)
	{
		file << *it;
	}
	file.close();

	return 0;
}

int CStdFile::SaveTXTFile(const std::string& strTxtPath, const std::string& strLine, bool bAppend /*= false*/)
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

	file << strLine;

	file.close();

	return 0;
}

__int64 CStdFile::GetFileSize(const std::string& strFilePath)
{
	_finddatai64_t filefind;
	size_t handle;
	__int64 fs = -1;
	if((handle=_findfirsti64(strFilePath.c_str() ,&filefind)) == -1)
		return -1;

	if   (!(_A_SUBDIR== (_A_SUBDIR & filefind.attrib)))
	{                
		fs = filefind.size ;

	}
	_findclose(handle);

	return fs;
}