//��ʾ�ṹ��
typedef struct _displayinfo
{
	CString IPAddr;
	CString SubMask;
	CString DefGateway;
	CString NameServer;
}IPConfigInfo ;


//�������ṹ��
typedef struct tagAdapterInfo
{
	CString strName;			// ����������
	CString strDriverDesc;	// ����������
	CString strIP;			// IP��ַ
	CString strNetMask;		// ��������
	CString strNetGate;		// ����
	CString strNameServer;  //����������

}ADAPTER_INFO;

