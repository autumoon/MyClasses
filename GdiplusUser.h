/* ******* GdiplusUserU.h **********
********* GDI+Í¼Ïñ²Ù×÷º¯ÊýÉùÃ÷ ********** */

/* author: autumoon */

#pragma once

#include <afx.h>        //Cstring
#include <gdiplus.h>
using namespace Gdiplus;
#include "StrDirFileU.h"

#pragma comment(lib, "lib/gdiplus.lib")

class CGdiplusUser
{
public:
	CGdiplusUser();
	~CGdiplusUser();

private:
	GdiplusStartupInput m_Gdistart; 
	ULONG_PTR m_GdiplusToken; 
	CStrDirFile sdf;

private:
	int GetImageCLSID(const WCHAR* format, CLSID* pCLSID);

public:
	int ConvertOneImage(const CString& strLoadFile, const CString& strSaveFile);
	int ConvertImageInDir(const CString& strDirSrc, const CString& strDirDst, const CString& strSuffix = _T(".jpg"));
};