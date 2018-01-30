#include "MfcStrFile.h"

_tstring CMfcStrFile::CString2string(const CString& strCstring)
{
	const TCHAR* pChar = CStringToChar(strCstring);

	_tstring strDst(pChar);

	delete[] pChar;
	pChar = nullptr;

	return strDst;
}

TCHAR* CMfcStrFile::CStringToChar(const CString& strCstring)
{
	CString strBak(strCstring);

	int nLen = (strBak.GetLength() + 1) * sizeof(TCHAR);
	TCHAR* pChar = new TCHAR[nLen];

	memset(pChar, 0, sizeof(TCHAR) * nLen);
	memcpy(pChar, strBak.GetBuffer(), nLen - sizeof(TCHAR));

	return pChar; //获取str的宽字符用数组保存  
}

CString CMfcStrFile::BrowseDir(const CString& strTips/* = CString("请选择文件夹")*/)
{
	CString szFileFolderPath;
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO biFolder;
	biFolder.hwndOwner = NULL;
	biFolder.pidlRoot = NULL;
	biFolder.pszDisplayName = NULL;
	biFolder.lpszTitle = strTips;
	biFolder.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	biFolder.lpfn = NULL;
	biFolder.lParam = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&biFolder);
	if (!pidl)
	{
		return "";
	}
	else
	{
		SHGetPathFromIDList(pidl, pszPath);
		return pszPath;
	}
}

CString CMfcStrFile::BrowseDirNew(const CString& strTips/* = CString("请选择文件夹")*/)
{
	CString szFileFolderPath;
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO biFolder;
	biFolder.hwndOwner = NULL;
	biFolder.pidlRoot = NULL;
	biFolder.pszDisplayName = NULL;
	biFolder.lpszTitle = strTips;
	biFolder.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
	biFolder.lpfn = NULL;
	biFolder.lParam = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&biFolder);
	if (!pidl)
	{
		return "";
	}
	else
	{
		SHGetPathFromIDList(pidl, pszPath);
		return pszPath;
	}
}

CString CMfcStrFile::OpenFile()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("所有文件(*.*)|*.*||"));

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::OpenSuffixFile(const _tstring& strSuffix)
{
	_tstring strSuffixBak(strSuffix);

	if (strSuffixBak[0] == '.')
	{
		//delete the '.' before suffix
		strSuffixBak.erase(0, 1);
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString((strSuffixBak + _T("文件（*.") + 
		strSuffixBak + _T(")|*.") + strSuffixBak + _T("|所有文件(*.*)|*.*||")).c_str()));

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::OpenSuffixFile(const int& nSuffix, ...)
{
	va_list argp;
	va_start(argp, nSuffix);

	CStringArray arrSuffixs;
	CString strSuffix;
	for (int i = 0; i < nSuffix; i++)
	{
		strSuffix = va_arg(argp, char*);
		arrSuffixs.Add(strSuffix);
	}
	va_end(argp);

	//打开多种类型
	for (int i = 0; i < nSuffix; i++)
	{
		if (arrSuffixs[i].Left(1) == '.')
		{
			//delete the '.' before suffix
			arrSuffixs[i] = arrSuffixs[i].Mid(1, arrSuffixs[i].GetLength() - 1);
		}
	}

	CString strTemp("");
	for (int i = 0; i < nSuffix; i++)
	{
		strTemp += arrSuffixs[i] + "文件（*." + arrSuffixs[i] + ")|*." + arrSuffixs[i] + "|";
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strTemp + "所有文件(*.*)|*.*||");

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::SaveSuffixFile(const _tstring& strSuffix, const _tstring& strDefaultName/* = _T("autumoon")*/)
{
	CString sSuffix(strSuffix.c_str()), strFilePath;
	size_t nIndex = strSuffix.rfind('.');
	if (nIndex != _tstring::npos)
	{
		sSuffix = sSuffix.Mid(int(nIndex + 1), sSuffix.GetLength() - int(nIndex) - 1);
	}

	if (sSuffix.GetLength() != 0)
	{
		CString strTemp = sSuffix + "文件（*." + sSuffix + ")|*." + sSuffix + "|";
		CFileDialog dlg(FALSE, CString(strSuffix.c_str()), CString(strDefaultName.c_str()), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strTemp);

		if (dlg.DoModal() == IDOK)
		{
			strFilePath = dlg.GetPathName();
		}
	}

	return strFilePath;
}

size_t CMfcStrFile::Split(const CString& str, CStringArray& Arr, const TCHAR& ch /*= ','*/)
{
	CString strTmp(str);
	//为方便处理，添加一个ch
	strTmp += ch;
	int nFindposi  = strTmp.Find(ch);
	while( nFindposi != _tstring::npos)
	{
		Arr.Add(strTmp.Left(nFindposi) );
		strTmp = strTmp.Right( strTmp.GetLength() - nFindposi - 1);
		nFindposi  = strTmp.Find(ch);
	}

	if(!strTmp.IsEmpty())
		Arr.Add(strTmp);

	return Arr.GetCount();
}

size_t CMfcStrFile::Split(const CString& str, CStringArray& Arr, const CString& pattern /*= ","*/)
{
	CString strBak(str);
	int pos = -1;
	//扩展字符串以方便操作
	strBak += pattern;
	int size = strBak.GetLength();

	for (int i = 0; i < size; i++)
	{
		pos = strBak.Find(pattern, i);
		if (pos < size && pos != -1)
		{
			CString s = strBak.Mid(i, pos - i);
			Arr.Add(s);
			i = int(pos + pattern.GetLength() - 1);
		}
		else
		{
			break;
		}
	}

	return Arr.GetCount();
}
