// MyEdit.cpp : 实现文件
//

//拷贝到工程目录下使用

#include "stdafx.h"
#include "MyEdit.h"

// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit():m_nFlag(EDIT_NONE_JUDGE)
{
	
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMyEdit 消息处理程序

void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(hDropInfo)
	{
		//拖拽文件的数量
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		//只拖拽一个文件
		if (nDrag > 1 && (m_nFlag & EDIT_SIG_JUDGE))
		{
			nDrag = 1;
		}

		// 被拖拽的文件的文件名
		TCHAR* szItemPath = new TCHAR[MAX_PATH + 1]();

		CString strAllPath;
		for (UINT i = 0; i < nDrag; ++i)
		{
			memset(szItemPath, 0, sizeof(TCHAR) * (MAX_PATH + 1));
			DragQueryFile(hDropInfo, i, szItemPath, MAX_PATH);
			if (PathFileExists(szItemPath) == TRUE)
			{
				//根据类型判断
				if ((m_nFlag & EDIT_DIR_JUDGE) && PathIsDirectory(szItemPath))
				{
					strAllPath += szItemPath;
				}
				else if ((m_nFlag & EDIT_FILE_JUDGE) && !PathIsDirectory(szItemPath))
				{
					strAllPath += szItemPath;
				}
				else if (m_nFlag == EDIT_NONE_JUDGE)
				{
					strAllPath += szItemPath;
				}

				//超一个项目的时添加间隔符
				if (nDrag > 1)
				{
					strAllPath += ";";
				}
			}

			SetWindowText(strAllPath);
		}

		delete[] szItemPath;
		szItemPath = nullptr;
	}
	CEdit::OnDropFiles(hDropInfo);
}
