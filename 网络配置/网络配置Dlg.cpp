// 网络配置Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "网络配置.h"
#include "网络配置Dlg.h"
#include <winsock2.h>
#include <setupapi.h>


#define  LZ_USE_PRINTF  1

#if LZ_USE_PRINTF==1
#pragma comment(linker, "/subsystem:\"CONSOLE\" /entry:\"WinMainCRTStartup\"")
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXNUM   20
#define UnknownDevice TEXT("<Unknown Device>")

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib,"setupapi")

/////////////////////////////////////////////////////////////////////////////
//全局数据和函数声明区
ADAPTER_INFO AdapterInfoVector[MAXNUM];
int CurrentNum=0;
HDEVINFO hDevInfo = 0;
static int flag=0;

BOOL stringcmp(char *base,char *des);//用base所指向的字符串去匹配des指向的字符串
void WriteInfoToFile(const CString filename,const IPConfigInfo disinfo);
void GetFileName(int index,CString& fileName);
void WrtieIPconfToFile(const CString fileName,const IPConfigInfo ipconf);
void AdatperInfoToIPConfInfo(const ADAPTER_INFO adatperinfo,IPConfigInfo& ipconf);
BOOL StateChange(DWORD NewState, DWORD SelectedItem,HDEVINFO hDevInfo);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_AdapterNum=0;
	m_InitIndex=0;
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_COMBO_IPADDRESS, m_ipAddr);
	DDX_Control(pDX, IDC_COMBO_ADATPER, m_adapter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_ADATPER, OnSelchangeComboAdatper)
	ON_CBN_SELCHANGE(IDC_COMBO_IPADDRESS, OnSelchangeComboIpaddress)
	ON_BN_CLICKED(IDC_ADDIP_BTN, OnAddipBtn)
	ON_BN_CLICKED(IDC_DELETEIP_BTN, OnDeleteipBtn)
	ON_MESSAGE(WM_SHOWTASK,OnShowTask)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_DISABLE_BTN, OnDisableBtn)
	ON_BN_CLICKED(IDC_ENABLE_BTN, OnEnableBtn)
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_AUTORUN,OnAutoRun)
	ON_COMMAND(IDM_ABOUT,OnAbout)
	ON_COMMAND(IDM_EXIT,OnExit)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(IDM_AUTORUN, OnUpdateAutorun)
	ON_BN_CLICKED(IDC_BUTTON_AUTOooo, OnBUTTONAUTOooo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//---------------------------托盘显示初始化---------------------------------//

    m_nid.cbSize  = (DWORD)sizeof(NOTIFYICONDATA);
    m_nid.hWnd    = this->m_hWnd;
    m_nid.uID     = IDR_MAINFRAME;
    m_nid.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
    m_nid.uCallbackMessage = WM_SHOWTASK;             // 自定义的消息名称
    m_nid.hIcon   = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

    //IDR_MAINFRAME可改为自己制作的资源名
    strcpy(m_nid.szTip, "网络配置");              // 信息提示条为"自制闹钟程序"
    Shell_NotifyIcon(NIM_ADD, &m_nid);                // 在托盘区添加图标
	///////////////////////////////////////////////////////////////////////////////////////
	
	//初始化网卡信息，添加到复选框
	GetAdapterInfo();
	for(int i=0;i<m_AdapterNum;i++)
	{
		m_adapter.InsertString(i,AdapterInfoVector[i].strDriverDesc);
	}
	m_adapter.SetCurSel(m_InitIndex);
	m_adapter. SetDroppedWidth(290);
	//显示当前复选框中的Adapter的IP信息
	DisplayAdapterInfo(AdapterInfoVector[m_InitIndex]);
	
	//如果配置文件不存在，则创建配置文件
	CString fileName,FileContent;
	for(i=0;i<m_AdapterNum;i++)
	{
		GetFileName(i,fileName);
		CreateConfigFile(fileName,AdapterInfoVector[i]);
	}

	//从文件中读取IP信息
	GetFileName(m_InitIndex,fileName);
	if(ReadFileToString(fileName,FileContent)!=FALSE)
	{
		//提取出FileContent中的IP地址项
		CString IPStr,temp;
		GetIPAddrFromString(FileContent,IPStr);
		//把IP地址项加入复选框
		int j=0,len=IPStr.GetLength();
		i=0;
		while(i<len)
		{
			temp.Empty();
			for(;IPStr.GetAt(i)!='#';i++)
				temp+=IPStr.GetAt(i);
			m_ipAddr.InsertString(j,temp);
			m_ipAddr.SetCurSel(j);
		
			j++;
			i++;
		}
		m_ipAddr.InsertString(j,"自动获取IP地址");
		m_ipAddr.SetCurSel(j+1);
		//
		IPConfigInfo IPConf;
		IPStr.Empty();
		m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),IPStr);
		GetIPConfigFromString(IPStr,FileContent,IPConf);
		DisplayIPConfInfo(IPConf);
	}
	else
	{
		IPConfigInfo IPConf;
		m_ipAddr.ResetContent();
		DisplayIPConfInfo(IPConf);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
//从注册表中得到所有的网卡信息
///////////////////////////////////////////////////////////////////////////////
BOOL CMyDlg::GetAdapterInfo()
{
	HKEY hKey,hSubKey,hNdiKey,hNdiIntKey;
	//打开注册表
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",\
		0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return FALSE;

	DWORD dwIndex = 0;
	DWORD dwBufSize = 256;
	DWORD dwDataType;
	char szSubKey[256]={0};
	unsigned char szData[256]={0};
	int flag=0;

	//先把适配器信息结构体数组AdapterInfoVector清空
	for(int i=0;i<m_AdapterNum;i++)
	{
		AdapterInfoVector[i].strDriverDesc.Empty();
		AdapterInfoVector[i].strIP.Empty();
		AdapterInfoVector[i].strName.Empty();
		AdapterInfoVector[i].strNameServer.Empty();
		AdapterInfoVector[i].strNetGate.Empty();
		AdapterInfoVector[i].strNetMask.Empty();
	}
	m_AdapterNum=0;
	
	//把所有适配器当前配置信息存入AdapterInfoVector中
	while(RegEnumKeyEx(hKey,dwIndex++,szSubKey,&dwBufSize,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
	{
		if(RegOpenKeyEx(hKey,szSubKey,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
		{
			int find=0;
			if(RegOpenKeyEx(hSubKey,"Ndi",0,KEY_READ,&hNdiKey)==ERROR_SUCCESS)
			{
				dwBufSize=256;
				if(RegQueryValueEx(hNdiKey,"Service",0,&dwDataType,szData,&dwBufSize)==ERROR_SUCCESS)
				{
					//判断虚拟网卡
					if(strcmp((char*)szData,"VMnetAdapter")==0)
						find=1;
					else if(RegOpenKeyEx(hSubKey,"Ndi\\Interfaces",0,KEY_READ,&hNdiIntKey)==ERROR_SUCCESS)
					{
						dwBufSize=256;
						if(RegQueryValueEx(hNdiIntKey,"LowerRange",0,&dwDataType,szData,&dwBufSize)==ERROR_SUCCESS)
						{
							//判断是不是以太网卡
							if(strcmp("ethernet",(char*)szData)==0)
							{
								flag=1;
								find=1;
							}//判断是不是无线网卡
							else if(stringcmp("ethernet",(char*)szData)==TRUE)
								find=1;
						}
						RegCloseKey(hNdiIntKey);
					}
					RegCloseKey(hNdiKey);
				}
				if(find)
				{
					dwBufSize=256;
					if(RegQueryValueEx(hSubKey,"DriverDesc",0,&dwDataType,szData,&dwBufSize)==ERROR_SUCCESS)
					{
						ADAPTER_INFO pAI;
						
						pAI.strDriverDesc.Format("%s",szData);
						dwBufSize = 256;
						if(RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
						{
							pAI.strName.Format("%s",szData);
							RegGetIp(pAI, (LPCTSTR)szData);
						}
						AdapterInfoVector[m_AdapterNum++]=pAI;// 加入到容器中
						if(flag==1)
						{
							m_InitIndex=m_AdapterNum-1;
							flag=0;
						}
							
					}
				}
				RegCloseKey(hSubKey);
			}
		}
		dwBufSize = 256;
	}
	RegCloseKey(hKey);

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
//从注册表中获得lpszAdapterName的IP配置信息
/////////////////////////////////////////////////////////////////////////////
BOOL CMyDlg::RegGetIp(ADAPTER_INFO& pAI, LPCTSTR lpszAdapterName, int nIndex)
{
	HKEY hKey;
	CString strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
	strKeyName += lpszAdapterName;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				strKeyName,
				0,
				KEY_READ,
				&hKey) != ERROR_SUCCESS)
		return FALSE;

	unsigned char szData[256];
	DWORD dwDataType, dwBufSize;

	dwBufSize = 256;
	if(RegQueryValueEx(hKey, "IPAddress", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
		pAI.strIP.Format("%s",szData);

	dwBufSize = 256;
	if(RegQueryValueEx(hKey, "SubnetMask", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
		pAI.strNetMask.Format("%s",szData);

	dwBufSize = 256;
	if(RegQueryValueEx(hKey, "DefaultGateway", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
		pAI.strNetGate.Format("%s",szData);

	dwBufSize = 256;
	if(RegQueryValueEx(hKey, "NameServer", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
		pAI.strNameServer.Format("%s",szData);
	
	RegCloseKey(hKey);
	return TRUE;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//把当前IP配置信息写入注册表
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMyDlg::RegSetIP(LPCTSTR lpszAdapterName, int nIndex, LPCTSTR pIPAddress, LPCTSTR pNetMask, LPCTSTR pNetGate,LPCTSTR pNameServer)
{
	HKEY hKey;
	CString strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
	strKeyName += lpszAdapterName;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				strKeyName,
				0,
				KEY_WRITE,
				&hKey) != ERROR_SUCCESS)
		return FALSE;
	
	char mszIPAddress[100]={0};
	char mszNetMask[100]={0};
	char mszNetGate[100]={0};
	char mszNameServer[100]={0};

	strncpy(mszIPAddress, pIPAddress, 98);
	strncpy(mszNetMask, pNetMask, 98);
	strncpy(mszNetGate, pNetGate, 98);
	strncpy(mszNameServer,pNameServer,98);

	int nIP, nMask, nGate,nServer;

	nIP = strlen(mszIPAddress);
	nMask = strlen(mszNetMask);
	nGate = strlen(mszNetGate);
	nServer= strlen(mszNameServer);

	*(mszIPAddress + nIP + 1) = 0x00;
	nIP += 2;

	*(mszNetMask + nMask + 1) = 0x00;
	nMask += 2;

	*(mszNetGate + nGate + 1) = 0x00;
	nGate += 2;

	*(mszNameServer + nServer + 1) = 0x00;
	nServer += 2;
	
	RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)mszIPAddress, nIP);
	RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)mszNetMask, nMask);
	RegSetValueEx(hKey, "DefaultGateway", 0, REG_MULTI_SZ, (unsigned char*)mszNetGate, nGate);
	RegSetValueEx(hKey, "NameServer", 0, REG_SZ, (unsigned char*)mszNameServer, nServer);

	RegCloseKey(hKey);
	return TRUE;

}
////////////////////////////////////////////////////////////////
//显示当前复选框中的适配器的IP信息
////////////////////////////////////////////////////////////////
void CMyDlg::DisplayAdapterInfo(const ADAPTER_INFO adapterinfo)
{
	GetDlgItem(IDC_STCAP_IPADDR)->SetWindowText(adapterinfo.strIP);
	GetDlgItem(IDC_STCAP_SUBMASK)->SetWindowText(adapterinfo.strNetMask);
	GetDlgItem(IDC_STCAP_GATEWAY)->SetWindowText(adapterinfo.strNetGate);
	GetDlgItem(IDC_STCAP_DNS)->SetWindowText(adapterinfo.strNameServer);


}
//////////////////////////////////////////////////////////
//显示当前IP地址的配置信息
//////////////////////////////////////////////////////////
void CMyDlg::DisplayIPConfInfo(const IPConfigInfo IPconf)
{
	GetDlgItem(IDC_STCIP_IPADDR)->SetWindowText(IPconf.IPAddr);
	GetDlgItem(IDC_STCIP_SUBMASK)->SetWindowText(IPconf.SubMask);
	GetDlgItem(IDC_STCIP_GATEWAY)->SetWindowText(IPconf.DefGateway);
	GetDlgItem(IDC_STCIP_DNS)->SetWindowText(IPconf.NameServer);
}

//////////////////////////////////////////////////////////////////////////////////
//创建指定名字的文件，如果该文件已经存在，则返回
///////////////////////////////////////////////////////////////////////////////////
void CMyDlg::CreateConfigFile(const CString fileName,const ADAPTER_INFO adatperinfo)
{
	CFileFind ff;
	if(ff.FindFile(fileName))
		return ;
	
	CFile file(fileName,CFile::modeCreate);
	IPConfigInfo temp;
	file.Close();

	AdatperInfoToIPConfInfo(adatperinfo,temp);
	WrtieIPconfToFile(fileName,temp);
	
	//程序第一次运行时设置为自动开机
	AutoRun();
}

void CMyDlg::OnSelchangeComboAdatper()
{
	CString FileContent,fileName,IPStr,temp;
	IPConfigInfo IPConf;
	int i=0,j=0,len;
	//刷新适配器显示列表
	DisplayAdapterInfo(AdapterInfoVector[m_adapter.GetCurSel()]);
	//刷新IP地址显示列表
	GetFileName(m_adapter.GetCurSel(),fileName);
	ReadFileToString(fileName,FileContent);

	GetIPAddrFromString(FileContent,IPStr);
	if(IPStr.GetLength()>1)
	{
		len=IPStr.GetLength();
		m_ipAddr.ResetContent();
		while(i<len)
		{
			temp.Empty();
			for(;IPStr.GetAt(i)!='#';i++)
				temp+=IPStr.GetAt(i);
			m_ipAddr.InsertString(j,temp);
			m_ipAddr.SetCurSel(j);
			
			j++;
			i++;
		}

		IPStr.Empty();
		m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),IPStr);
		GetIPConfigFromString(IPStr,FileContent,IPConf);
		DisplayIPConfInfo(IPConf);
	}
	else
	{	
		IPConfigInfo IPConf;
		m_ipAddr.ResetContent();
		DisplayIPConfInfo(IPConf);
		MessageBox("该网卡没有配置IP信息");
	}

}

void CMyDlg::OnSelchangeComboIpaddress()
{
	CString fileName,FileContent,IPStr;
	IPConfigInfo IPconf;
	
	m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),IPStr);
	GetFileName(m_adapter.GetCurSel(),fileName);
	ReadFileToString(fileName,FileContent);
	GetIPConfigFromString(IPStr,FileContent,IPconf);
	DisplayIPConfInfo(IPconf);

}

void CMyDlg::OnAddipBtn()
{
	CAddipConfDlg dlg;
	CString fileName,FileContent,IPStr,temp;
	IPConfigInfo IPconf;

	CurrentNum=m_adapter.GetCurSel();
	if(dlg.DoModal())
	{
		//重新从文件读取IP地址配置信息
		GetFileName(CurrentNum,fileName);
		if(ReadFileToString(fileName,FileContent)!=FALSE)
		{
			GetIPAddrFromString(FileContent,IPStr);

			//将IPStr中的没个IP地址分离出来，并添加进复选框         /////////////
			m_ipAddr.ResetContent();
			int j=0,len=IPStr.GetLength();
			int i=0;
			while(i<len)
			{
				temp.Empty();                                                 //有
				for(;IPStr.GetAt(i)!='#';i++)
					temp+=IPStr.GetAt(i);
				m_ipAddr.InsertString(j,temp);                               //问
				m_ipAddr.SetCurSel(j);
			
				j++;                                                         //题
				i++;
			}                                                            //////////////
														      //当添加完信息后没一个IP没添加两次到复选框
			IPStr.Empty();
			m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),IPStr);
			GetIPConfigFromString(IPStr,FileContent,IPconf);
			DisplayIPConfInfo(IPconf);
		}
		else
		{
			IPConfigInfo IPConf;
			m_ipAddr.ResetContent();
			DisplayIPConfInfo(IPConf);
		}
	}
	
}

void CMyDlg::OnDeleteipBtn()
{
	CString CurIP,fileName,FileContent,templeft,tempright;
	
	if(m_ipAddr.GetCount()==0)
	{
		MessageBox("没有可以删除的项");
		return;
	}

	int ret=MessageBox("             你确定删除这一项",NULL,MB_OKCANCEL);
	if(ret==IDCANCEL)
		return ;

	//首先把该项的信息从文件中删除
	GetFileName(m_adapter.GetCurSel(),fileName);
	m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),CurIP);
	ReadFileToString(fileName,FileContent);

	int pos=FileContent.Find(CurIP);
	while(pos&&FileContent.GetAt(pos-1)!='#')
		pos=FileContent.Find(CurIP,pos);
	templeft+=FileContent.Left(pos);
	while(FileContent.GetAt(pos)!='#')
		pos++;
	
	pos++;
	tempright+=FileContent.Right(FileContent.GetLength()-pos);

	FileContent.Empty();
	FileContent+=templeft;
	FileContent+=tempright;
	
	CFile file;

	file.Open(fileName,CFile::modeCreate|CFile::modeWrite);
	file.Write(FileContent,FileContent.GetLength());
	file.Close();
	//把该项从复选框中删除
	int index=m_ipAddr.GetCurSel();
	m_ipAddr.DeleteString(index);
	//把删除项的下一个设为当前项
	if(m_ipAddr.GetCount())
	{
		IPConfigInfo IPconf;

		m_ipAddr.SetCurSel(index%m_ipAddr.GetCount());
		//重新从文件读取IP信息
		FileContent.Empty();
		CurIP.Empty();
		m_ipAddr.GetLBText(m_ipAddr.GetCurSel(),CurIP);
		ReadFileToString(fileName,FileContent);
		GetIPConfigFromString(CurIP,FileContent,IPconf);
		DisplayIPConfInfo(IPconf);
	}
	else
	{
		IPConfigInfo IPconf;
		DisplayIPConfInfo(IPconf);
		m_ipAddr.ResetContent();
	}
	return;

}
/////////////////////////////////////////////////////////////////////////
//把文件中是数据读到contentStr中
/////////////////////////////////////////////////////////////////////////
BOOL CMyDlg::ReadFileToString(const CString fileName, CString &contentStr)
{
	CFile file;
	if(file.Open(fileName,CFile::modeRead)==0)
	{
		MessageBox("读取文件失败");
		return FALSE;
	}
	
	char content[MAXLEN];
	int len=file.Read(content,MAXLEN);
	content[len]='\0';
	contentStr.Format("%s",content);
	file.Close();
	if(contentStr.IsEmpty())
	{
		CString temp;
		temp+=fileName;
		temp+="文件为空，请点击“添加IP项”添加配置信息";
		MessageBox(temp);
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////
//得到contentStr中的所有IP地址
///////////////////////////////////////////////////////////////////
void CMyDlg::GetIPAddrFromString(const CString str, CString &IPStr)
{
	int i=0,j=0;
	int len=str.GetLength();
	while(j<len)
	{
		if(i==0||str.GetAt(i)=='#')
		{
			while(i<len&&str.GetAt(i)!='/')
			{
				IPStr+=str.GetAt(i);
				i++;
			}
		}
		for(;i<len&&str.GetAt(i)!='#';i++)
			;
		j=i;
	}
		

}

/////////////////////////////////////////////////////////////////////////////////////////////
//从contentStr中得到指定的IP地址的配置信息
//////////////////////////////////////////////////////////////////////////////////////////////
void  CMyDlg::GetIPConfigFromString(const CString IPStr,const CString str,IPConfigInfo& IPConf)
{                                                              
	int i=0,j=0,pos=str.Find(IPStr);
	int len=str.GetLength();
	
	IPConf.IPAddr=IPStr;
	for(;str.GetAt(pos)!='/';pos++);
	pos++;

	for(;str.GetAt(pos)!='/';pos++)
		IPConf.SubMask+=str.GetAt(pos);
	pos++;
	
	for(;str.GetAt(pos)!='/';pos++)
		IPConf.DefGateway+=str.GetAt(pos);
	pos++;

	for(;str.GetAt(pos)!='#';pos++)
		IPConf.NameServer+=str.GetAt(pos);
	
}


void GetFileName(int index,CString& fileName)
{
	fileName.Format("%s%d%s","Adapter",index,".config");
}

///////////////////////////////////////////////////////////////////////
//把IP的配置信息写入fileName文件
///////////////////////////////////////////////////////////////////////
void WrtieIPconfToFile(const CString fileName,const IPConfigInfo ipconf)
{
	CFile file;
	CString fileContent;

	if(file.Open(fileName,CFile::modeNoTruncate|CFile::modeWrite)==0)
	{
		AfxMessageBox("打开文件错误");
		return;
	}
	
	fileContent+=ipconf.IPAddr;
	fileContent+="/";
	fileContent+=ipconf.SubMask;
	fileContent+="/";
	fileContent+=ipconf.DefGateway;
	fileContent+="/";
	fileContent+=ipconf.NameServer;
	fileContent+="#";
	
	file.SeekToEnd();
	file.Write(fileContent,fileContent.GetLength());
	file.Close();
}

void AdatperInfoToIPConfInfo(const ADAPTER_INFO adatperinfo,IPConfigInfo& ipconf)
{
		ipconf.IPAddr=adatperinfo.strIP;
		ipconf.SubMask=adatperinfo.strNetMask;
		ipconf.DefGateway=adatperinfo.strNetGate;
		ipconf.NameServer=adatperinfo.strNameServer;
}
//////////////////////////////////////////////////////////
//如果des中有一子串和base相同，则返回true，否则返回false
//////////////////////////////////////////////////////////
BOOL stringcmp(char *base,char *des)
{
	int baselen=strlen(base);
	int deslen=strlen(des);
	char temp[9]={0};
	int i=0;
	int flag=0;
	
	memset(temp,0,strlen(temp));
	for(i=0;i<=(deslen-baselen);i++)
	{
		strncpy(temp,des+i,baselen);
		if(strcmp(base,temp)==0)
		{
			flag=1;
			break;
		}
	}

	if(flag)
		return TRUE;
	else
		return FALSE;
}


LRESULT CMyDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam != IDR_MAINFRAME) 
          return 1;
    switch(lParam) 
	{ 
	case WM_RBUTTONUP:                                   // 右键起来时弹出菜单
		{     
			POINT curPos;
			GetCursorPos(&curPos);
			m_PopMenu.LoadMenu(IDR_POPMENU);
	
			m_PopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,curPos.x,curPos.y,this);
		
			m_hMenu=m_PopMenu.Detach();
			m_PopMenu.DestroyMenu();
		} 
		break; 
    case WM_LBUTTONDBLCLK:                                 // 双击左键的处理
		{  
			this->ShowWindow(SW_SHOWNORMAL);                     // 显示主窗口

            this->SetForegroundWindow();   // 置顶显示
			
		}
		break;
	}
	return 0;
}

void CMyDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
///	int uID;
	
//	uID=MessageBox("确定要退出程序吗？",NULL,MB_OKCANCEL);
///	if(uID==IDOK)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
	    CDialog::OnCancel();
	}
}

void CMyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(nType == SIZE_MINIMIZED)
	{ 
		ShowWindow(SW_HIDE); // 当最小化时，隐藏主窗口 
	}
}

void CMyDlg::OnOK()
{
	CString IPAddrStr,SubMaskStr,DefGateWayStr,DnsStr;
	int imode=0;
	GetDlgItem(IDC_STCIP_IPADDR)->GetWindowText(IPAddrStr);
	GetDlgItem(IDC_STCIP_SUBMASK)->GetWindowText(SubMaskStr);
	GetDlgItem(IDC_STCIP_GATEWAY)->GetWindowText(DefGateWayStr);
	GetDlgItem(IDC_STCIP_DNS)->GetWindowText(DnsStr);
		
	imode=0;//dhcp  disable
	//把当前IP的信息写入注册表中
	if(RegSetIP(AdapterInfoVector[m_adapter.GetCurSel()].strName,imode,IPAddrStr,SubMaskStr,DefGateWayStr,DnsStr))
	{
		MessageBox("IP配置成功写入注册表");
	}

	//重启网卡
	int ret1=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_DISABLE);
	Sleep(500);
	int ret2=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_ENABLE);
		Sleep(500);
	if(ret1==TRUE&&ret2==TRUE)
		MessageBox("IP更改操作成功");
	else
	{
		MessageBox("IP更改操作失败");
		return ;
	}
	///更新适配器显示信息
	m_adapter.ResetContent();
	GetAdapterInfo();
	for(int i=0;i<m_AdapterNum;i++)
	{
		m_adapter.InsertString(i,AdapterInfoVector[i].strDriverDesc);
	}
	m_adapter.SetCurSel(m_InitIndex);
	m_adapter. SetDroppedWidth(290);
	//显示当前复选框中的Adapter的IP信息
	DisplayAdapterInfo(AdapterInfoVector[m_InitIndex]);


}


BOOL CMyDlg::ChangeAdapterStatus(LPCTSTR AdapterDeviceDesc, int Flag)//Flag =DICS_DISABLE-->禁用网络
{                                                                          //Flag =DICS_ENABLE--->恢复网络
	if (INVALID_HANDLE_VALUE == (hDevInfo = 
        SetupDiGetClassDevs(NULL,NULL,0,
        DIGCF_PRESENT|DIGCF_ALLCLASSES)))
    {
         printf("GetClassDevs");
    }

    DWORD i; 
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};
	unsigned char *guid[4]={0};
    char DeviceName[200]={0};
	wchar_t str[100]={0};
	
	int size=100;
	DWORD DataT;
	unsigned char *buffer=NULL;
	DWORD sz=0;

    for (i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData);i++)
    {
		//DeviceInfoData.ClassGuid="{4D36E972-E325-11CE-BFC1-08002BE10318}";
		StringFromGUID2(DeviceInfoData.ClassGuid,str,size);
		
		char temp[100]={0};
		char Rtl[]="{4D36E972-E325-11CE-BFC1-08002BE10318}";
		for(int j=0;str[j]!='\0';j++)
		{
			temp[j]=(char)str[j];
		}

		if(strcmp(temp,Rtl)==0)
		{
			LPTSTR buffer=NULL ;
			DWORD buffersize=0 ;

			while(!SetupDiGetDeviceRegistryProperty(hDevInfo,&DeviceInfoData,\
			SPDRP_DEVICEDESC,&DataT,	(PBYTE)buffer,buffersize,&buffersize))
			{
				if(GetLastError()==ERROR_INVALID_DATA)
				{
					break ;
				}
				else if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
				{
					if(buffer)
						LocalFree(buffer);
					buffer=(LPTSTR)LocalAlloc(LPTR,buffersize);
				}
				else 
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
					return FALSE ;
				}
			}
			if(GetLastError()==ERROR_INVALID_DATA)
				continue ;

			if(strcmp(buffer,AdapterDeviceDesc)==0)
			{//
				if (StateChange(Flag,i,hDevInfo))
				//	if(EnableNetDevice(Flag, i))
				{
					break;
				}
				else
				{
					CString temp="对";
					temp+=AdapterDeviceDesc;
					temp+="的操作失败！";
					AfxMessageBox(temp);
					return FALSE;
				}
			}
		}
    }
	return TRUE;
}

BOOL StateChange(DWORD NewState, DWORD SelectedItem,HDEVINFO hDevInfo)
{
    LPTSTR buffer=NULL ;
    DWORD buffersize=0 ;
    SP_PROPCHANGE_PARAMS PropChangeParams = {sizeof(SP_CLASSINSTALL_HEADER)};
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};
    HCURSOR hCursor;

    //
    // This may take a while :^(
    //
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

    //
    // Get a handle to the Selected Item.
    //
    if (!SetupDiEnumDeviceInfo(hDevInfo,SelectedItem,&DeviceInfoData))
    {
        printf("EnumDeviceInfo");
        return FALSE;
    }
	
    //{4D36E972-E325-11CE-BFC1-08002BE10318}
    // Set the PropChangeParams structure.
    //
    PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    PropChangeParams.Scope = DICS_FLAG_GLOBAL;
    PropChangeParams.StateChange = NewState; 

    if (!SetupDiSetClassInstallParams(hDevInfo,
        &DeviceInfoData,
        (SP_CLASSINSTALL_HEADER *)&PropChangeParams,
        sizeof(PropChangeParams)))
    {
        printf("SetClassInstallParams");
        SetCursor(hCursor);
        return FALSE;
    }

    //
    // Call the ClassInstaller and perform the change.
    //
    if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,
        hDevInfo,
        &DeviceInfoData))
    {
        printf("SetClassInstallParams");
        SetCursor(hCursor);
        return TRUE;
    }
    return TRUE;
}

void CMyDlg::OnDisableBtn() 
{
	// TODO: Add your control notification handler code here
	int ret=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_DISABLE);
	if(ret==FALSE)
	{
		printf("禁用网卡失败");
			MessageBox("禁用网卡失败");
	}
	
	else
		MessageBox("禁用网卡成功！");
}

void CMyDlg::OnEnableBtn() 
{
	// TODO: Add your control notification handler code here
	int ret=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_ENABLE);
	if(ret==FALSE)
		MessageBox("启用网卡失败");
	else
		MessageBox("启用网卡成功");
}

void CMyDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	POINT curPos;
	GetCursorPos(&curPos);
	m_PopMenu.LoadMenu(IDR_POPMENU);
	
	m_PopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,curPos.x,curPos.y,this);
	
	m_hMenu=m_PopMenu.Detach();
	m_PopMenu.DestroyMenu();
	
}


//////////////
void CMyDlg::OnAutoRun()
{
	MENUITEMINFO info; 
    ZeroMemory(&info,sizeof(MENUITEMINFO)); 
    info.cbSize = sizeof(MENUITEMINFO); 
    info.fMask  = MIIM_STATE;  
    info.cch = 254;

	GetMenuItemInfo(m_hMenu,IDM_AUTORUN,FALSE,&info);
	UINT ret;
	ret=~info.fState&MF_CHECKED;
	if(ret)
		AutoRun();
	else
		UnAutoRun();
}

void CMyDlg::OnAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
	
}
void CMyDlg::OnExit()
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	CDialog::OnCancel();
}

BOOL CMyDlg::AutoRun()
{
	TCHAR m_fileName[100];
	GetModuleFileName(NULL,m_fileName,100);
	HKEY hKey;
	CString str=_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	if(ERROR_SUCCESS!=RegCreateKey(HKEY_LOCAL_MACHINE,str,&hKey))
	{
		MessageBox(_T("打开注册表失败"));
		RegCloseKey(hKey);
		return FALSE;
	}
	int length=0;
	while(m_fileName[length]!=_T('\0'))
		length++;
    if(ERROR_SUCCESS!=RegSetValueEx(hKey,_T("网络配置"),0,REG_SZ,(const BYTE *)m_fileName,sizeof(TCHAR)*length))
	{
		MessageBox(_T("打开注册表失败"));
		RegCloseKey(hKey);
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;

}

BOOL CMyDlg::UnAutoRun()
{
	CString str = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hKey;
	if((RegOpenKeyEx(HKEY_LOCAL_MACHINE,str,0,KEY_WRITE,&hKey))==ERROR_SUCCESS)
	{
		if(RegDeleteValue(hKey,_T("网络配置"))!=ERROR_SUCCESS)
		{
			MessageBox(_T("删除注册表失败"));
			return FALSE;
		}
		RegCloseKey(hKey);
	}
	return FALSE;

}

BOOL CMyDlg::IsAutoRun()
{
	HKEY hKey;
	CString strKeyName = "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				strKeyName,
				0,
				KEY_READ,
				&hKey) != ERROR_SUCCESS)
		return FALSE;

	unsigned char szData[256];
	DWORD dwDataType, dwBufSize;

	dwBufSize = 256;
	if(RegQueryValueEx(hKey, "网络配置", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
		return TRUE;
	return FALSE;

}

void CMyDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
     ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
         state.m_pParentMenu = pPopupMenu;//???  Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
		CWnd* pParent = this;
        // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				 if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				 {
                // When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			 continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;    // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);//??  Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
        {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
            while (state.m_nIndex < nCount &&
                  pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
            }
		}
        state.m_nIndexMax = nCount;
   }
}

void CMyDlg::OnUpdateAutorun(CCmdUI* pCmdUI)
{
	m_IsAutoRun=IsAutoRun();
	if(m_IsAutoRun)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMyDlg::OnBUTTONAUTOooo() 
{
	// TODO: Add your control notification handler code here
		CString IPAddrStr,SubMaskStr,DefGateWayStr,DnsStr;
	int imode=0;
	GetDlgItem(IDC_STCIP_IPADDR)->GetWindowText(IPAddrStr);
	GetDlgItem(IDC_STCIP_SUBMASK)->GetWindowText(SubMaskStr);
	GetDlgItem(IDC_STCIP_GATEWAY)->GetWindowText(DefGateWayStr);
	GetDlgItem(IDC_STCIP_DNS)->GetWindowText(DnsStr);
		
	imode=1;//dhcp,自动获取IP地址
	//把当前IP的信息写入注册表中
	IPAddrStr="";
	SubMaskStr="";
	DefGateWayStr="";
	DnsStr="";
	if(RegSetIP(AdapterInfoVector[m_adapter.GetCurSel()].strName,imode,IPAddrStr,SubMaskStr,DefGateWayStr,DnsStr))
	{
		MessageBox("IP配置成功写入注册表");
	}

	//重启网卡
	int ret1=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_DISABLE);
	int ret2=ChangeAdapterStatus(AdapterInfoVector[m_adapter.GetCurSel()].strDriverDesc,DICS_ENABLE);
	if(ret1==TRUE&&ret2==TRUE)
		MessageBox("IP更改操作成功");
	else
	{
		MessageBox("IP更改操作失败");
		return ;
	}
	///更新适配器显示信息
	m_adapter.ResetContent();
	GetAdapterInfo();
	for(int i=0;i<m_AdapterNum;i++)
	{
		m_adapter.InsertString(i,AdapterInfoVector[i].strDriverDesc);
	}
	m_adapter.SetCurSel(m_InitIndex);
	m_adapter. SetDroppedWidth(290);
	//显示当前复选框中的Adapter的IP信息
	DisplayAdapterInfo(AdapterInfoVector[m_InitIndex]);
}
