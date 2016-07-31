// AddipConfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "网络配置.h"
#include "AddipConfDlg.h"
#include "tagAdapterInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddipConfDlg dialog
extern int CurrentNum;
extern void WrtieIPconfToFile(const CString fileName,const IPConfigInfo ipconf);
extern void GetFileName(int index,CString& fileName);

CAddipConfDlg::CAddipConfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddipConfDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddipConfDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAddipConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddipConfDlg)
	DDX_Control(pDX, IDC_ADDIP_DNS1, m_dns1);
	DDX_Control(pDX, IDC_ADDIP_DNS2, m_dns2);
	DDX_Control(pDX, IDC_ADDIP_SUBMASK, m_submask);
	DDX_Control(pDX, IDC_ADDIP_GATEWAY, m_gateway);
	DDX_Control(pDX, IDC_ADDIP_IPADDR, m_ipaddr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddipConfDlg, CDialog)
	//{{AFX_MSG_MAP(CAddipConfDlg)
	ON_UPDATE_COMMAND_UI(IDM_AUTORUN, OnUpdateAutorun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddipConfDlg message handlers

void CAddipConfDlg::OnOK() 
{
	// TODO: Add extra validation here
	IPConfigInfo IPconf;
	CString dns1,dns2,fileName;

	if(m_ipaddr.IsBlank()||m_gateway.IsBlank()||m_submask.IsBlank()||m_dns1.IsBlank())
	{
		MessageBox("请输入完整的信息！");
		return ;
	}
	
	GetIPFromIPCtrl(&m_ipaddr,IPconf.IPAddr);
	GetIPFromIPCtrl(&m_submask,IPconf.SubMask);
	GetIPFromIPCtrl(&m_gateway,IPconf.DefGateway);
	GetIPFromIPCtrl(&m_dns1,dns1);
	GetIPFromIPCtrl(&m_dns2,dns2);
	if(m_dns2.IsBlank())
		IPconf.NameServer+=dns1;
	else
	{
		IPconf.NameServer+=dns1;
		IPconf.NameServer+=",";
		IPconf.NameServer+=dns2;
	}
	
	GetFileName(CurrentNum,fileName);
	WrtieIPconfToFile(fileName,IPconf);
	CDialog::OnOK();
}

void CAddipConfDlg::GetIPFromIPCtrl(CIPAddressCtrl* ipctrl,CString& str)
{
	BYTE field0,field1,field2,field3;
	CString temp;
	
	ipctrl->GetAddress(field0,field1,field2,field3);
	temp.Format("%d",field0);
	str+=temp;
	str+=".";
	temp.Format("%d",field1);
	str+=temp;
	str+=".";
	temp.Format("%d",field2);
	str+=temp;
	str+=".";
	temp.Format("%d",field3);
	str+=temp;
}

void CAddipConfDlg::OnUpdateAutorun(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}
