// MyListCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PairSubtitles.h"
#include "MyListCtrl.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{
	m_nFlag = LISTCTRL_NONE_JUDGE;
}

CMyListCtrl::~CMyListCtrl()
{
}

int CMyListCtrl::AddItemToList(const CString& strItem)
{
	CStringArray vItems;
	int nItemNum = GetItemCount();
	for (int i = 0; i < nItemNum; ++i)
	{
		CString strCurItem = GetItemText(i, 0);

		//�Ѿ����б���
		if (strCurItem == strItem)
		{
			return nItemNum;
		}

		vItems.Add(strCurItem);
	}

	//����Ҫ����
	if (PathFileExists(strItem))
	{
		int nPos = GetItemCount();

		//���������ж�
		if ((m_nFlag & LISTCTRL_DIR_JUDGE) && PathIsDirectory(strItem))
		{
			InsertItem(nPos, strItem);
		}
		else if ((m_nFlag & LISTCTRL_FILE_JUDGE) && !PathIsDirectory(strItem))
		{
			InsertItem(nPos, strItem);
		}
		else if (m_nFlag == LISTCTRL_NONE_JUDGE)
		{
			InsertItem(nPos, strItem);
		}

		return true;
	}

	return false;
}

void CMyListCtrl::SetSingleColumn(const CString& strTipsInfo)
{
	CString strAddItemTips = strTipsInfo;

	CRect rListRect;
	GetClientRect(rListRect);
	InsertColumn(0, strAddItemTips, LVCFMT_LEFT, rListRect.Width());
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMyListCtrl ��Ϣ�������


void CMyListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(hDropInfo)
	{
		//��ק�ļ�������
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		//ֻ��קһ���ļ�
		if (nDrag > 1 && (m_nFlag & LISTCTRL_SIG_JUDGE))
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
			AddItemToList(szItemPath);
		}

		delete[] szItemPath;
		szItemPath = nullptr;
	}

	CListCtrl::OnDropFiles(hDropInfo);
}
