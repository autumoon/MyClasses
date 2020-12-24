//author:autumoon
//2020.09.25 联系QQ4589968

#pragma once

/*使用范例m_eDstDir.SetFlag(EDIT_DIR_JUDGE | EDIT_SIG_JUDGE);*/
#define EDIT_NONE_JUDGE		0x0000		//无判断
#define EDIT_FILE_JUDGE		0x0001		//判断是否为文件
#define EDIT_DIR_JUDGE		0x0002		//判断是否为目录
#define EDIT_SIG_JUDGE		0x0004		//是否设置只允许单个项目

// CMyEdit

class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)
public:
	CMyEdit();
	virtual ~CMyEdit();

	//设置拖拽项目类型判断
	void SetFlag(const int& nFlag = EDIT_NONE_JUDGE){m_nFlag = nFlag;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	int m_nFlag;
};


