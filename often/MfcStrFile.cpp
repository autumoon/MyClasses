#include "MfcStrFile.h"

// ..\..\Common\often;

bool CMfcStrFile::StartProcess(const _tstring& stCommand, bool bShow)
{
	if (stCommand.length() == 0)
	{
		return false;
	}

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	ZeroMemory (&si, sizeof (STARTUPINFO));     //��ʼ��

	si.cb = sizeof (STARTUPINFO);
	si.wShowWindow = bShow ? SW_SHOW : SW_HIDE; 
	si.dwFlags = STARTF_USESHOWWINDOW; 

	return ::CreateProcess (NULL, (TCHAR*)stCommand.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi) == TRUE;
}

bool CMfcStrFile::StartProcess(const _tstring& stProgram, const _tstring& stFileOrCmd, bool bShow, bool bWait)
{
	_tstring stProgrammer(stProgram);

	if (stProgram.length() == 0)
	{
		return false;
	}

	if (stProgram.at(0) != '\"' && stProgram.find(' ') != _tstring::npos)
	{
		stProgrammer = _T("\"") + stProgrammer + _T("\"");
	}

	SHELLEXECUTEINFO ShExecInfo= {0};

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	ShExecInfo.fMask =SEE_MASK_NOCLOSEPROCESS;

	ShExecInfo.hwnd = NULL;

	ShExecInfo.lpVerb = _T("open");

	ShExecInfo.lpFile = stProgrammer.c_str();

	ShExecInfo.lpParameters = stFileOrCmd.c_str();

	ShExecInfo.lpDirectory = NULL;

	ShExecInfo.nShow = bShow ? SW_SHOW : SW_HIDE;

	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);

	if (bWait)
	{
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	}

	return true;
}

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
	strBak.ReleaseBuffer();

	return pChar; //��ȡstr�Ŀ��ַ������鱣��  
}

CString CMfcStrFile::BrowseDir(bool bNewButton /*= false*/, const CString& strTips/* = _T("��ѡ���ļ���")*/)
{
	CString szFileFolderPath;
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO biFolder;
	biFolder.hwndOwner = NULL;
	biFolder.pidlRoot = NULL;
	biFolder.pszDisplayName = NULL;
	biFolder.lpszTitle = strTips;

	biFolder.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;

	if (bNewButton)
	{
		biFolder.ulFlags |= BIF_NEWDIALOGSTYLE;
	}
	
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

CString CMfcStrFile::OpenSuffixFile(const _tstring& strSuffix)
{
	_tstring strSuffixBak(strSuffix);

	if (strSuffixBak.length() > 0 && strSuffixBak[0] == '.')
	{
		//delete the '.' before suffix
		strSuffixBak.erase(0, 1);
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString((strSuffixBak + _T("�ļ���*.") + 
		strSuffixBak + _T(")|*.") + strSuffixBak + _T("|�����ļ�(*.*)|*.*||")).c_str()));

	CString szFileName("");

	if (dlg.DoModal() == IDOK)
	{
		szFileName = dlg.GetPathName();
	}

	return szFileName;
}

CString CMfcStrFile::OpenSuffixFile(const int nSuffix, ...)
{
	va_list argp;
	va_start(argp, nSuffix);

	CStringArray arrSuffixs;
	CString strSuffix;
	for (int i = 0; i < nSuffix; i++)
	{
		strSuffix = va_arg(argp, TCHAR*);
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
		CString strTemp = sSuffix + "�ļ���*." + sSuffix + ")|*." + sSuffix + "|";
		CFileDialog dlg(FALSE, CString(strSuffix.c_str()), CString(strDefaultName.c_str()), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strTemp);

		if (dlg.DoModal() == IDOK)
		{
			strFilePath = dlg.GetPathName();
		}
	}

	return strFilePath;
}

int CMfcStrFile::IsFileDir(const CString& strFilePath)
{
	CString strPathBak(strFilePath);
	strPathBak.TrimRight(_T("\\"));
	//·��������
	if (!PathFileExists(strPathBak))
	{
		return 0;
	}

	WIN32_FIND_DATA FindFileData;
	FindFirstFile(strPathBak, &FindFileData);

	return FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? 1 : -1;
}

size_t CMfcStrFile::OpenMultiFiles(CStringArray&arrFiles, const int nSuffix, ...)
{
	va_list argp;
	va_start(argp, nSuffix);

	CStringArray arrSuffixs;
	CString strSuffix;
	for (int i = 0; i < nSuffix; i++)
	{
		strSuffix = va_arg(argp, TCHAR*);
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

	CFileDialog fpDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, strTemp + "�����ļ�(*.*)|*.*||", NULL); //

	DWORD MAXFILE = 2000 * MAX_PATH;;//��ʼ���ռ��С����������ʼ���ռ䣬����fd.DoModal()���Լ�����IDCANCELֵ  

	fpDlg.m_ofn.nMaxFile = MAXFILE;  

	TCHAR * pc = new TCHAR[MAXFILE]();//����ΪT���ͣ��ɱ���ת��  

	fpDlg.m_ofn.lpstrFile = pc;  

	fpDlg.m_ofn.lpstrFile[0] = NULL;//���ļ�ѡ��Ի����ʼ��

	std::string strImgPath;

	if(IDOK == fpDlg.DoModal())
	{
		POSITION pos  = fpDlg.GetStartPosition(); 

		while(pos)
		{
			CString imgPath = fpDlg.GetNextPathName(pos); 
			arrFiles.Add(imgPath);
		}
	}

	delete[] pc;
	pc=NULL;

	return arrFiles.GetCount();
}

size_t CMfcStrFile::OpenMultiKindFiles(CStringArray&arrFiles, const int nSuffix, ...)
{
	va_list argp;
	va_start(argp, nSuffix);

	CStringArray arrSuffixs;
	CString strSuffix;
	for (int i = 0; i < nSuffix; i++)
	{
		strSuffix = va_arg(argp, TCHAR*);
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

	CString strTemp("֧�ֵ�����|");
	for (int i = 0; i < nSuffix; i++)
	{
		strTemp += "*." + arrSuffixs[i];

		if (i != nSuffix - 1)
		{
			strTemp += ";";
		}
	}

	CFileDialog fpDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, strTemp + "|", NULL); //

	DWORD MAXFILE = 2000 * MAX_PATH;;//��ʼ���ռ��С����������ʼ���ռ䣬����fd.DoModal()���Լ�����IDCANCELֵ  

	fpDlg.m_ofn.nMaxFile = MAXFILE;  

	TCHAR * pc = new TCHAR[MAXFILE]();//����ΪT���ͣ��ɱ���ת��  

	fpDlg.m_ofn.lpstrFile = pc;  

	fpDlg.m_ofn.lpstrFile[0] = NULL;//���ļ�ѡ��Ի����ʼ��

	std::string strImgPath;

	if(IDOK == fpDlg.DoModal())
	{
		POSITION pos  = fpDlg.GetStartPosition(); 

		while(pos)
		{
			CString imgPath = fpDlg.GetNextPathName(pos); 
			arrFiles.Add(imgPath);
		}
	}

	delete[] pc;
	pc=NULL;

	return arrFiles.GetCount();
}

size_t CMfcStrFile::Split(const CString& str, CStringArray& Arr, const TCHAR& ch /*= ','*/)
{
	CString strTmp(str);
	if (strTmp.GetLength() == 0)
	{
		return 0;
	}

	//Ϊ���㴦�����һ��ch
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
	if (strBak.GetLength() == 0)
	{
		return 0;
	}
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

