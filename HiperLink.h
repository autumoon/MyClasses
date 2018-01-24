//HiperLink.H
#ifndef __HYPERLINK_H__
#define __HYPERLINK_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class AFX_EXT_CLASS CHyperLink : public CStatic
{
// Construction/destruction
public:
    CHyperLink();
    virtual ~CHyperLink();
// Attributes
public:
	CString m_strWebsite;
// Operations
public:
    BOOL SetCursor(HCURSOR hCursor);
    void SetInfo(LPTSTR sMailAddr,LPTSTR sName, LPTSTR sBadTips);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CHyperLink)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Protected attributes
protected:
    COLORREF m_crLinkColour;
    COLORREF m_crHoverColour;
    BOOL     m_bOverControl;
    HCURSOR  m_hLinkCursor;
    CFont    m_Font;
    CString  m_MailAddr;
    CString  m_ShowName;
	CString  m_BadTips;
    // Generated message map functions
protected:
    //{{AFX_MSG(CHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG
    afx_msg void OnClicked();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
