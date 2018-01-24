#include "MfcStrFile.h"

std::string CMfcStrFile::CString2string(const CString& strCstring)
{
	const char* pChar = CStringToChar(strCstring);
	std::string strDst(pChar);
	delete[] pChar;
	pChar = nullptr;

	return strDst;
}

char* CMfcStrFile::CStringToChar(const CString& strCstring)
{
	CString strBak(strCstring);
#ifdef _UNICODE
	setlocale(LC_CTYPE, "chs");//�趨֧������·��
	wchar_t *pWChar = strBak.GetBuffer(); //��ȡstr�Ŀ��ַ������鱣��  
	strBak.ReleaseBuffer();
	int nLen = strBak.GetLength(); //��ȡstr���ַ���  
	char *pChar = new char[nLen * 2 + 1];
	memset(pChar, 0, nLen * 2 + 1);
	size_t i;
	size_t rtnVal = wcstombs_s(&i, pChar, nLen * 2 + 1, pWChar, _TRUNCATE); //���ַ�ת��Ϊ���ֽ��ַ�
	setlocale(LC_ALL, "C"); //��ԭ�����趨

	return pChar; //ע����ڴ沢δ�ͷ�
#else
	char *pDst = new char[strBak.GetLength() + 1];
	strcpy(pDst, strBak);

	return pDst;
#endif // _UNICODE
}

CString CMfcStrFile::BrowseDir(const CString& strTips/* = CString("��ѡ���ļ���")*/)
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

CString CMfcStrFile::BrowseDirNew(const CString& strTips/* = CString("��ѡ���ļ���")*/)
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
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("�����ļ�(*.*)|*.*||"));

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::OpenSuffixFile(const std::string& strSuffix)
{
	std::string strSuffixBak(strSuffix);

	if (strSuffixBak[0] == '.')
	{
		//delete the '.' before suffix
		strSuffixBak.erase(0, 1);
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString((strSuffixBak + "�ļ���*." + strSuffixBak + ")|*." + strSuffixBak + "|�����ļ�(*.*)|*.*||").c_str()));

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

	//�򿪶�������
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
		strTemp += arrSuffixs[i] + "�ļ���*." + arrSuffixs[i] + ")|*." + arrSuffixs[i] + "|";
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strTemp + "�����ļ�(*.*)|*.*||");

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::SaveSuffixFile(const std::string& strSuffix)
{
	CString sSuffix(strSuffix.c_str()), strFilePath;
	size_t nIndex = strSuffix.rfind('.');
	if (nIndex != std::string::npos)
	{
		sSuffix = sSuffix.Mid(int(nIndex + 1), sSuffix.GetLength() - int(nIndex) - 1);
	}

	if (sSuffix.GetLength() != 0)
	{
		CString strTemp = sSuffix + "�ļ���*." + sSuffix + ")|*." + sSuffix + "|";
		CFileDialog dlg(FALSE, CString(strSuffix.c_str()), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strTemp);

		if (dlg.DoModal() == IDOK)
		{
			strFilePath = dlg.GetPathName();
		}
	}

	return strFilePath;
}

size_t CMfcStrFile::Split(const CString& str, CStringArray& Arr, const char& ch /*= ','*/)
{
	CString strTmp(str);
	//Ϊ���㴦�����һ��ch
	strTmp += ch;
	int nFindposi  = strTmp.Find(ch);
	while( nFindposi != std::string::npos)
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
	//��չ�ַ����Է������
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
