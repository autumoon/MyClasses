#pragma once

// CMyListCtrl

/*ʹ�÷���m_eDstDir.SetFlag(EDIT_DIR_JUDGE | EDIT_SIG_JUDGE);*/
#define LISTCTRL_NONE_JUDGE		0x0000		//���ж�
#define LISTCTRL_FILE_JUDGE		0x0001		//�ж��Ƿ�Ϊ�ļ�
#define LISTCTRL_DIR_JUDGE		0x0002		//�ж��Ƿ�ΪĿ¼
#define LISTCTRL_SIG_JUDGE		0x0004		//�Ƿ�����ֻ��������Ŀ

class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

	//������Ŀ����
	int AddItemToList(const CString& strItem);

	void SetSingleColumn(const CString& strTipsInfo = _T("���������Ŀ(����ק���):"));

	//������ק��Ŀ�����ж�
	void SetFlag(const int& nFlag = LISTCTRL_NONE_JUDGE){m_nFlag = nFlag;}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	int m_nFlag;
};


