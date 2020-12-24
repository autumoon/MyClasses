//author:autumoon
//2020.09.25 ��ϵQQ4589968

#pragma once

/*ʹ�÷���m_eDstDir.SetFlag(EDIT_DIR_JUDGE | EDIT_SIG_JUDGE);*/
#define EDIT_NONE_JUDGE		0x0000		//���ж�
#define EDIT_FILE_JUDGE		0x0001		//�ж��Ƿ�Ϊ�ļ�
#define EDIT_DIR_JUDGE		0x0002		//�ж��Ƿ�ΪĿ¼
#define EDIT_SIG_JUDGE		0x0004		//�Ƿ�����ֻ��������Ŀ

// CMyEdit

class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)
public:
	CMyEdit();
	virtual ~CMyEdit();

	//������ק��Ŀ�����ж�
	void SetFlag(const int& nFlag = EDIT_NONE_JUDGE){m_nFlag = nFlag;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	int m_nFlag;
};


