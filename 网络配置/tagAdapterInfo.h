//显示结构体
typedef struct _displayinfo
{
	CString IPAddr;
	CString SubMask;
	CString DefGateway;
	CString NameServer;
}IPConfigInfo ;


//适配器结构体
typedef struct tagAdapterInfo
{
	CString strName;			// 适配器名称
	CString strDriverDesc;	// 适配器描述
	CString strIP;			// IP地址
	CString strNetMask;		// 子网掩码
	CString strNetGate;		// 网关
	CString strNameServer;  //域名服务器

}ADAPTER_INFO;

