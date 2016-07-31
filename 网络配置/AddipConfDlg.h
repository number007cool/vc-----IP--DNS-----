#if !defined(AFX_ADDIPCONFDLG_H__999D3121_500C_4B8E_B83C_F191B81367B0__INCLUDED_)
#define AFX_ADDIPCONFDLG_H__999D3121_500C_4B8E_B83C_F191B81367B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddipConfDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddipConfDlg dialog

class CAddipConfDlg : public CDialog
{
// Construction
public:
	CAddipConfDlg(CWnd* pParent = NULL);   // standard constructor
	void GetIPFromIPCtrl(CIPAddressCtrl* ipctrl,CString& str);
// Dialog Data
	//{{AFX_DATA(CAddipConfDlg)
	enum { IDD = IDD_ADDIPCONF_DIALOG };
	CIPAddressCtrl	m_dns2;
	CIPAddressCtrl	m_dns1;
	CIPAddressCtrl	m_submask;
	CIPAddressCtrl	m_gateway;
	CIPAddressCtrl	m_ipaddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddipConfDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddipConfDlg)
	virtual void OnOK();
	afx_msg void OnUpdateAutorun(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDIPCONFDLG_H__999D3121_500C_4B8E_B83C_F191B81367B0__INCLUDED_)
