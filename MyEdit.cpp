// MyEdit.cpp : ʵ���ļ�
//

//����������Ŀ¼��ʹ��

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


// CMyEdit ��Ϣ�������

void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(hDropInfo)
	{
		//��ק�ļ�������
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		//ֻ��קһ���ļ�
		if (nDrag > 1 && (m_nFlag & EDIT_SIG_JUDGE))
		{
			nDrag = 1;
		}

		// ����ק���ļ����ļ���
		TCHAR* szItemPath = new TCHAR[MAX_PATH + 1]();

		CString strAllPath;
		for (UINT i = 0; i < nDrag; ++i)
		{
			memset(szItemPath, 0, sizeof(TCHAR) * (MAX_PATH + 1));
			DragQueryFile(hDropInfo, i, szItemPath, MAX_PATH);
			if (PathFileExists(szItemPath) == TRUE)
			{
				//���������ж�
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

				//��һ����Ŀ��ʱ��Ӽ����
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
