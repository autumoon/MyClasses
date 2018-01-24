//HyperLink.CPP
// HyperLink.cpp : implementation file

#include "..\stdafx.h"
#include "HiperLink.h"
#include "..\Resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
    m_crLinkColour    = RGB( 0, 0, 238 );
    m_crHoverColour    = RGB( 255, 0, 255 );
    m_bOverControl    = FALSE;
}

CHyperLink::~CHyperLink()
{
    m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
    //{{AFX_MSG_MAP(CHyperLink)
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

void CHyperLink::OnClicked()
{
    CString strURL(m_MailAddr);
    
    //GetWindowText( strMailAddress );
    // 装配成 URL 地址
    //strURL.Format( _T("mailto:%s"), (LPCTSTR)m_MailAddr);
    
    // 调用 ShellExecute() 执行邮件发送
    int iRet = (int)ShellExecute( NULL, _T("open"), (LPCTSTR)strURL, NULL, NULL, SW_SHOW );
	if (iRet <= HINSTANCE_ERROR  && 
		MessageBox(_T("提示：发送邮件失败！\n系统未安装邮件处理程序，或未设置缺省的邮件处理程序！！\n\n是否打开我的头条号:音符的尾巴？"), 
		_T("哎哟，发不了邮件！"), MB_YESNO) == IDYES)
	{
		int iRet2 = (int)ShellExecute( NULL, _T("open"), _T("https://www.toutiao.com/c/user/64659995094/"), NULL, NULL, SW_SHOW );
		if( iRet2 <= HINSTANCE_ERROR )
			AfxMessageBox(_T("打开失败！\n系统未安装浏览器，或未设置缺省的浏览器！！\n\n"), MB_OK | MB_ICONEXCLAMATION );
	}
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

    if( m_bOverControl )
        pDC->SetTextColor(m_crHoverColour);
    else
        pDC->SetTextColor(m_crLinkColour);
    
    pDC->SetBkMode( TRANSPARENT );

    return (HBRUSH)GetStockObject( NULL_BRUSH );
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point) 
{
    CStatic::OnMouseMove(nFlags, point);

    if( m_bOverControl ) {
        CRect rc;
        GetClientRect( &rc );

        if( !rc.PtInRect(point) ) {
            m_bOverControl = FALSE;
            ReleaseCapture();
            RedrawWindow();
            return;
        }
    }
    else {
        m_bOverControl = TRUE;
        RedrawWindow();
        SetCapture();
    }
}

BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
    ::SetCursor( m_hLinkCursor );
    return TRUE;
}

void CHyperLink::PreSubclassWindow() 
{
    // 要求得到 STN_CLICKED 通知消息
    DWORD dwStyle = GetStyle();
    ::SetWindowLong( GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    // 设置字体下划线
    LOGFONT lf;
    GetFont()->GetLogFont( &lf );
    lf.lfUnderline = TRUE;
    m_Font.CreateFontIndirect( &lf );
    SetFont( &m_Font );

    CStatic::PreSubclassWindow();
}

void CHyperLink::SetInfo(LPTSTR sMailAddr, LPTSTR sName, LPTSTR sBadTips)
{
    m_MailAddr			= sMailAddr;
    m_ShowName			= sName;
	m_BadTips			= sBadTips;
}

BOOL CHyperLink::SetCursor(HCURSOR hCursor)
{
    m_hLinkCursor = hCursor;

    return TRUE;
}