// 网络配置Dlg.h : header file
//

#if !defined(AFX_DLG_H__5B14CF34_C3D6_437C_A41B_04B9AEE12541__INCLUDED_)
#define AFX_DLG_H__5B14CF34_C3D6_437C_A41B_04B9AEE12541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tagAdapterInfo.h"
#include "AddipConfDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

#define MAXLEN       10000
#define WM_SHOWTASK WM_USER+1//自定义消息

class CMyDlg : public CDialog
{
// Construction
public:
	BOOL ReadFileToString(const CString fileName,CString& contentStr);
	void CreateConfigFile(const CString fileName,const ADAPTER_INFO adapterinfo);
	void DisplayAdapterInfo(const ADAPTER_INFO adapterinfo);
	void DisplayIPConfInfo(const IPConfigInfo IPconf);
	void GetIPAddrFromString(const CString str, CString &IPStr);
	void GetIPConfigFromString(const CString IPStr,const CString str,IPConfigInfo& IPConf);
	BOOL RegGetIp(ADAPTER_INFO &pAI, LPCTSTR lpszAdapterName, int nIndex=0);
	BOOL GetAdapterInfo();
	BOOL RegSetIP(LPCTSTR lpszAdapterName, int nIndex, LPCTSTR pIPAddress, LPCTSTR pNetMask, LPCTSTR pNetGate, \
				  LPCTSTR pNameServer);
	BOOL ChangeAdapterStatus(LPCTSTR AdapterDeviceDesc, int Flag);
	BOOL AutoRun();
	BOOL UnAutoRun();
	BOOL IsAutoRun();

	CMyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_IPCONFIG_DIALOG };
	CComboBox	m_ipAddr;
	CComboBox	m_adapter;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSelchangeComboAdatper();
	afx_msg void OnSelchangeComboIpaddress();
	afx_msg void OnAddipBtn();
	afx_msg void OnDeleteipBtn();
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDisableBtn();
	afx_msg void OnEnableBtn();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAutoRun();
	afx_msg void OnAbout();
	afx_msg void OnExit();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg void OnUpdateAutorun(CCmdUI* pCmdUI);
	afx_msg void OnBUTTONAUTOooo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_AdapterNum;
	int m_InitIndex;
	NOTIFYICONDATA m_nid;
	HMENU m_hMenu;
	CMenu m_PopMenu;
	int m_IsAutoRun;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__5B14CF34_C3D6_437C_A41B_04B9AEE12541__INCLUDED_)
