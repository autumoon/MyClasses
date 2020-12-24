// MyListCtrl.cpp : 实现文件
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

		//已经在列表中
		if (strCurItem == strItem)
		{
			return nItemNum;
		}

		vItems.Add(strCurItem);
	}

	//首先要存在
	if (PathFileExists(strItem))
	{
		int nPos = GetItemCount();

		//根据类型判断
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


// CMyListCtrl 消息处理程序


void CMyListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(hDropInfo)
	{
		//拖拽文件的数量
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		//只拖拽一个文件
		if (nDrag > 1 && (m_nFlag & LISTCTRL_SIG_JUDGE))
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
			AddItemToList(szItemPath);
		}

		delete[] szItemPath;
		szItemPath = nullptr;
	}

	CListCtrl::OnDropFiles(hDropInfo);
}
