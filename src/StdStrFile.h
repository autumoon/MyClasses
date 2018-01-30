#pragma once
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <direct.h>		//_mkdir函数的头文件
#include <io.h>
#include <tchar.h>

#ifdef _UNICODE

#ifndef _tstring
#define _tstring std::wstring
#endif // _tstring

#else

#ifndef _tstring
#define _tstring std::string
#endif // _tstring

#endif

class CStdStr
{
public:
	//字符操作
	static bool IsFileContainsDir(const _tstring& strFile, const _tstring& strDir);
	static bool IsFilesContainsDir(const std::vector<_tstring>& vFiles, const _tstring& strDir);
	static _tstring AddSlashIfNeeded(const _tstring& strDir, const TCHAR& cDir = '\\');
	static _tstring GetDirOfDir(const _tstring& strDir, const TCHAR& cDir = '\\');
	static _tstring GetDirOfFile(const _tstring& strFile, const TCHAR& cDir = '\\');
	static _tstring GetFilesCommonDir(const std::vector<_tstring>& vFilesFullPath);
	static _tstring GetRootDir(const _tstring& strDirOrFile, const TCHAR& cDir = '\\');
	static _tstring GetSuffixOfFile(const _tstring& strFile, bool bWithDot = true);
	static _tstring GetNameOfDir(const _tstring& strDir, const TCHAR& cDir = '\\');
	static _tstring GetNameOfFile(const _tstring& strFile, const bool& bWithSuffix = true, const TCHAR& cDir = '\\');
	static _tstring RepalceAll(const _tstring& str,const _tstring& old_value,const _tstring& new_value = _T(""));
	static _tstring ReplaceAllDistinct(const _tstring& str,const _tstring& old_value,const _tstring& new_value = _T(""));
	static _tstring ReplaceSuffix(const _tstring& strFilePath, const _tstring& strNewSuffix);
	static _tstring ToUpperLower(const _tstring& strOri, const bool& bToLower = true);
	static _tstring Trim(const _tstring& strOri, const TCHAR& cToTrim = ' ');
	static _tstring TrimHead(const _tstring& strOri, const TCHAR& cToTrim = ' ');
	static _tstring TrimTail(const _tstring& strOri, const TCHAR& cToTrim = ' ');
	static std::string ws2s(const std::wstring& ws);
	static std::wstring s2ws(const std::string& s);

	static std::vector<_tstring> Split(const _tstring& str, const _tstring& pattern);
};

class CStdDir
{
public:
	static bool IfAccessDir(const _tstring& strDirPath);
	static bool CreateDir(const _tstring& strDir, const TCHAR& cDir = '\\');
};

class CStdFile
{
public:
	//文件操作
	static bool CompareFileDistinct(const _tstring strLeftFile, const _tstring strRightFile);
	static bool CopyAFile(const _tstring& strSrcFileName, const _tstring& strDstFileName, const bool& bFailIfExists);
	static bool IfAccessFile(const _tstring& strFilePath);
	
	static size_t ParseTXTFile(const _tstring& strFilePath, std::list<_tstring>& lContentInFile);
	static size_t ParseTXTFile(const _tstring& strFilePath, std::vector<_tstring>& vContentInFile);
	static int SaveTXTFile(const _tstring& strTxtPath, std::list<_tstring>& lContent, bool bAppend = false);
	static int SaveTXTFile(const _tstring& strTxtPath, std::vector<_tstring>& vContent, bool bAppend = false);
	static int SaveTXTFile(const _tstring& strTxtPath, const _tstring& strLine, bool bAppend = false);
	static __int64 GetFileSize(const _tstring& strFilePath);
};

class CStdTpl
{
public:
	typedef enum _VectorContainFlag
	{
		eCircleReverseContain = -2,
		eReverseContain = -1,
		eNoSeq = 0,
		eSequenceContain = 1,
		eCircleSequenceContain = 2
	}VectorContainFlag;


public:
	template <class T>
	static bool DelPointerSafely(T*& pPointer, const bool& bArray = false);
	//如果是结构体或者类，自带构造函数，则bIniZero必须为false，否则构造函数失效
	template <class T>
	static bool NewSafely(T*& pPointer, const size_t& nSize = 1, const bool& bIniZero = false);
	template <class T>
	static bool VectorContains(const std::vector<T>& vTs, const T& value);
	//nStrictSeq = -2环形逆向严格包含,-1逆向严格包含，0或者其他值不讲究顺序，1正向严格包含，2环形正向严格包含
	template <class T>
	static bool VectorContains(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, const VectorContainFlag& nStrictSeq = eNoSeq);
	template <class T>
	static int VectorFind(const std::vector<T>& vTs, const T& value, bool bPositiveGoing = true);
	template <class T>
	static void* ConvertFromString(T &value, const _tstring &s);
	template <class T>
	static _tstring ConvertToString(T value);

private:
	template <class T>
	static bool VectorContainsStrict(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing = true);
	template <class T>
	static bool VectorContainsStrictCircle(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing = true);
};

/***********************内联或者模板实现***********************/
inline bool CStdDir::IfAccessDir(const _tstring& strDirPath)
{
#ifdef _UNICODE
	return _access(CStdStr::ws2s(strDirPath).c_str(), 0) == 0 && !CStdFile::IfAccessFile(strDirPath);
#else
	return _access(strDirPath.c_str(), 0) == 0 && !CStdFile::IfAccessFile(strDirPath);
#endif // _UNICODE
}

template <class T>
inline bool CStdTpl::DelPointerSafely(T*& pPointer, const bool& bArray /*= false*/)
{
	if (pPointer != nullptr)
	{
		if (bArray)
		{
			delete[] pPointer;
		}
		else
		{
			delete pPointer;
		}
		pPointer = nullptr;

		return true;
	}

	return false;
}

template <class T>
bool CStdTpl::NewSafely(T*& pPointer, const size_t& nSize /*= 1*/, const bool& bIniZero /*= false*/)
{
	try
	{
		pPointer = new T[nSize];

		if (bIniZero)
		{
			memset(pPointer, 0, nSize * sizeof(T));
		}
	}
	catch ( const std::bad_alloc& e ) 
	{
		pPointer = nullptr;

		return false;
	}

	return true;
}

template <class T>
inline bool CStdTpl::VectorContains(const std::vector<T>& vTs, const T& value)
{
	return VectorFind(vTs, value) != -1;
}

template <class T>
bool CStdTpl::VectorContains(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, const VectorContainFlag& nStrictSeq /*= NoSeq*/)
{
	if (vTsPart.size() == 0)
	{
		//空集是任何集合的子集
		return true;
	}

	switch (nStrictSeq)
	{
	case eCircleReverseContain:
		return VectorContainsStrictCircle(vTsSum, vTsPart, false);
		//break;
	case eReverseContain:
		//如果判断必须严格按照顺序包含
		return VectorContainsStrict(vTsSum, vTsPart, false);
		//break;
	case eSequenceContain:
		//如果判断必须严格按照顺序包含
		return VectorContainsStrict(vTsSum, vTsPart, true);
		//break;
	case eCircleSequenceContain:
		return VectorContainsStrictCircle(vTsSum, vTsPart, true);
		//break;
	default:
		{
			for (size_t i = 0; i < vTsPart.size(); ++i)
			{
				if (!VectorContains(vTsSum, vTsPart[i]))
				{
					return false;
				}
			}

			return true;
		}
		//break;
	}

	return false;
}

template <class T>
inline bool CStdTpl::VectorContainsStrict(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing /*= true*/)
{
	if (vTsPart.size() == 0)
	{
		//空集是任何集合的子集
		return true;
	}
	if (vTsPart.size() == 1)
	{
		//查找是否包含即可
		return VectorContains(vTsSum, vTsPart[0]);
	}
	else
	{
		//一一对应查找
		const size_t nSumSize = vTsSum.size();
		const size_t nPartSize = vTsPart.size();

		if (nSumSize < nPartSize)
		{
			return false;
		}

		std::vector<T> vTsPartBak(vTsPart);
		if (!bPositiveGoing)
		{
			std::reverse(vTsPartBak.begin(), vTsPartBak.end());
		}

		//找出第一个元素在原来合集中第一次出现的位置
		int nSIndex = VectorFind(vTsSum, vTsPartBak[0]);
		//找出最后一个元素在原来合集中反向第一次出现的位置
		int nEIndex = VectorFind(vTsSum, vTsPartBak[vTsPartBak.size() - 1], false);

		if (nSIndex == -1 || nEIndex == -1 || nSIndex >= nEIndex)
		{
			return false;
		}

		for (int i = nSIndex; i < nEIndex && i - nSIndex < nPartSize; ++i)
		{
			if (vTsSum[i] != vTsPartBak[i - nSIndex])
			{
				return false;
			}
		}

		return true;
	}
}

template <class T>
inline bool CStdTpl::VectorContainsStrictCircle(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing /*= true*/)
{
	if (vTsPart.size() == 0)
	{
		//空集是任何集合的子集
		return true;
	}
	if (vTsPart.size() == 1)
	{
		//查找是否包含即可
		return VectorContains(vTsSum, vTsPart[0]);
	}
	else
	{
		//一一对应查找
		const size_t nSumSize = vTsSum.size();
		const size_t nPartSize = vTsPart.size();

		if (nSumSize < nPartSize)
		{
			return false;
		}

		std::vector<T> vTsPartBak(vTsPart);
		if (!bPositiveGoing)
		{
			std::reverse(vTsPartBak.begin(), vTsPartBak.end());
		}

		//找出第一个元素在原来合集中第一次出现的位置
		int nSIndex = VectorFind(vTsSum, vTsPartBak[0]);
		//找出最后一个元素在原来合集中反向第一次出现的位置
		int nEIndex = VectorFind(vTsSum, vTsPartBak[vTsPartBak.size() - 1], false);

		if (nSIndex == -1 || nEIndex == -1 || nSIndex == nEIndex)
		{
			return false;
		}

		if (nSIndex < nEIndex)
		{
			for (int i = nSIndex; i <= nEIndex && i - nSIndex < nPartSize; ++i)
			{
				if (vTsSum[i] != vTsPartBak[i - nSIndex])
				{
					return false;
				}
			}
		}
		else
		{
			//公共边可能被首尾点截断
			int nTmpPos = 0;
			for (int i = nSIndex; i < nSumSize && i - nSIndex < nPartSize; ++i)
			{
				if (vTsSum[i] != vTsPartBak[i - nSIndex])
				{
					return false;
				}
				nTmpPos++;
			}

			for (int i = 0; i <= nEIndex && i + nTmpPos < nPartSize; ++i)
			{
				if (vTsSum[i] != vTsPartBak[i + nTmpPos])
				{
					return false;
				}
			}
		}

		return true;
	}
}

template <class T>
inline int CStdTpl::VectorFind(const std::vector<T>& vTs, const T& value, bool bPositiveGoing /*= true*/)
{
	if (bPositiveGoing)
	{
		for (int i = 0; i < int(vTs.size()); ++i)
		{
			if (vTs[i] == value)
			{
				return i;
			}
		}
	}
	else
	{
		for (int i = int(vTs.size() - 1); i >= 0; --i)
		{
			if (vTs[i] == value)
			{
				return i;
			}
		}
	}

	return -1;
}

template <class T>
inline void* CStdTpl::ConvertFromString(T &value, const _tstring &s)
{
	_tstringstream ss(s);

	return ss >> value;
}

template <class T>
inline _tstring CStdTpl::ConvertToString(T value)
{
#ifdef _UNICODE
	std::wstringstream ss;
#else
	std::stringstream ss;
#endif
	
	ss << value;

	return ss.str();
}

inline _tstring ToString(const double& dValue, const int& nLenDec = 6)
{
	char tmp[256];
	_tstring strFormat = _T("%.") + CStdTpl::ConvertToString(nLenDec) + _T("lf");

#ifdef _UNICODE
	sprintf_s(tmp, CStdStr::ws2s(strFormat).c_str(), dValue);
	return CStdStr::s2ws(tmp);
#else
	sprintf_s(tmp, strFormat.c_str(), dValue);
	return _tstring(tmp);
#endif
}
