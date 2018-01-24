#pragma once
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <direct.h>		//_mkdir������ͷ�ļ�
#include <io.h>

class CStdStr
{
public:
	//�ַ�����
	static bool IsFileContainsDir(const std::string& strFile, const std::string& strDir);
	static bool IsFilesContainsDir(const std::vector<std::string>& vFiles, const std::string& strDir);
	static std::string AddSlashIfNeeded(const std::string& strDir, const char& cDir = '\\');
	static std::string GetDirOfDir(const std::string& strDir, const char& cDir = '\\');
	static std::string GetDirOfFile(const std::string& strFile, const char& cDir = '\\');
	static std::string GetFilesCommonDir(const std::vector<std::string>& vFilesFullPath);
	static std::string GetRootDir(const std::string& strDirOrFile, const char& cDir = '\\');
	static std::string GetSuffixOfFile(const std::string& strFile, bool bWithDot = true);
	static std::string GetNameOfDir(const std::string& strDir, const char& cDir = '\\');
	static std::string GetNameOfFile(const std::string& strFile, const bool& bWithSuffix = true, const char& cDir = '\\');
	static std::string RepalceAll(const std::string& str,const std::string& old_value,const std::string& new_value = "");
	static std::string ReplaceAllDistinct(const std::string& str,const std::string& old_value,const std::string& new_value = "");
	static std::string ReplaceSuffix(const std::string& strFilePath, const std::string& strNewSuffix);
	static std::string ToUpperLower(const std::string& strOri, const bool& bToLower = true);
	static std::string Trim(const std::string& strOri, const char& cToTrim = ' ');
	static std::string TrimHead(const std::string& strOri, const char& cToTrim = ' ');
	static std::string TrimTail(const std::string& strOri, const char& cToTrim = ' ');
	static std::string ws2s(const std::wstring& ws);
	static std::wstring s2ws(const std::string& s);

	static std::vector<std::string> Split(const std::string& str, const std::string& pattern);
};

class CStdDir
{
public:
	static bool IfAccessDir(const std::string& strDirPath);
	static bool CreateDir(const std::string& strDir, const char& cDir = '\\');
};

class CStdFile
{
public:
	//�ļ�����
	static bool CompareFileDistinct(const std::string strLeftFile, const std::string strRightFile);
	static bool CopyAFile(const std::string& strSrcFileName, std::string& strDstFileName, const bool& bFailIfExists);
	static bool IfAccessFile(const std::string& strFilePath);
	
	static size_t ParseTXTFile(const std::string& strFilePath, std::list<std::string>& lContentInFile);
	static size_t ParseTXTFile(const std::string& strFilePath, std::vector<std::string>& vContentInFile);
	static int SaveTXTFile(const std::string& strTxtPath, std::list<std::string>& lContent, bool bAppend = false);
	static int SaveTXTFile(const std::string& strTxtPath, std::vector<std::string>& vContent, bool bAppend = false);
	static int SaveTXTFile(const std::string& strTxtPath, const std::string& strLine, bool bAppend = false);
	static __int64 GetFileSize(const std::string& strFilePath);
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
	//����ǽṹ������࣬�Դ����캯������bIniZero����Ϊfalse�������캯��ʧЧ
	template <class T>
	static bool NewSafely(T*& pPointer, const size_t& nSize = 1, const bool& bIniZero = false);
	template <class T>
	static bool VectorContains(const std::vector<T>& vTs, const T& value);
	//nStrictSeq = -2���������ϸ����,-1�����ϸ������0��������ֵ������˳��1�����ϸ������2���������ϸ����
	template <class T>
	static bool VectorContains(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, const VectorContainFlag& nStrictSeq = eNoSeq);
	template <class T>
	static int VectorFind(const std::vector<T>& vTs, const T& value, bool bPositiveGoing = true);
	template <class T>
	static int ConvertFromString(T &value, const std::string &s);
	template <class T>
	static std::string ConvertToString(T value);

private:
	template <class T>
	static bool VectorContainsStrict(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing = true);
	template <class T>
	static bool VectorContainsStrictCircle(const std::vector<T>& vTsSum, const std::vector<T>& vTsPart, bool bPositiveGoing = true);
};

/***********************��������ģ��ʵ��***********************/
inline bool CStdDir::IfAccessDir(const std::string& strDirPath)
{
	return _access(strDirPath.c_str(), 0) == 0 && !CStdFile::IfAccessFile(strDirPath);
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
		//�ռ����κμ��ϵ��Ӽ�
		return true;
	}

	switch (nStrictSeq)
	{
	case eCircleReverseContain:
		return VectorContainsStrictCircle(vTsSum, vTsPart, false);
		//break;
	case eReverseContain:
		//����жϱ����ϸ���˳�����
		return VectorContainsStrict(vTsSum, vTsPart, false);
		//break;
	case eSequenceContain:
		//����жϱ����ϸ���˳�����
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
		//�ռ����κμ��ϵ��Ӽ�
		return true;
	}
	if (vTsPart.size() == 1)
	{
		//�����Ƿ��������
		return VectorContains(vTsSum, vTsPart[0]);
	}
	else
	{
		//һһ��Ӧ����
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

		//�ҳ���һ��Ԫ����ԭ���ϼ��е�һ�γ��ֵ�λ��
		int nSIndex = VectorFind(vTsSum, vTsPartBak[0]);
		//�ҳ����һ��Ԫ����ԭ���ϼ��з����һ�γ��ֵ�λ��
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
		//�ռ����κμ��ϵ��Ӽ�
		return true;
	}
	if (vTsPart.size() == 1)
	{
		//�����Ƿ��������
		return VectorContains(vTsSum, vTsPart[0]);
	}
	else
	{
		//һһ��Ӧ����
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

		//�ҳ���һ��Ԫ����ԭ���ϼ��е�һ�γ��ֵ�λ��
		int nSIndex = VectorFind(vTsSum, vTsPartBak[0]);
		//�ҳ����һ��Ԫ����ԭ���ϼ��з����һ�γ��ֵ�λ��
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
			//�����߿��ܱ���β��ض�
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
inline int CStdTpl::ConvertFromString(T &value, const std::string &s)
{
	std::stringstream ss(s);
	ss >> value;

	return 0;
}

template <class T>
inline std::string CStdTpl::ConvertToString(T value)
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

inline std::string ToString(const double& dValue, const int& nLenDec = 6)
{
	char tmp[256];
	std::string strFormat = "%." + CStdTpl::ConvertToString(nLenDec) + "lf";
	sprintf_s(tmp,strFormat.c_str(), dValue);

	return std::string(tmp);
}
