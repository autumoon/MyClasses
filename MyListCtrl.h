#pragma once

// CMyListCtrl

/*使用范例m_eDstDir.SetFlag(EDIT_DIR_JUDGE | EDIT_SIG_JUDGE);*/
#define LISTCTRL_NONE_JUDGE		0x0000		//无判断
#define LISTCTRL_FILE_JUDGE		0x0001		//判断是否为文件
#define LISTCTRL_DIR_JUDGE		0x0002		//判断是否为目录
#define LISTCTRL_SIG_JUDGE		0x0004		//是否设置只允许单个项目

class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

	//返回项目个数
	int AddItemToList(const CString& strItem);

	void SetSingleColumn(const CString& strTipsInfo = _T("待处理的项目(可拖拽添加):"));

	//设置拖拽项目类型判断
	void SetFlag(const int& nFlag = LISTCTRL_NONE_JUDGE){m_nFlag = nFlag;}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	int m_nFlag;
};


