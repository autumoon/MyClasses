#pragma once

/************************************************************************/
//author:autumoon
//2018.04.18
//���˶�������ڿ��ƽ��ȵĻ��࣬����ȫ�����麯��
/************************************************************************/

class CProgressInterface
{
public:
	virtual ~CProgressInterface(){};
	virtual int Start() = 0;

	virtual int SetProgressValue(
		/* [in] */ size_t ullCompleted,
		/* [in] */ size_t ullTotal){return 0;};
		
	virtual int OutputInfo(
		/* [in] */ const char* lpszFormat, ...){return 0;};

	virtual int End() = 0;
};

#ifndef __AFX_H__
/************************************************************************/
/* �����Ƿ�������ֱ��ʹ�ã�Ҳ���Թرտ��ص����±�д                     */
/* ����1�������н�����ʾ                                                */
/************************************************************************/
#include <stdarg.h>

class CcmdProgress: public CProgressInterface
{
public:

	~CcmdProgress(){};

	int Start()
	{
		printf("Start...\n");

		return 0;
	}

	int SetProgressValue( 
		/* [in] */ size_t ullCompleted,
		/* [in] */ size_t ullTotal)
	{
		printf("Current progress is %d / %d\n", (int)ullCompleted, (int)ullTotal);

		return 0;
	}

	int OutputInfo(
		/* [in] */ const char* lpszFormat, ...)
	{
		char szInfo[1024];

		va_list argList;
		va_start(argList, lpszFormat);
		vsnprintf(szInfo, 1024, lpszFormat, argList);
		va_end(argList);

		printf("%s\n", szInfo);

		return 0;
	}

	int End()
	{
		printf("All Over!\n");

		return 0;
	}
};

#else	//__AFX_H__

/************************************************************************/
/* �����Ƿ�������ֱ��ʹ�ã�Ҳ���Թرտ��ص����±�д                     */
/* ����2:������������                                                   */
/************************************************************************/
#include <afxwin.h>
#include <locale.h>
#include "ShObjIdl.h"

class CTaskBarProgress : public CProgressInterface
{
	HWND m_hWnd;
	ITaskbarList3* m_pTaskbarList;
public:
	CTaskBarProgress(HWND hWnd) : m_hWnd(hWnd)
	{
		m_pTaskbarList = nullptr;
		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&m_pTaskbarList);
	}

	~CTaskBarProgress()
	{
		if (m_pTaskbarList)
		{
			m_pTaskbarList->Release();
			m_pTaskbarList = nullptr;
		}
	}

	int Start()
	{
		if (m_pTaskbarList && m_hWnd)
		{
			m_pTaskbarList->SetProgressState(m_hWnd, TBPF_INDETERMINATE);
		}

		return 0;
	}

	int SetProgressValue(
		/* [in] */ size_t ullCompleted,
		/* [in] */ size_t ullTotal)
	{
		if (m_pTaskbarList && m_hWnd)
		{
			m_pTaskbarList->SetProgressValue(m_hWnd, ullCompleted, ullTotal);
		}

		return 0;
	}

	int OutputInfo(
		/* [in] */ const char* lpszFormat, ...)
	{
		char szInfo[1024];

		va_list argList;
		va_start(argList, lpszFormat);
		vsnprintf_s(szInfo, 1024, lpszFormat, argList);
		va_end(argList);

		if (m_pTaskbarList && m_hWnd)
		{
			size_t convertedChars = 0;
			std::string curLocale = setlocale(LC_ALL,NULL);
			setlocale(LC_ALL,"chs");
			const char* source = szInfo;
			size_t charNum = sizeof(szInfo);

			wchar_t* dest = new wchar_t[charNum];
			mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
			m_pTaskbarList->SetThumbnailTooltip(m_hWnd, dest);
			delete[] dest;
			setlocale(LC_ALL,curLocale.c_str());
		}

		return 0;
	}

	int End()
	{
		if (m_pTaskbarList && m_hWnd)
		{
			m_pTaskbarList->SetProgressState(m_hWnd, TBPF_NOPROGRESS);
		}

		return 0;
	}
};
#endif // __AFX_H__