/*

  myopc.cpp

  */
#include "stdafx.h"
#include "opcdrv.h"
#include "myopc.h"

// Versions for serializing:
#define VERSION_1					1			// serialized verions
#define CURRENT_VERSION				VERSION_1	// current version for saving

// m_arrMultiQI indices:
#define MQI_IOPCSERVER		0
#define MQI_IOPCCOMMON		1
#define MQI_IOPCCONNPT		2
#define MQI_IOPCITEMPROP	3
#define MQI_IOPCBROWSE		4
#define MQI_IOPCPUBLIC		5
#define MQI_IOPCPERSIST		6

// Clipboard formats for data access 1.0 streams:
UINT CF_DATA_CHANGE = RegisterClipboardFormat (_T("OPCSTMFORMATDATA"));
UINT CF_DATA_CHANGE_TIME	= RegisterClipboardFormat (_T("OPCSTMFORMATDATATIME"));
UINT CF_WRITE_COMPLETE = RegisterClipboardFormat (_T("OPCSTMFORMATWRITECOMPLETE"));


void MytoWchar(WCHAR* lpwcd,LPCTSTR lpszTxt) //多字节字符转换为BSTR
{
	CString szt = lpszTxt;
	BSTR bstr = szt.AllocSysString();
	wcscpy( lpwcd, bstr );
	SysFreeString(bstr);
}

// **************************************************************************
// String container object for items
// **************************************************************************
class CStringContainer
	{
	public:
		// Constructor:
		CStringContainer ()
		{
			m_bInitialized = false;
		}

		// Initialize container object.
		void Initialize ()
		{
			// Load string resources and assign public pointers if we haven't
			// already done so:
			if (!m_bInitialized)
			{
				// Load quality string resources:
				// Set initialized flag so we don't waste time loading string 
				// resources if called again.
				m_bInitialized = true;
			}
		}

	private:
		bool m_bInitialized;
	
	} cStringContainer;

/////////////////////////////////////////////////////////////////////////////
// CKServer construction/destruction
/////////////////////////////////////////////////////////////////////////////

CKServer::CKServer ()
{
	m_pdrv = NULL;
	// Initialize members:
	m_strProgID = _T("");
	m_strRemoteMachine.Empty ();

	ZeroMemory (&m_bfFlags, sizeof (m_bfFlags));

	// Fill our multiple query interface structure with all of the
	// interfaces we can possible request at the server level:
	for (int i = 0; i < sizeof (m_arrMultiQI) / sizeof (MULTI_QI); i++)
		{
		m_arrMultiQI [i].pItf = NULL;
		m_arrMultiQI [i].hr = 0;
		}
	
	// Interface pointers we will query and maintain:
	m_pIServer				= NULL;
	m_pICommon				= NULL;
	m_pIConnPtContainer		= NULL;
	m_pIItemProps			= NULL;
	m_pIBrowse				= NULL;
	m_pIPublicGroups		= NULL;
	m_pIPersistFile			= NULL;	

	m_pIShutdownSink		= NULL;
	m_dwCookieShutdownSink	= 0;
	
	m_bConnected = false;
}

CKServer::CKServer (CString &strProgID, CString &strRemoteMachine)
	{
	ASSERT (!strProgID.IsEmpty ());

	m_pdrv = NULL;

	// Initialize members:
	m_strProgID = strProgID;
	m_strRemoteMachine = strRemoteMachine;

	ZeroMemory (&m_bfFlags, sizeof (m_bfFlags));

	// Fill our multiple query interface structure with all of the
	// interfaces we can possible request at the server level:
	for (int i = 0; i < sizeof (m_arrMultiQI) / sizeof (MULTI_QI); i++)
		{
		m_arrMultiQI [i].pItf = NULL;
		m_arrMultiQI [i].hr = 0;
		}
	
	// Interface pointer we will query and maintain:
	m_pIServer				= NULL;
	m_pICommon				= NULL;
	m_pIConnPtContainer		= NULL;
	m_pIItemProps			= NULL;
	m_pIBrowse				= NULL;
	m_pIPublicGroups		= NULL;
	m_pIPersistFile			= NULL;	

	m_pIShutdownSink		= NULL;
	m_dwCookieShutdownSink	= 0;
	
	m_bConnected = false;
	}


CKServer::~CKServer ()
	{
	// Remove all groups:

	// Disconnect from the OPC Sserver we are connected to:
	Disconnect ();

	// Assert that all interfaces and groups have been properly released:
	ASSERT (m_pIServer == NULL);
	ASSERT (m_pICommon == NULL);
	ASSERT (m_pIConnPtContainer == NULL);
	ASSERT (m_pIItemProps == NULL);
	ASSERT (m_pIBrowse == NULL);
	ASSERT (m_pIPublicGroups == NULL);
	ASSERT (m_pIPersistFile == NULL);
	ASSERT (m_pIShutdownSink == NULL);
	ASSERT (m_dwCookieShutdownSink == 0);
	}

/////////////////////////////////////////////////////////////////////////////
// CKServer manipulators
/////////////////////////////////////////////////////////////////////////////

void CKServer::SetProgID (CString &strProgID)
{
	ASSERT (!strProgID.IsEmpty ());

	m_strProgID = strProgID;
}

void CKServer::SetRemoteMachine (CString &strRemoteMachine)//本机为空串
{
	m_strRemoteMachine = strRemoteMachine;
}

void CKServer::Start ()//只是连接
{
	// Attempt to connect to the OPC Server:
	Connect ();
}

void CKServer::Stop ()//只是断开连接
{
	Disconnect ();
}


/////////////////////////////////////////////////////////////////////////////
// CKServer OPC Specifics
/////////////////////////////////////////////////////////////////////////////

HRESULT CKServer::GetCLSID (CLSID &clsid)//取 opc server ClassID
{
	HRESULT hr;
	WCHAR wszProgID [DEFBUFFSIZE];
	MytoWchar(wszProgID,m_strProgID); //多字节字符转换为BSTR
	hr = CLSIDFromProgID (wszProgID, &clsid);
	if(SUCCEEDED(hr))
		return hr;
	
	//再判断是否是clsid字符串
	hr = CLSIDFromString(wszProgID,&clsid);
	return hr;
}

// **************************************************************************
// IsAlive ()
// Description:
//检查是否连接到Server
// Returns:
//  bool - true if connection is live.
// **************************************************************************
bool CKServer::IsAlive () 
{
	bool bAlive = false;

	// If we don't think we are connected, i.e. m_bConnected or m_pIServer is
	// NULL, then there is no need to test connection.
	if (IsConnected() && m_pIServer)
	{
		//用GetStatus测试连接
		try
		{
			OPCSERVERSTATUS *pStatus;

			if (SUCCEEDED (m_pIServer->GetStatus (&pStatus)))
			{
				if (pStatus->szVendorInfo)
					CoTaskMemFree (pStatus->szVendorInfo);
				CoTaskMemFree (pStatus);
				bAlive = true;
			}
		}
		catch (...)
		{
			ASSERT (FALSE);
			bAlive = false;
		}
	}
	return (bAlive);
}

// **************************************************************************
// Connect ()
// Description:
//	Connect to OPC Server.  OPC Server's ProgID and machine	name must have
//  been previously specified.
// Returns:
//  bool - true if success.
// **************************************************************************
BOOL GetOPCServerClsID(LPCTSTR lpszIP,LPCTSTR lpszProgID,CLSID *pclsid);
bool CKServer::Connect ()
	{
	CSafeLock lock(&m_csLock);
	// Program ID of OPC Server should have been defined by now:
	ASSERT (!m_strProgID.IsEmpty ());

	// Assume we are not connecting to KEPServerEx:
	m_bfFlags.bIsKepServerEx = false;

	Disconnect ();//断开上次连接

	//首先获取OPC程序的CLASSID
	CLSID clsid;
	if(FAILED(GetCLSID (clsid)))
	{
		if (m_strRemoteMachine.IsEmpty ())//本机
		{
			TRACE("Class ID error!\n");
			return false;
		}
		//尝试从服务器获取
		if(!GetOPCServerClsID(m_strRemoteMachine,m_strProgID,&clsid))
		{
			TRACE("Class ID error!\n");
			return false;
		}
	}
	
	HRESULT hr;

	// Re-intialize Multi-Query Interface:
	//先初始化数组每项内容为空
	for (int i = 0; i < sizeof (m_arrMultiQI) / sizeof (MULTI_QI); i++)
	{
		m_arrMultiQI [i].pItf = NULL;
		m_arrMultiQI [i].hr = 0;
	}

	// Load up the Interface ID's we hope to get pointers for when we
	// call CoCreateInstanceEx():
	//填写每个接口的ID
	m_arrMultiQI [MQI_IOPCSERVER].pIID		= &IID_IOPCServer;
	m_arrMultiQI [MQI_IOPCCOMMON].pIID		= &IID_IOPCCommon;
	m_arrMultiQI [MQI_IOPCCONNPT].pIID		= &IID_IConnectionPointContainer;
	m_arrMultiQI [MQI_IOPCITEMPROP].pIID	= &IID_IOPCItemProperties;
	m_arrMultiQI [MQI_IOPCBROWSE].pIID		= &IID_IOPCBrowseServerAddressSpace;
	m_arrMultiQI [MQI_IOPCPUBLIC].pIID		= &IID_IOPCServerPublicGroups;
	m_arrMultiQI [MQI_IOPCPERSIST].pIID		= &IID_IPersistFile;

	// Connect to the OPC Server and query all possible interfaces:
	//连接OPC服务器，查询所有可能的接口
	if (m_strRemoteMachine.IsEmpty ())		//本机，注意本机和远程计算机的CoCreateInstanceEx使用方法
	{
		//hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
		//	RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);

		hr = CoCreateInstanceEx (
			clsid,										// CLSID，OPC服务器的CLASSID
			NULL,										// No aggregation
			CLSCTX_LOCAL_SERVER /*CLSCTX_SERVER*/,								// connect to local, inproc and remote servers
			NULL,										// remote machine name 
			sizeof (m_arrMultiQI) / sizeof (MULTI_QI),	// number of IIDS to query		
			m_arrMultiQI);								// array of IID pointers to query
		
	}	//一次查询所有接口

	else	//远程计算机
	{
		COSERVERINFO tCoServerInfo;		//该结构只使用了pwszName成员

		ZeroMemory (&tCoServerInfo, sizeof (tCoServerInfo)); //情空服务器计算机信息结构

		// 分配计算机名空间,注意OPC都要使用UNICODE
		int nSize = m_strRemoteMachine.GetLength () * sizeof (WCHAR);
		tCoServerInfo.pwszName = new WCHAR [nSize];

		if (tCoServerInfo.pwszName == NULL)//判断计算机名UNICODE空间NEW是否成功
		{
			ASSERT (FALSE);
			return (false);
		}

		MytoWchar(tCoServerInfo.pwszName,m_strRemoteMachine);//转换为BSTR

		hr = CoCreateInstanceEx (
			clsid,										// CLSID
			NULL,										// No aggregation
			CLSCTX_SERVER,								// connect to local, inproc and remote servers
			&tCoServerInfo,								// remote machine name 
			sizeof (m_arrMultiQI) / sizeof (MULTI_QI),	// number of IIDS to query		
			m_arrMultiQI);								// array of IID pointers to query
		
		delete [] tCoServerInfo.pwszName; //释放计算机名称UNICODE占用内存
	}

	// 0x00080012表示部分接口不支持，IOPCServerPublicGroups、IPersistFile和IOPCBrowseServerAddressSpace为可选接口，有些OPC服务器并不实现

	if (SUCCEEDED (hr) || (hr == 0x00080012))//成功后，逐个检查每个接口状态，并将接口保存到CKServer的接口指针成员变量中,有些opc服务器不支持某些接口，这句可去掉
	{
		TRACE (_T("OTC: Initializing server %s interfaces.\r\n"), GetProgID ());

		// Check IOPCServer interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCSERVER].hr))//检查hr成员即可
		{
			m_pIServer = (IOPCServer *)m_arrMultiQI [MQI_IOPCSERVER].pItf;

			if (m_pIServer == NULL)
			{
				((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCServer接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
				return false;
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCSERVER].pItf != NULL)
			{
				// Warning failure but pointer not set to null
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCServer接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IOPCServer (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCSERVER].hr); 

			return false;
		}

		// Check IOPCCommon interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCCOMMON].hr))
		{
			m_pICommon = (IOPCCommon *)m_arrMultiQI [MQI_IOPCCOMMON].pItf;

			if (m_pICommon == NULL)
			{
				((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCCommon接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
				return false;
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCCOMMON].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCCommon接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IOPCCommon (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCCOMMON].hr); 
			return false;
		}

		// Check IConnectionPointContainer interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCCONNPT].hr))
		{
			m_pIConnPtContainer = 
				(IConnectionPointContainer *)m_arrMultiQI [MQI_IOPCCONNPT].pItf;

			if (m_pIConnPtContainer == NULL)
			{
				((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IConnectionPointContainer接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
				return false;
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCCONNPT].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IConnectionPointContainer接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IConnectionPoint (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCCONNPT].hr); 
			return false;
		}

		// Check IOPCItemProperties interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCITEMPROP].hr))
		{
			m_pIItemProps = 
				(IOPCItemProperties *)m_arrMultiQI [MQI_IOPCITEMPROP].pItf;

			if (m_pIItemProps == NULL)
			{
				((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCItemProperties接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
				return false;
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCITEMPROP].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddErrorFmt(_T("查询IOPCItemProperties接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IOPCItemProperties (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCITEMPROP].hr); 
			return false;				
		}

		// Check IOPCBrowseServerAddressSpace interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCBROWSE].hr))
		{
			m_pIBrowse = 
				(IOPCBrowseServerAddressSpace *)m_arrMultiQI [MQI_IOPCBROWSE].pItf;

			if (m_pIBrowse == NULL)
			{
				((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IOPCBrowseServerAddressSpace接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCBROWSE].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IOPCBrowseServerAddressSpace接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IOPCBrowseServerAddressSpace (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCBROWSE].hr); 				
		}

		// Check IOPCServerPublicGroups interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCPUBLIC].hr))
		{
			m_pIPublicGroups = 
				(IOPCServerPublicGroups *)m_arrMultiQI [MQI_IOPCPUBLIC].pItf;

			if (m_pIPublicGroups == NULL)
			{
				((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IOPCServerPublicGroups接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCPUBLIC].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IOPCServerPublicGroups接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IOPCServerPublicGroups (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCPUBLIC].hr); 				
		}

		// Check IPersistFile interface pointer:
		if (SUCCEEDED (m_arrMultiQI [MQI_IOPCPERSIST].hr))
		{
			m_pIPersistFile = 
				(IPersistFile *)m_arrMultiQI [MQI_IOPCPERSIST].pItf;

			if (m_pIPersistFile == NULL)
			{
				((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IPersistFile接口成功，但返回指针无效！"));
				// Warning success but no valid pointer:
				ASSERT (FALSE);
			}
		}
		else
		{
			if (m_arrMultiQI [MQI_IOPCPERSIST].pItf != NULL)
			{
				// Warning failure but pointer not set to null:
				ASSERT (FALSE);
			}

			((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IPersistFile接口失败，可能OPC服务器不支持该接口！"));
			TRACE (_T("OTC: Failed to query IPersistsFile (%08X).\r\n"), 
				m_arrMultiQI [MQI_IOPCPERSIST].hr); 				
		}

		// Check IConnectionPointContainer interface pointer:
		if (m_pIConnPtContainer != NULL)
		{
			// If the server supports the shutdown interface, provide a sink 
			// to the server.

			// Get connection point pointer:
			IConnectionPoint *pCP = NULL;
			hr = m_pIConnPtContainer->FindConnectionPoint (IID_IOPCShutdown, &pCP);

			// If we got the connection point, instantiate our shutdown sink:
			if (SUCCEEDED (hr))
			{
				try
				{
					// Instantiate the shutdown sink and add us to its reference count:
					m_pIShutdownSink = new IKShutdownSink (this);
					m_pIShutdownSink->AddRef ();
											
					// Give the connection point a pointer to our shutdown sink:
					// (m_dwCookieShutdownSink is a returned token that uniquely
					// identifies this connection.)
					hr = pCP->Advise (m_pIShutdownSink, &m_dwCookieShutdownSink);

					// We are done with the connection point, so release our reference:
					pCP->Release ();
				}
				
				catch (...)
				{
					// If we find ourselves here, either "new" failed or pCP is bad.
					ASSERT (FALSE);
					hr = E_FAIL;
				}
			}
			else {
				((COpcDrv*)m_pdrv)->AddDebugFmt(_T("查询IOPCShutdown接口失败，可能OPC服务器不支持该接口！"));
			}
		}

		m_bConnected = (m_pIServer != NULL);
	}
	else
	{
		TRACE("CoCreateInstanceEx!\n");
	}

	return (m_bConnected);
}
/*
bool CKServer::Connect ()
	{
	// Program ID of OPC Server should have been defined by now:
	ASSERT (!m_strProgID.IsEmpty ());

	// Assume we are not connecting to KEPServerEx:
	m_bfFlags.bIsKepServerEx = false;

	Disconnect ();//断开上次连接

	//首先获取OPC程序的CLASSID
	CLSID clsid;
	if (SUCCEEDED (GetCLSID (clsid)))
	{
		HRESULT hr;

		// Re-intialize Multi-Query Interface:
		//先初始化数组每项内容为空
		for (int i = 0; i < sizeof (m_arrMultiQI) / sizeof (MULTI_QI); i++)
		{
			m_arrMultiQI [i].pItf = NULL;
			m_arrMultiQI [i].hr = 0;
		}

		// Load up the Interface ID's we hope to get pointers for when we
		// call CoCreateInstanceEx():
		//填写每个接口的ID
		m_arrMultiQI [MQI_IOPCSERVER].pIID		= &IID_IOPCServer;
		m_arrMultiQI [MQI_IOPCCOMMON].pIID		= &IID_IOPCCommon;
		m_arrMultiQI [MQI_IOPCCONNPT].pIID		= &IID_IConnectionPointContainer;
		m_arrMultiQI [MQI_IOPCITEMPROP].pIID	= &IID_IOPCItemProperties;
		m_arrMultiQI [MQI_IOPCBROWSE].pIID		= &IID_IOPCBrowseServerAddressSpace;
		m_arrMultiQI [MQI_IOPCPUBLIC].pIID		= &IID_IOPCServerPublicGroups;
		m_arrMultiQI [MQI_IOPCPERSIST].pIID		= &IID_IPersistFile;

		// Connect to the OPC Server and query all possible interfaces:
		//连接OPC服务器，查询所有可能的接口
		if (m_strRemoteMachine.IsEmpty ())		//本机，注意本机和远程计算机的CoCreateInstanceEx使用方法
		{
			hr = CoCreateInstanceEx (
				clsid,										// CLSID，OPC服务器的CLASSID
				NULL,										// No aggregation
				CLSCTX_SERVER,								// connect to local, inproc and remote servers
				NULL,										// remote machine name 
				sizeof (m_arrMultiQI) / sizeof (MULTI_QI),	// number of IIDS to query		
				m_arrMultiQI);								// array of IID pointers to query
		}	//一次查询所有接口

		else	//远程计算机
		{
			COSERVERINFO tCoServerInfo;		//该结构只使用了pwszName成员

			ZeroMemory (&tCoServerInfo, sizeof (tCoServerInfo)); //情空服务器计算机信息结构

			// 分配计算机名空间,注意OPC都要使用UNICODE
			int nSize = m_strRemoteMachine.GetLength () * sizeof (WCHAR);
			tCoServerInfo.pwszName = new WCHAR [nSize];

			if (tCoServerInfo.pwszName == NULL)//判断计算机名UNICODE空间NEW是否成功
			{
				ASSERT (FALSE);
				return (false);
			}

			MytoWchar(tCoServerInfo.pwszName,m_strRemoteMachine);//转换为BSTR

			hr = CoCreateInstanceEx (
				clsid,										// CLSID
				NULL,										// No aggregation
				CLSCTX_SERVER,								// connect to local, inproc and remote servers
				&tCoServerInfo,								// remote machine name 
				sizeof (m_arrMultiQI) / sizeof (MULTI_QI),	// number of IIDS to query		
				m_arrMultiQI);								// array of IID pointers to query
			
			delete [] tCoServerInfo.pwszName; //释放计算机名称UNICODE占用内存
		}

		if (SUCCEEDED (hr))//成功后，逐个检查每个接口状态，并将接口保存到CKServer的接口指针成员变量中
		{
			TRACE (_T("OTC: Initializing server %s interfaces.\r\n"), GetProgID ());

			// Check IOPCServer interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCSERVER].hr))//检查hr成员即可
			{
				m_pIServer = (IOPCServer *)m_arrMultiQI [MQI_IOPCSERVER].pItf;

				if (m_pIServer == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCSERVER].pItf != NULL)
				{
					// Warning failure but pointer not set to null
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IOPCServer (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCSERVER].hr); 
			}

			// Check IOPCCommon interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCCOMMON].hr))
			{
				m_pICommon = (IOPCCommon *)m_arrMultiQI [MQI_IOPCCOMMON].pItf;

				if (m_pICommon == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCCOMMON].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IOPCCommon (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCCOMMON].hr); 
			}

			// Check IConnectionPointContainer interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCCONNPT].hr))
			{
				m_pIConnPtContainer = 
					(IConnectionPointContainer *)m_arrMultiQI [MQI_IOPCCONNPT].pItf;

				if (m_pIConnPtContainer == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCCONNPT].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IConnectionPoint (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCCONNPT].hr); 
			}

			// Check IOPCItemProperties interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCITEMPROP].hr))
			{
				m_pIItemProps = 
					(IOPCItemProperties *)m_arrMultiQI [MQI_IOPCITEMPROP].pItf;

				if (m_pIItemProps == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCITEMPROP].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IOPCItemProperties (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCITEMPROP].hr); 				
			}

			// Check IOPCBrowseServerAddressSpace interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCBROWSE].hr))
			{
				m_pIBrowse = 
					(IOPCBrowseServerAddressSpace *)m_arrMultiQI [MQI_IOPCBROWSE].pItf;

				if (m_pIBrowse == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCBROWSE].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IOPCBrowseServerAddressSpace (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCBROWSE].hr); 				
			}

			// Check IOPCServerPublicGroups interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCPUBLIC].hr))
			{
				m_pIPublicGroups = 
					(IOPCServerPublicGroups *)m_arrMultiQI [MQI_IOPCPUBLIC].pItf;

				if (m_pIPublicGroups == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCPUBLIC].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IOPCServerPublicGroups (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCPUBLIC].hr); 				
			}

			// Check IPersistFile interface pointer:
			if (SUCCEEDED (m_arrMultiQI [MQI_IOPCPERSIST].hr))
			{
				m_pIPersistFile = 
					(IPersistFile *)m_arrMultiQI [MQI_IOPCPERSIST].pItf;

				if (m_pIPersistFile == NULL)
				{
					// Warning success but no valid pointer:
					ASSERT (FALSE);
				}
			}
			else
			{
				if (m_arrMultiQI [MQI_IOPCPERSIST].pItf != NULL)
				{
					// Warning failure but pointer not set to null:
					ASSERT (FALSE);
				}

				TRACE (_T("OTC: Failed to query IPersistsFile (%08X).\r\n"), 
					m_arrMultiQI [MQI_IOPCPERSIST].hr); 				
			}

			// Check IConnectionPointContainer interface pointer:
			if (m_pIConnPtContainer != NULL)
			{
				// If the server supports the shutdown interface, provide a sink 
				// to the server.

				// Get connection point pointer:
				IConnectionPoint *pCP = NULL;
				hr = m_pIConnPtContainer->FindConnectionPoint (IID_IOPCShutdown, &pCP);

				// If we got the connection point, instantiate our shutdown sink:
				if (SUCCEEDED (hr))
				{
					try
					{
						// Instantiate the shutdown sink and add us to its reference count:
						m_pIShutdownSink = new IKShutdownSink (this);
						m_pIShutdownSink->AddRef ();
												
						// Give the connection point a pointer to our shutdown sink:
						// (m_dwCookieShutdownSink is a returned token that uniquely
						// identifies this connection.)
						hr = pCP->Advise (m_pIShutdownSink, &m_dwCookieShutdownSink);

						// We are done with the connection point, so release our reference:
						pCP->Release ();
					}
					
					catch (...)
					{
						// If we find ourselves here, either "new" failed or pCP is bad.
						ASSERT (FALSE);
						hr = E_FAIL;
					}
				}
			}

			m_bConnected = (m_pIServer != NULL);
		}
		else
		{
			TRACE("CoCreateInstanceEx!\n");
		}
	}

	else
	{
		TRACE("Class ID error!\n");
	}

	return (m_bConnected);
}
*/
// **************************************************************************
// Connect () 连接
//
// Description:
//	Attempts to connect to the OPC Server with specified ProgID and machine name.
//
// Parameters:
//  CString		&strProgID			ProgID of OPC server.
//	CString		&strRemoteMachine	Machine name OPC server is on.
//
// Returns:
//  bool - true if success.
// **************************************************************************
bool CKServer::Connect (CString &strProgID, CString &strRemoteMachine)
{
	ASSERT (m_bConnected == FALSE);

	SetProgID (strProgID);
	SetRemoteMachine (strRemoteMachine);

	return (Connect ());
}

// **************************************************************************
// Disconnect () 断开连接
//
// Description:
//	Called to disconnect from OPC server.
//	断开连接，除m_pIConnPtContainer接口外，其他直接Release
//
//
// Parameters:
//  none
//
// Returns:
//  void
// **************************************************************************
void CKServer::Disconnect ()
{
	CSafeLock lock(&m_csLock);
	
	TRACE(_T("CKServer::Disconnect () - 0x%02X\n"), (DWORD)this);

	m_bConnected = false;

	// Release 所有接口
	if (m_pIServer)
	{
		m_pIServer->Release ();
		m_pIServer = NULL;
	}

	if (m_pICommon)
	{
		m_pICommon->Release ();
		m_pICommon = NULL;
	}

	if (m_pIConnPtContainer)
	{
		// Unadvise shutdown notifications:
		if (m_dwCookieShutdownSink != 0)
		{
			HRESULT hr = E_FAIL;
			IConnectionPoint *pCP = NULL;

			hr = m_pIConnPtContainer->FindConnectionPoint (IID_IOPCShutdown, &pCP);

			if (SUCCEEDED (hr))
			{
				hr = pCP->Unadvise (m_dwCookieShutdownSink);
				pCP->Release ();
			}

			if (FAILED (hr))
			{
				TRACE (_T("OTC: CKServer::Disconnect () - failed to unadvise shutdown notifications\r\n"));
			}

			m_dwCookieShutdownSink = 0;
		}

		if (m_pIShutdownSink != NULL)
		{
			m_pIShutdownSink->Release ();
			m_pIShutdownSink = NULL;
		}

		m_pIConnPtContainer->Release ();
		m_pIConnPtContainer = NULL;
	}

	if (m_pIItemProps)
	{
		m_pIItemProps->Release ();
		m_pIItemProps = NULL;
	}

	if (m_pIBrowse)
	{
		m_pIBrowse->Release ();
		m_pIBrowse = NULL;
	}

	if (m_pIPublicGroups)
	{
		m_pIPublicGroups->Release ();
		m_pIPublicGroups = NULL;
	}

	if (m_pIPersistFile)
	{
		m_pIPersistFile->Release ();
		m_pIPersistFile = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CKServer group management 组管理
/////////////////////////////////////////////////////////////////////////////

// AddGroup () 添加组
BOOL CKServer::AddGroup (CKGroup *pGroup,CString &szErr)
{
	CSafeLock lock(&m_csLock);

	ASSERT (pGroup != NULL);
	szErr="";
	
	if(!m_bConnected)
	{
		szErr.Format("OPC Server<%s>没有联接",m_strProgID);
		return FALSE;
	}

	// Initialize arguments for add group request:
	HRESULT hr				= E_FAIL;
	WCHAR *pszName			= NULL;
	long lBias				= pGroup->GetBias ();
	float fDeadband			= pGroup->GetDeadband ();
	LPCTSTR lpszName		= pGroup->GetName ();
	DWORD dwRevUpdateRate	= 0;
	OPCHANDLE hServer		= NULL;
	IUnknown *pUnknown		= NULL;

	
	WCHAR wchBuffer	[DEFBUFFSIZE];

	if (lpszName != NULL)
	{

		MytoWchar(wchBuffer,lpszName);//jy200807w
		pszName = wchBuffer;
	}
		
	// Issue add OPC Group request using IOPCServer interface.  Pointer to 
	// the IOPCServer interface, m_pIServer, should have been set in Connect().
	hr = m_pIServer->AddGroup ( //加入组
			pszName,					// [in] group name
			pGroup->IsActive (),		// [in] active state
			pGroup->GetUpdateRate (),	// [in] requested update rate
			(OPCHANDLE) pGroup,			// [in] our handle to this group
			&lBias,						// [in] time bias
			&fDeadband,					// [in] percent deadband
			pGroup->GetLanguageID (),	// [in] requested language ID
			&hServer,					// [out] server handle to this group
			&dwRevUpdateRate,			// [out] revised update rate
			IID_IUnknown,				// [in] request an IUknown pointer
			&pUnknown);

	// OPC Group was successfully added: 组加入到OPC成功
	if (SUCCEEDED (hr))
	{
			
		pGroup->SetValid (TRUE);// 组加入OPC有效标志

		// We should have gotten a valid pointer to the OPC Groups's IUnknown interface.
		// Set some things that only make sence if we have a vaild IUnknown pointer:
		if (pUnknown)
		{
			// Set the OPC Server's handle for this group:
			pGroup->SetServerHandle (hServer);

			// Reset update rate if OPC Server does not support requested rate:
			if (pGroup->GetUpdateRate () != dwRevUpdateRate)
				pGroup->SetUpdateRate (dwRevUpdateRate);

			// Initialize the CKGroup object, which will include getting necessary
			// interface pointers from IUnknown:
			pGroup->Initialize (pUnknown);

			// We can release the IUnknown pointer since initialized group
			// should have gotten the interface pointers it needs from it.
			pUnknown->Release ();
		}
		else
		{
			TRACE (_T("OTC: Warning added group %s to OPC server, but IUnknown is invalid.\r\n"),
				pGroup->GetName ());
		}
	}
	// OPC Group was not successfully added:
	else
	{
		szErr.Format("Add Group Err:<%s> <%s>",pGroup->GetName (), GetProgID ());
		return FALSE;
	}
	return TRUE;
}

// RemoveGroup ()删除组
void CKServer::RemoveGroup (CKGroup *pGroup)
{
	ASSERT (pGroup != NULL);

	HRESULT hr;
	
	pGroup->Uninitialize ();//组内清除,包括release接口，删除数据项

	// 从Server移除组
	if (pGroup->IsValid ())
	{
		ASSERT (m_pIServer != NULL);
		
		hr = m_pIServer->RemoveGroup (
			pGroup->GetServerHandle (),		// server handle for this group.
			pGroup->IsForceDeletion ());	// force a delete if this client has not released the group properly.

	}
	pGroup->SetValid (false);
}

void CKServer::ShutdownRequest (LPCTSTR lpszReason)
{
	//通知主程序StopServer
	if (m_pdrv)
		((COpcDrv*)m_pdrv)->Disconnect();
	Disconnect();
}


/////////////////////////////////////////////////////////////////////////////
// CKGroup construction/destruction
/////////////////////////////////////////////////////////////////////////////

CKGroup::CKGroup ()
{
	m_pDrv = NULL;
	m_pServer = NULL;

	// OPC Group properties:
	m_strName			= GROUP_DEFAULT_NAME;
	m_dwUpdateRate		= GROUP_DEFAULT_UPDATERATE;	
	m_dwLanguageID		= GROUP_DEFAULT_LANGUAGEID;	
	m_lBias				= GROUP_DEFAULT_TIMEBIAS;
	m_fDeadband			= GROUP_DEFAULT_DEADBAND;
	m_bActive			= GROUP_DEFAULT_ACTIVESTATE;
	m_dwUpdateMethod	= GROUP_DEFAULT_UPDATEMETHOD;

	m_hServer		= NULL;
	m_bValid		= FALSE;

	// Flags:
	ZeroMemory (&m_bfFlags, sizeof (m_bfFlags));

	// Interface pointers we will query and maintain:
	m_pIGroupState			= NULL;
	m_pIPublicGroupState	= NULL;
	m_pIItemMgt				= NULL;
	m_pISync				= NULL;
	m_pIAsync				= NULL;
	m_pIDataObject			= NULL;
	m_pIAsync2				= NULL;		
	m_pIConnPtContainer		= NULL;

	m_pIDataSink20			= NULL;
	m_dwCookieDataSink20	= 0;

	m_pIAdviseSink			= NULL;
	m_dwCookieRead			= 0;
	m_dwCookieWrite			= 0;
}


CKGroup::~CKGroup ()
{
	// Assert that all interfaces and items have been properly released:

	ASSERT (m_pIGroupState == NULL);
	ASSERT (m_pIPublicGroupState == NULL);
	ASSERT (m_pIItemMgt == NULL);
	ASSERT (m_pISync == NULL);
	ASSERT (m_pIAsync == NULL);
	ASSERT (m_pIDataObject == NULL);
	ASSERT (m_pIAsync2 == NULL);
	ASSERT (m_pIConnPtContainer == NULL);
	
	ASSERT (m_pIDataSink20 == NULL);
	ASSERT (m_dwCookieDataSink20 == 0);

	ASSERT (m_pIAdviseSink == NULL);
	ASSERT (m_dwCookieRead == 0);
	ASSERT (m_dwCookieWrite == 0);
}


/////////////////////////////////////////////////////////////////////////////
// CKGroup item management
/////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// AddItems ()添加数据项
//
// Description:
//	Called to add multiple OPC Items at one time.  A CKItem will be added to
//  this object if	needed and a request to add a corresponding OPC Item to
//  the OPC Group will be made.
//
// Parameters:
//  CObArray	&cItemList		Array of item objects to add. 数据项对象数组
//	DWORD		dwCount			Number of items in cItemList.
//	bool		bLoadingProject Set to true if call is made during project 
//								  load so item can be added to this object's
//								  imem list.  Otherwise we will assume item
//								  already exists in list, and only needs to
//								  added to OPC Group.
//
// Returns:
//  void
// **************************************************************************
BOOL CKGroup::AddItems (CObArray &cItemList, DWORD dwCount,CString &szErr)
{
	CKItem *pItem = NULL;
	DWORD dwIndex = 0;

//	ASSERT (dwCount > 0);

	if(!m_pServer->IsConnected ())
	{
		szErr.Format("Err:OpcServer<%s> Not Connect",m_pServer->GetProgID());
		return FALSE;
	}
	if(m_pIItemMgt == NULL)
	{
		szErr.Format("Err:Group <%s> m_pIItemMgt == NULL",m_strName);
		return FALSE;
	}

	HRESULT hr;
	OPCITEMDEF *pItemArray = NULL;
	OPCITEMRESULT *pResults = NULL;
	HRESULT *pErrors = NULL;
	DWORD dwLen;
	DWORD cdwSuccess = 0;

	// Allocate memory for item definition array:
	pItemArray = (OPCITEMDEF *) CoTaskMemAlloc (dwCount * sizeof (OPCITEMDEF));

	// Don't bother if memory allocation failed:
	if( pItemArray == NULL)
	{
		szErr="Err:Allocate memory for item definition array = NULL";
		return FALSE;
	}

	// Fill the item definition array: 填写每一个item
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		// Get pointer to item in input array:
		pItem = (CKItem *) cItemList [dwIndex];
		ASSERT (pItem != NULL);

		pItemArray [dwIndex].szAccessPath = NULL; //不要AccessPath

		// Multibyte to wide character conversion for Item ID:
		dwLen = lstrlen (pItem->GetItemID ()); // This can't be zero, so no test as above
		pItemArray [dwIndex].szItemID = (WCHAR *) CoTaskMemAlloc ((dwLen + 1) * sizeof (WCHAR));

		MytoWchar(pItemArray [dwIndex].szItemID,pItem->GetItemID ());//jy200807w

		// Set remaining structure members:
		// (If requested data type is NULL, the OPC Server should return
		// the default type.  The returned canonical data type may not be
		// the same as the requested data type.)
		pItemArray [dwIndex].bActive = pItem->IsActive ();	// active state
		pItemArray [dwIndex].hClient = (OPCHANDLE) pItem;	// our handle to item
		pItemArray [dwIndex].dwBlobSize = 0;				// no blob support
		pItemArray [dwIndex].pBlob = NULL;
		pItemArray [dwIndex].vtRequestedDataType = pItem->GetDataType (); // Requested data type
	}
	// 添加到组
	// Issue request add items request through IOPCItemMgt interface:
	hr = m_pIItemMgt->AddItems (
					dwCount,		// Item count
					pItemArray,		// Array of item definition structures
					&pResults,		// Result array
					&pErrors);		// Error array

	// Check results for each item:
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)	// 检查每一个结果
	{
		// Get pointer to item in input array:
		pItem = (CKItem *) cItemList [dwIndex];
		ASSERT (pItem != NULL);

		// 如果其中有不存在的标签，则返回的hr可能为S_FALSE，所以这里不能使用SUCCEEDED(hr)作为条件
		//if ( SUCCEEDED(hr) && (pErrors && SUCCEEDED (pErrors [dwIndex])))
		if (pErrors && SUCCEEDED(pErrors [dwIndex]))
		{
			// Set CKItem valid:
			pItem->SetValid (TRUE);
			TRACE("AddItem Success <%s>\n",pItem->GetItemID());
			// Update CKItem properties:
			// (Note, the returned canonical data type may not be the same
			// as the requested data type.)
			pItem->SetServerHandle (pResults [dwIndex].hServer);
			pItem->SetDataType (pResults [dwIndex].vtCanonicalDataType);
			pItem->SetAccessRights (pResults [dwIndex].dwAccessRights);

			// If the server passed back a blob, it is our responsibilty
			// to free it.  (Wrap with exception handler in case they were
			// bad and did not initialize the blob pointer.)
			try
			{
				if (pResults [dwIndex].pBlob != NULL)
					CoTaskMemFree (pResults [dwIndex].pBlob);
			}
					
			catch (...)
			{
				// Probably hit a bad pointer.  Try to process next item.
				TRACE (_T("OTC: AddItems exception thrown from freeing invalid blob.\r\n"));
			}
			// Bump success count:
			++cdwSuccess;
		}
				
		// There were errors:
		else
		{
			// Set item invalid:
			pItem->SetValid (FALSE);
			// Use NULL server handle for invalid items:
			pItem->SetServerHandle (NULL);

			((COpcDrv*)m_pDrv)->AddDebugFmt(_T("在OPC Server端无法查询到标签<%s>，可能该标签不存在"), pItem->GetItemID());
		}

		// COM requires us to free memory allocated for [out] and [in/out]
		// arguments (i.e. access path and item id strings):
		if (pItemArray [dwIndex].szAccessPath)
			CoTaskMemFree (pItemArray [dwIndex].szAccessPath);

		if (pItemArray [dwIndex].szItemID)
			CoTaskMemFree (pItemArray [dwIndex].szItemID);
	}
			
	// COM requires us to free memory allocated for [out] and [in/out]
	// arguments (i.e. pResults, pErrors, and pItemArray).
	if (pResults)
		CoTaskMemFree (pResults);
	if (pErrors)
		CoTaskMemFree (pErrors);

	CoTaskMemFree (pItemArray);
	
	return TRUE;
}

// **************************************************************************
// RemoveItems () 从OPC组内移除items
//
// Description:
//	Remove OPC Items from associated OPC Group and delete associated CKItems
//	if asked.
//
// Parameters:
//  CObArray	&cItemList		Array of items to remove.
//	DWORD		dwCount			Number of items in dwCount
//	bool		bDelete			Set to true to delete CKItem objects.
//
// Returns:
//  void
// **************************************************************************
BOOL CKGroup::RemoveItems (CObArray &cItemList, DWORD dwCount, CString &szErr)
{
	DWORD dwIndex = 0;
	CKItem *pItem = NULL;

	if(!m_pServer->IsConnected ())
	{
		szErr.Format("Err:OpcServer<%s> Not Connect!",m_pServer->GetProgID());
		return FALSE;
	}

	OPCHANDLE *phServer		= NULL;
	HRESULT *pErrors		= NULL;
	HRESULT hr				= E_FAIL;
	DWORD cdwItemsRemoved	= 0;

	if (m_pIItemMgt == NULL)
		return TRUE;

	ASSERT (m_pIItemMgt != NULL);

	ASSERT (dwCount > 0);

	// Allocate memory for IOPCServer::RemoveItems() [in] arguments:
	phServer = (OPCHANDLE *) CoTaskMemAlloc (dwCount * sizeof (OPCHANDLE));

	// We can't do anything if allocation failed:
	if (phServer == NULL)
	{
		ASSERT (FALSE);
		szErr="Err:CoTaskMemAlloc (dwCount * sizeof (OPCHANDLE)) in CKGroup::RemoveItems";
		return FALSE;
	}

	// Get the server handles associated with each item in input list:
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		// Get pointer to item in input list:
		pItem = (CKItem *) cItemList [dwIndex];
		ASSERT (pItem != NULL);

		// Set server handle in request argument:
		phServer [dwIndex] = pItem->GetServerHandle ();
	}
		
	// Issue the remove items request through the IOPCItemMgt interface:
	hr = m_pIItemMgt->RemoveItems (
			dwCount,	// Item count
			phServer,	// Array of server handles for items
			&pErrors);	// Error array

	// Check error result and remove the items from the project:
	cdwItemsRemoved = dwCount;
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		// Get pointer to item in input list:
		pItem = (CKItem *) cItemList [dwIndex];
		ASSERT (pItem != NULL);

		// Check for successful remove:
		if (FAILED (hr))
		{
			// Request failed.  
			TRACE (_T("OTC: Failed to remove item %s on %s\\\\%s. (%08X)\r\n"), 
			pItem->GetItemID (), m_pServer->GetProgID (), GetName (), hr);
		}
		else if (FAILED (pErrors [dwIndex]))
		{
			// Request succeeded, but this item could not be removed.
			// Decrement number of (valid) items removed:
			if (pItem->IsValid ())
				--cdwItemsRemoved;

			TRACE (_T("OTC: Failed to remove item %s on %s\\\\%s. (%08X)\r\n"), 
				pItem->GetItemID (), m_pServer->GetProgID (), GetName (), pErrors [dwIndex]);
		}

		// because there is no longer an assoicated OPC Item.
		pItem->SetValid (false);
	}

	CoTaskMemFree (phServer);
	CoTaskMemFree (pErrors);
	return TRUE;
}

void CKGroup::WriteOneSync (OPCHANDLE hi, LPCTSTR lpszVal,int opcdadatype)// 同步写
{
	if (!m_pServer->IsConnected () || m_pISync==NULL)
		return;

	ASSERT (m_dwCookieDataSink20 != 0);

	DWORD dwIndex				= 0;
	OPCHANDLE *phServer			= NULL;
	VARIANT *pValues			= NULL;
	HRESULT *pErrors			= NULL;
	HRESULT hr					= E_FAIL;
	DWORD dwCancelTransID		= 0;
	DWORD cdwSuccess			= 0;
	DWORD dwClientTransID		= GetTickCount ();	// client provided transaction for notifications (not useful right now)
	DWORD cdwSentItems			= 1;
		
	// Allocate storage for server item handles and write values:
	phServer = (OPCHANDLE *) CoTaskMemAlloc ( sizeof (OPCHANDLE));
	pValues = (VARIANT *) CoTaskMemAlloc ( sizeof (VARIANT));

	if (phServer == NULL || pValues == NULL)
	{
		ASSERT (FALSE);
		return;
	}
	CString szVal=lpszVal;
	if (MapStringValToVariant (szVal, pValues [0], opcdadatype))
		phServer [0] = hi;
	else
		phServer [0] = NULL;

	try
	{
			// Issue write request using IOPCAsyncIO2 interface:
		hr = m_pISync->Write (
					1,	// Item count
					phServer,		// Array of server handles for items
					pValues,		// Array of values
					&pErrors);		// Array of errors
		TRACE("m_pISync->Write OK!\n");
	}
	catch (...)
	{
			// Probably hit a bad pointer.  Try to process next item.
		TRACE (_T("OTC: WriteOneSync exception handler invoked\r\n"));
		if (m_pServer->IsConnected ())
		{
			// We not disconnected so why was an exception thrown?
			ASSERT (FALSE);
		}
	}

	// COM requires us to free memory allocated for [out] and [in/out]
	// arguments (i.e. phServer, pValues, and pErrors):
	CoTaskMemFree (phServer);
	CoTaskMemFree (pValues);

	if (pErrors)
		CoTaskMemFree (pErrors);
}
// **************************************************************************
// ReadSync ()同步读
//
// Description:
//	Perform a synchronous read for specified items.
//	
// Parameters:
//	CObArray	&cItemList		Array of items to read.
//	DWORD		cdwItems		Number of items in cItemList.
//	bool		bDeviceRead		Set to true to perform device read, false
//								  for cache read.
//	bool		bPostMsg		Set to true to post successful read message.
//
// Returns:
//  void
// **************************************************************************
BOOL CKGroup::ReadSync (CObArray &cItemList, DWORD cdwItems, bool bDeviceRead, bool bPostMsg /* = true */)
{
//	ASSERT (cdwItems > 0);

	BOOL bConnected = TRUE;
	// There is no sence in building a request unless we are connected to
	// the OPC Server and have a pointer to the IOPCSyncIO interface.
	if (m_pServer->IsConnected () && m_pISync)
	{
		DWORD dwIndex			= 0;
		CKItem *pItem			= NULL;
		OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
		OPCHANDLE *phServer		= NULL;
		OPCITEMSTATE *pValues	= NULL;
		HRESULT *pErrors		= NULL;
		HRESULT hr				= E_FAIL;
		DWORD cdwSuccess		= 0;

		// Allocate storage for server item handles 
		phServer = (OPCHANDLE *) CoTaskMemAlloc (cdwItems * sizeof (OPCHANDLE));

		// Return if allocation failed:
		if (phServer == NULL)
			return bConnected;

		// Fill request [in] arguments:
		for (dwIndex = 0; dwIndex < cdwItems; dwIndex++)
		{
			// Get pointer to item in input list:
			CKItem *pItem = (CKItem *) cItemList [dwIndex];
			ASSERT (pItem != NULL);

			// Set server handle:
			phServer [dwIndex] = pItem->GetServerHandle ();
		}

		// Wrap request processing in exception handler in case we get
		// a bad pointer:
		try
		{
			// Issue read request using IOPCSyncIO interface:
			hr = m_pISync->Read (
					dwSource,	// Source (device or cache)
					cdwItems,	// Item count
					phServer,	// Array of server handles for items
					&pValues,	// Array of values
					&pErrors);	// Array of errors

			// Check results:
			for (dwIndex = 0; dwIndex < cdwItems; dwIndex++)
			{
				// Get pointer to item in input list:
				CKItem *pItem = (CKItem *) cItemList [dwIndex];
				ASSERT (pItem != NULL);

				// Request succeeded and there were no errors:
				if (SUCCEEDED (hr) && pValues && (pErrors && SUCCEEDED (pErrors [dwIndex])))
				{
					// Increment the successful read counter:
					++cdwSuccess;
					// Update item data:
					pItem->UpdateData (pValues [dwIndex].vDataValue, 
						pValues [dwIndex].wQuality, pValues [dwIndex].ftTimeStamp);
					//TRACE("同步\n");
				}

				// Failure:
				else
				{
					// Request succeeded, but one or more items could not be read:
					if (pErrors && FAILED (pErrors [dwIndex]))
					{
						// Log error message with error code.  (Could use GetErrorString()
						// to get user friendly error string if we wish.)
//						LogMsg (bDeviceRead ? IDS_SYNC_READDEVICE_FAILED : IDS_SYNC_READCACHE_FAILED,
//							pItem->GetItemID (), GetName (), pErrors [dwIndex]);
					}

					// Request failed:
					else
					{
						// Log error message with error code.  (Could use GetErrorString()
						// to get user friendly error string if we wish.)
//						LogMsg (bDeviceRead ? IDS_SYNC_READDEVICE_FAILED : IDS_SYNC_READCACHE_FAILED,
//							pItem->GetItemID (), GetName (), hr);
					}
				}

				// Should clear variant before we free it later:
				if (pValues)
					VariantClear (&pValues [dwIndex].vDataValue);
				else
					((COpcDrv *)m_pDrv)->AddDebug(_T("ReadSync失败，pValues==NULL"));
					
			}// for

			TRACE(_T("CKGroup::ReadSync() 调用 DoMultiVal()\n"));

			((COpcDrv *)m_pDrv)->DoMultiVal();//批量处理接收数据
		}
		
		catch (...)
		{
			bConnected = FALSE;

			// Probably hit a bad pointer.  Try to process next item.
			TRACE (_T("OTC: ReadSync exception handler invoked\r\n"));

			if (m_pServer->IsConnected ())
			{
				// We not disconnected so why was an exception thrown?
				ASSERT (FALSE);
				
			}
		}

		// COM requires us to free memory allocated for [out] and [in/out]
		// arguments (i.e. phServer, pValues, and pErrors):
		CoTaskMemFree (phServer);

		if (pValues)
			CoTaskMemFree (pValues);	

		if (pErrors)
			CoTaskMemFree (pErrors);

		// Log success status if we are asked to post messages:
		if (bPostMsg && cdwSuccess > 0)
		{
			// Log diferent message for read device and read cache:
//			LogMsg (bDeviceRead ? IDS_SYNC_READDEVICE_SUCCESS : IDS_SYNC_READCACHE_SUCCESS, 
//				cdwSuccess, GetName ());
		}
		if(cdwSuccess >0)
			return TRUE;
		return FALSE;
	}
	return FALSE;
}

// **************************************************************************
// RefreshAsync10 ()
//
// Description:
//	Perform an asynchronous 1.0 refresh.
//
// Parameters:
//	bool		bDeviceRead		Set to true to perform device refresh, false
//								  for cache refresh.	
//
// Returns:
//  void
// **************************************************************************
BOOL CKGroup::RefreshAsync10 (bool bDeviceRead)
	{
//	ASSERT (m_cdwItems > 0);

	// There is no sence in building a request unless we are connected to
	// the OPC Server and have a pointer to the IOPCAsyncIO interface.
	if (m_pServer->IsConnected () && m_pIAsync)
		{
		ASSERT (m_dwCookieRead != 0);
	
		OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
		DWORD dwTransID = 0;
		HRESULT hr = E_FAIL;

		// Wrap request processing in exception hander in case we get
		// a bad pointer - Not really needed in this case, but this keeps
		// same code structure as other operations:
		try
			{
			// Issue refresh request using IOPCAsyncIO interface:
			hr = m_pIAsync->Refresh (
					m_dwCookieRead,	// Cookie returned from DAdvise
					dwSource,		// Source (device or cache)
					&dwTransID);	// Server generated transaction id
			if (FAILED (hr))
				return FALSE;
		}
		
		catch (...)
		{
			// Probably hit a bad pointer.  Try to process next item.
			TRACE (_T("OTC: RefreshAsync10 exception handler invoked\r\n"));

			if (m_pServer->IsConnected ())
			{
				// We not disconnected so why was an exception thrown?
				ASSERT (FALSE);
			}
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

// **************************************************************************
// RefreshAsync20 ()
//
// Description:
//	Perform an asynchronous 2.0 refresh.
//
// Parameters:
//	bool		bDeviceRead		Set to true to perform device refresh, false
//								  for cache refresh.	
//
// Returns:
//  void 
// **************************************************************************
BOOL CKGroup::RefreshAsync20 (bool bDeviceRead)
	{
//	ASSERT (m_cdwItems > 0);
	
	// There is no sence in building a request unless we are connected to
	// the OPC Server and have a pointer to the IOPCAsyncIO2 interface.
	if (m_pServer->IsConnected () && m_pIAsync2)
		{
		ASSERT (m_dwCookieDataSink20 != 0);

		OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
		DWORD dwCancelTransID		= 0;
		DWORD dwClientTransID		= GetTickCount ();	// client provided transaction for notifications (not useful right now)
		HRESULT hr = E_FAIL;

		// Wrap request processing in exception hander in case we get
		// a bad pointer - Not really needed in this case, but this keeps
		// same code structure as other operations:
		try
		{
			// Issue refresh request using IOPCAsyncIO2 interface:
			hr = m_pIAsync2->Refresh2 (
				dwSource,			// Source (device or cache)
				dwClientTransID,	// Client generated transaction id received in callback
				&dwCancelTransID);	// Server generated transaction id used for canceling this xact

			if (FAILED (hr))
			{
				((COpcDrv*)m_pDrv)->AddErrorFmt(_T("调用IOPCAsyncIO2->Refresh2()失败，错误码为：0x%02X"), hr);
				return FALSE;
			}
			((COpcDrv*)m_pDrv)->AddDebug(_T("调用IOPCAsyncIO2->Refresh2()成功"));
		}
		
		catch (...)
		{
			// Probably hit a bad pointer.  Try to process next item.
			TRACE (_T("OTC: RefreshAsync20 exception handler invoked\r\n"));

			((COpcDrv*)m_pDrv)->AddError(_T("调用IOPCAsyncIO2->Refresh2()失败，错误信息为：exception handler invoked"));

			if (m_pServer->IsConnected ())
			{
				// We not disconnected so why was an exception thrown?
				ASSERT (FALSE);
			}
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CKGroup helpers
/////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// MapStringValToVariant ()
//
// Description:
//	Map a string value to a variant.
//
// Parameters:
//  CString		&strValue	String to be converted to variant
//	VARIANT		&vtVal		Variant.
//	VARTYPE		vtType		Variant type to force string to.
//
// Returns:
//  bool - true if success.
// **************************************************************************
bool CKGroup::MapStringValToVariant (CString &strValue, VARIANT &vtVal, VARTYPE vtType)
	{
	// Assume success until a problem arises:
	bool bSuccess = true;

	// Initialize variant for use:
	VariantInit (&vtVal);

	// Set variant type:
	vtVal.vt = vtType;

	// Cast string to proper value:
	switch (vtVal.vt)
		{
		case VT_BOOL:
			{
			unsigned char ucVal = (unsigned char) _ttol (strValue);
			vtVal.boolVal = ucVal ? VARIANT_TRUE : VARIANT_FALSE;
			}
			break;

		case VT_UI1:
			vtVal.bVal = (unsigned char) _ttol (strValue);
			break;

		case VT_I1:
			vtVal.cVal = (char) _ttol (strValue);
			break;

		case VT_UI2:
			vtVal.uiVal = (unsigned short) _ttol (strValue);
			break;

		case VT_I2:
			vtVal.iVal = (short) _ttol (strValue);
			break;

		case VT_UI4:
			vtVal.ulVal = (unsigned long) _ttol (strValue);
			break;

		case VT_I4:
			vtVal.lVal = (long) _ttol (strValue);
			break;

		case VT_R4:
			// Text to Float conversions can threow expections.  Be
			// prepared to handle them:
			try
			{
				vtVal.fltVal = (float) atof (strValue);
			}
		
			catch (...)
				{
				// Set value to zero in case of conversion error:
				vtVal.fltVal = 0;
				}
			break;

		case VT_R8:
			// Text to Float conversions can threow expections.  Be
			// prepared to handle them:
			try
				{
				vtVal.dblVal = (double) atof (strValue);
				}
			
			catch (...)
				{
				// Set value to zero in case of conversion error:
				vtVal.dblVal = 0;
				}
			break;

		case VT_BSTR:
			{
			// Get string length:
			DWORD dwLength = strValue.GetLength () + 1;

			// Alocate memory for string:
			vtVal.bstrVal = SysAllocStringLen (NULL, dwLength);

			// ANSI build requires string conversion:
			MytoWchar(vtVal.bstrVal,strValue);//jy200807w
			//MultiByteToWideChar (CP_ACP, 0, strValue, -1, vtVal.bstrVal, dwLength);
			}
			break;

		// We don't expect array types.  If so, return false:
		case VT_UI1	| VT_ARRAY:
		case VT_I1	| VT_ARRAY:
		case VT_UI2	| VT_ARRAY:
		case VT_I2	| VT_ARRAY:
		case VT_UI4	| VT_ARRAY:
		case VT_I4	| VT_ARRAY:
		case VT_R4	| VT_ARRAY:
		case VT_R8	| VT_ARRAY:
			ASSERT (FALSE);
			bSuccess = false;
			break;

		default:
			// Unexpected variant type.  Return false:
			bSuccess = false;
			break;
		}

	// Return success:
	return (bSuccess);
	}

// **************************************************************************
// GetArrayElement ()
//
// Description:
//	Parse an element from a comma delimited string of array values.  Called 
//	once per element.  pnStart will be reset to end of current element for 
//	subsequent call.
//
// Parameters:
//  LPCTSTR		szInBuff	Input string.
//	int			*pnStart	Character position to begin parse.
//	LPTSTR		szOutBuff	Array element output string buffer.
//	int			nBuffSize	Size of output buffer.
//
// Returns:
//  GETARRELEMRET - Values enumerated in group.h.
// **************************************************************************
CKGroup::GETARRELEMRET CKGroup::GetArrayElement (LPCTSTR szInBuff, int *pnStart, LPTSTR szOutBuff, int nBuffSize)
	{
	// Initialize some variables:
	TCHAR ch = _T('\r');
	BOOL bLeftBracket = FALSE;
	int cnChars = 0;
	
	GETARRELEMRET eRet = tElement;

	// If we are at the end of the buffer then we're done:
	if (!szInBuff [*pnStart])
		return (tDone);

	// Continue to read characters until we hit the next comma or
	// an EOL (End Of Line - "\n") character:
	while (TRUE)
		{
		// Get the next character, and increment start position for next time:
		ch = szInBuff [(*pnStart)++];

		// If the character is NULL, we are at the end of the record and
		// therefore is nothing left to read.  Back up the start position
		// and break out of the loop.
		if (!ch)
			{
			(*pnStart)--;
			break;
			}

		// Trim leading whitespace, if any.  If current character is a space,
		// then continue.  This will force us to process next character without
		// saving current one (a space) in output buffer.
		if (!cnChars && _istspace (ch))
			continue;

		// Brackets and commas delimit the fields:
		if (ch == _T('['))
			{
			if (bLeftBracket)
				{
				eRet = tInvalid;
				break;
				}

			continue;
			}

		if (ch == _T(']'))
			{
			eRet = tEndRow;
			break;
			}

		if (ch == _T(','))
			break;

		// If we make it here, then the current character is "a keeper".
		// Increment the character counter:
		++cnChars;

		// Add the current character to the output buffer as long a there
		// is room:
		if (nBuffSize > 1)
			{
			// There is room, so add the current character to the output 
			// buffer, and move output buffer pointer to next position.
			*szOutBuff++ = ch;

			// Decrement nBuffSize.  When it hits zero we know we have
			// filled the output buffer to capacity.
			--nBuffSize;
			}
		else
			{
			// There is no more room in the output buffer.  Set the bOverflow
			// flag, but don't break out of the loop.  We want to keep going
			// until we hit a field deleimiter.  This will allow us to parse
			// the next field, even though this one is too big to deal with.
			eRet = tOverflow;
			break;
			}
		}

	// Make sure the output string is properly NULL terminated:
	*szOutBuff = 0;

	// Return return code:
	return (eRet);
	}

// **************************************************************************
// MapStringValToArrayVariant ()
//
// Description:
//	Map a string of array elements to a variant array.
//
// Parameters:
//  CString		&strValue		Input string.
//	VARIANT		*pvtSrc			Source variant.  Used to set properties of
//								  destination variant.
//	VARIANT		*pvtDst			Destination variant
//
// Returns:
//  bool - true if successful.
// **************************************************************************
bool CKGroup::MapStringValToArrayVariant (CString &strValue, VARIANT *pvtSrc, VARIANT *pvtDst)
	{
	// Source variant must not be empty:
	if (pvtSrc->vt == VT_EMPTY)
		return (false);

	ASSERT (pvtSrc != NULL);
	ASSERT (pvtDst != NULL);
	ASSERT (pvtSrc->vt & VT_ARRAY);

	// Source variant must contain array data:
	if (!pvtSrc->parray)
		return (false);

	int cnRows = 0;
	int cnCols = 0;
	TCHAR szValue [DEFBUFFSIZE];

	// Create a local copy of input string:
	lstrcpyn (szValue, strValue, (sizeof (szValue) / sizeof (TCHAR)) - sizeof (TCHAR));

	// Set number of rows and columns for this array:
	
	// 1-D array:
	if (pvtSrc->parray->cDims == 1)
		{
		cnRows = 1;
		cnCols = pvtSrc->parray->rgsabound [0].cElements;
		}

	// 2-D array:
	else if (pvtSrc->parray->cDims == 2)
		{
		cnRows = pvtSrc->parray->rgsabound [0].cElements;
		cnCols = pvtSrc->parray->rgsabound [1].cElements;
		}

	// Do not support more than 2 dimensions:
	else
		{
		ASSERT (FALSE);
		return (false);
		}

	// Initialize destination array:
	VariantInit (pvtDst);

	// Set destination array type:
	pvtDst->vt = pvtSrc->vt;

	// Allocate memory for destination array:
	pvtDst->parray = SafeArrayCreate (pvtSrc->vt & ~VT_ARRAY, 
		pvtSrc->parray->cDims, pvtSrc->parray->rgsabound);

	// Copy data to destination array:
	VariantCopy (pvtDst, pvtSrc);

	// Overwrite fields with data:
	int nIndex = 0;
	GETARRELEMRET eRet;
	BYTE HUGEP *pVal = (BYTE *) pvtDst->parray->pvData;
	int cnRowElements = 0;

	// Loop over all array elements:
	for (int i = 0; i < cnRows * cnCols; i++)
		{
		// Create a scratch buffer for parsed array element string:
		TCHAR szBuffer [DEFBUFFSIZE];

		// Parse next element in string:
		eRet = GetArrayElement (szValue, &nIndex, szBuffer, DEFBUFFSIZE);

		// If parse resulted in done code, the break out of loop:
		if (eRet == tDone)
			break;

		// If parse resulted in invalid code, return false:
		if (eRet == tInvalid)
			{
			TRACE (_T("OTC: Invalid character parsing array value\r\n"));
			return (false);
			}

		// If parse reslted in overflow code, return false:
		if (eRet == tOverflow)
			{
			TRACE (_T("OTC: Buffer overflow parsing array value\r\n"));
			return (false);
			}

		// If we make it here, we expect to have element or end of row code:
		ASSERT (eRet == tElement || eRet == tEndRow);

		// If element is not NULL string, then overwrite field in destination:
		if (*szBuffer != _T('\0'))
			{
			// Cast string to proper type:
			switch (pvtDst->vt & ~VT_ARRAY)
				{
				case VT_UI1	:	*pVal = (BYTE) _ttol (szBuffer); break;
				case VT_I1	:	*pVal = (char) _ttol (szBuffer);	break;
				case VT_UI2	:	*(WORD *)pVal = (WORD) _ttol (szBuffer);	break;
				case VT_I2	:	*(short *)pVal = (short) _ttol (szBuffer); break;
				case VT_UI4 :	*(DWORD *)pVal = (DWORD) _ttol (szBuffer); break;
				case VT_I4	:	*(long *)pVal = (long) _ttol (szBuffer);	break;

				case VT_R4:
					// Text to float conversions can throw exceptions, so 
					// be prepared to handle them:
					try
						{
						*(float *)pVal = (float) atof (szBuffer);
						}
					
					catch (...)
						{
						*(float *)pVal = 0;
						}

					break;

				case VT_R8:
					// Text to float conversions can throw exceptions, so 
					// be prepared to handle them:
					try
						{
						*(double *)pVal = (double) atof (szBuffer);
						}
					
					catch (...)
						{
						*(double *)pVal = 0;
						}
					break;

				// Unexpected type:
				default:
					ASSERT (FALSE);
					break;
				}
			}

		// Increment destination array pointer to accept next element:

		// If last element in row:
		if (eRet == tEndRow && cnCols > cnRowElements)
			{
			pVal += (pvtDst->parray->cbElements * (cnCols - cnRowElements));
			cnRowElements = 0;
			}

		// Else next element:
		else
			{
			cnRowElements++;
			pVal += pvtDst->parray->cbElements;
			}
		}

	// If we make it here, then return true to indicate success:
	return (true);
	}


/////////////////////////////////////////////////////////////////////////////
// CKGroup OPC Specifics
/////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// Initialize ()
//
// Description:
//	Initialize group.通过组IUnknown接口来获取组内其他接口
//
// Parameters:
//  LPUNKNOWN	pUnknown		Pointer to group IUnknown interface.
//
// Returns:
//  void
// **************************************************************************
void CKGroup::Initialize (LPUNKNOWN pUnknown)
{
	ASSERT (pUnknown != NULL);
	TRACE (_T("OTC: Initializing group %s interfaces.\r\n"), GetName ());

	HRESULT hr;


	//////////////////////////////////////////////////////////////
	// Get IID_IOPCGroupStateMgt interface and set group state	//
	//////////////////////////////////////////////////////////////

	// Get pointer to IID_IOPCGroupStateMgt interface, save it in 
	// m_pIGroupState:
	ASSERT (m_pIGroupState == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCGroupStateMgt, (void **) &(m_pIGroupState));

	// If we got the interface, use it to get the OPC Group state, and
	// save state properties in this object:
	if (SUCCEEDED (hr) && m_pIGroupState)
	{
		// Get our current group name setting:
		CString strItemID = GetName ();

		// If name has not been set yet, we are relying on server to get it
		// and other properties.  Get name and other properties from server:
		if (strItemID.IsEmpty ())
		{
			WCHAR *pszName;
			OPCHANDLE hClient;

			// Use IID_IOPCGroupStateMgt interface to get OPC Group
			// properties:
			if (SUCCEEDED (m_pIGroupState->GetState (
				&m_dwUpdateRate,	// update rate
				&m_bActive,			// active state
				&pszName,			// put the name here
				&m_lBias,			// time bias
				&m_fDeadband,		// deadband
				&m_dwLanguageID,	// language id
				&hClient,			// server handle
				&m_hServer)))		// server handle
			{
				if (pszName)
				{
					CString sztt = pszName;
					SetName (sztt);
					CoTaskMemFree (pszName);
				}
			}
		}
	}
	else {
		((COpcDrv*)m_pDrv)->AddErrorFmt(_T("查询IOPCGroupStateMgt接口失败，可能OPC服务器不支持该接口！"));

		// Issue TRACE message if we failed to get IID_IOPCGroupStateMgt
		// interface pointer:
		TRACE (_T("OTC: Failed to query IID_IOPCGroupStateMgt (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIGroupState);
	}


	//////////////////////////////////////
	// Get other interface pointers.	//
	//////////////////////////////////////

	// Get IID_IOPCPublicGroupStateMgt interface:
	ASSERT (m_pIPublicGroupState == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCPublicGroupStateMgt, (void **) &(m_pIPublicGroupState));

	if (FAILED (hr) || m_pIPublicGroupState == NULL)
	{
		((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IOPCPublicGroupStateMgt接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IOPCPublicGroupStateMgt (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIPublicGroupState);
	}

	// Get IID_IOPCItemMgt interface:
	ASSERT (m_pIItemMgt == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCItemMgt, (void **) &(m_pIItemMgt));

	if (FAILED (hr) || m_pIItemMgt == NULL)
	{
		((COpcDrv*)m_pDrv)->AddErrorFmt(_T("查询IOPCItemMgt接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IItemMgt (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIItemMgt);
	}

	// Get IID_IOPCSyncIO interface:
	ASSERT (m_pISync == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCSyncIO, (void **) &(m_pISync));

	if (FAILED (hr) || m_pISync == NULL)
	{
		((COpcDrv*)m_pDrv)->AddErrorFmt(_T("查询IOPCSyncIO接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_ISyncIO (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pISync);
	}

	// Get IID_IOPCAsyncIO interface:
	ASSERT (m_pIAsync == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCAsyncIO, (void **) &(m_pIAsync));

	if (FAILED (hr) || m_pIAsync == NULL)
	{
		((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IOPCAsyncIO接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IAsyncIO (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIAsync);
	}

	// Get IID_IDataObject interface:
	ASSERT (m_pIDataObject == NULL);
	hr = pUnknown->QueryInterface (
		IID_IDataObject, (void **) &(m_pIDataObject));

	if (FAILED (hr) || m_pIDataObject == NULL)
	{
		((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IDataObject接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IDataObject (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIDataObject);
	}

	// Get IID_IOPCAsyncIO2 interface:
	ASSERT (m_pIAsync2 == NULL);
	hr = pUnknown->QueryInterface (
		IID_IOPCAsyncIO2, (void **) &(m_pIAsync2));

	if (FAILED (hr) || m_pIAsync2 == NULL)
	{
		((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IOPCAsyncIO2接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IAsyncIO2 (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIAsync2);
	}

	// Get IID_IConnectionPointContainer interface:
	ASSERT (m_pIConnPtContainer == NULL);
	hr = pUnknown->QueryInterface (
		IID_IConnectionPointContainer, (void **) &(m_pIConnPtContainer));

	if (FAILED (hr) || m_pIConnPtContainer == NULL)
	{
		((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IConnectionPointContainer接口失败，可能OPC服务器不支持该接口！"));

		TRACE (_T("OTC: Failed to query IID_IConnectionPointContainer (hr -> %08X, pI -> %08X)\r\n"),
			hr, m_pIConnPtContainer);
	}

	// 如果为同步方式，则不需要注册异步回调接口
	if (((COpcDrv*)m_pDrv)->m_nReadMode < 2)
		return;

	//////////////////////////////////////////////////
	// Request on data change update notifications.	//
	//////////////////////////////////////////////////

	// Set this flag if 2.0 is requested but not supported:
	bool bDropTo10 = false;
	
	// If we want OPC 2.0 Data Change Notifications:
	if (GetUpdateMethod () == OPC_20_DATACHANGE)
		{
		// Assume failure until proven good:
		HRESULT hr = E_FAIL;

		// We will need the IID_IConnectionPointContainer interface.  If
		// not we will fall through with hr = E_FAIL:
		if (m_pIConnPtContainer)
			{
			// Get connection point (IID_IOPCDataCallback interface):
			IConnectionPoint *pCP = NULL;
			hr = m_pIConnPtContainer->FindConnectionPoint (IID_IOPCDataCallback, &pCP);

			// If we succeeded to get connection point interface, create
			// our data sink interface and advise server of it:
			if (SUCCEEDED (hr))
				{
				try
					{
					// Instantiate a new IKDataSink20:
					m_pIDataSink20 = new IKDataSink20 ();

					// Add ourselves to its reference count:
					m_pIDataSink20->AddRef ();
					
					// Advise the server of our data sink:
					hr = pCP->Advise (m_pIDataSink20, &m_dwCookieDataSink20);

					// We are done with the IID_IOPCDataCallback, so release
					// (remove us from its reference count):
					pCP->Release ();
					}
				
				catch (...)
					{
					// If a problem, make sure hr = E_FAIL so error gets 
					// processed correctly below:
					ASSERT (FALSE);
					hr = E_FAIL;
					}
				}
			}

		// If we did not succeed in connecting to the server 2.0 data,
		// try the 1.0 interface.  We do this by setting bDropTo10 flag.
		// Make sure we also reset our update method too.
		if (FAILED (hr))
		{
			((COpcDrv*)m_pDrv)->AddDebugFmt(_T("查询IOPCDataCallback接口失败，可能OPC服务器不支持该接口！"));

			bDropTo10 = true;
//			LogMsg (IDS_CONNECT_TO_DATASINK20_FAILED, GetName (), m_pServer->GetProgID (), hr); 
			SetUpdateMethod (OPC_10_DATATIMECHANGE);
		}
	}

	// If we want OPC 1.0 Data Change Notifications (with or without timestamp)
	// or if we requested 2.0 but found that it is not supported by server:
	if ((GetUpdateMethod () == OPC_10_DATACHANGE) ||
		(GetUpdateMethod () == OPC_10_DATATIMECHANGE) ||
		(bDropTo10 == true))
		{
		// We will need the IID_IDataObject interface:
		if (m_pIDataObject)
			{
			// Will need to fill in a FORMATETC stucture to advise server
			// of our callback format:
			FORMATETC fmt;

			// First set "format" member (timestamp or not):
			if (GetUpdateMethod () == OPC_10_DATACHANGE)
				fmt.cfFormat = CF_DATA_CHANGE;
			else
				fmt.cfFormat = CF_DATA_CHANGE_TIME;

			// Fill in the "etc." members:
			fmt.dwAspect = DVASPECT_CONTENT;	// aspect of the object	
			fmt.ptd = NULL;						// pointer to a target device for which data is being composed
			fmt.tymed = TYMED_HGLOBAL;			// storage medium is a global memory handle (HGLOBAL
			fmt.lindex = -1;

			ASSERT (m_pIAdviseSink == NULL);

			try
				{
				// Instantiate a new CKAdviseSink object:
				m_pIAdviseSink = new CKAdviseSink ();
				m_pIAdviseSink->m_pDrv = m_pDrv;
				// Add ourselves to its reference count:
				m_pIAdviseSink->AddRef ();

				// Advise server of our data sink:
				hr = m_pIDataObject->DAdvise (
					&fmt,					// format of interest
					0,						// advise flags	(OPC not supported)
					m_pIAdviseSink,			// our advise sink callback
					&m_dwCookieRead);		// cookie used for AsyncIO and DUnadvise

				if (FAILED (hr))
					{
					TRACE (_T("OTC: IDataObject::DAdvise failed\r\n"));
					}
				}
			
			// If a problem, return:
			catch (...)
				{
				TRACE (_T("OTC: CKGroup::Initialize memory exception thrown\r\n"));
				return;
				}

			// Must also request 1.0 write complete notifications.
			// Reuse FORMATETC structure to advise server:
			fmt.cfFormat = CF_WRITE_COMPLETE;	// format		
			fmt.dwAspect = DVASPECT_CONTENT;	// aspect of the object	
			fmt.ptd = NULL;						// pointer to a target device for which data is being composed
			fmt.tymed = TYMED_HGLOBAL;			// storage medium is a global memory handle (HGLOBAL
			fmt.lindex = -1;

			// Advise server that we want write complete notifications to
			// come to our data sink:
			hr = m_pIDataObject->DAdvise (
				&fmt,					// format of interest
				0,						// advise flags (OPC not supported)
				m_pIAdviseSink,			// our advise sink callback
				&m_dwCookieWrite);		// cookie used for AsyncIO and DUnadvise

			if (FAILED (hr))
				{
				TRACE (_T("OTC: IDataObject::DAdvise failed\r\n"));
				}
			}
		
		// Can't get 1.0 updates because server does not support required
		// IID_IDataObject interface:
		else
			{
			TRACE (_T("OTC: Unable to request OnDataChange 1.0 updates, interface not supported.\r\n"));
			}
		}
	}

// **************************************************************************
// Uninitialize ()
//
// Description:
//	Cleanup group before remove. If bDelete is false, we  remove our server
//	references but we do not free any objects
//
// Parameters:
//  bool		bDelete		Set to true to delete items.
//
// Returns:
//  void
// **************************************************************************
void CKGroup::Uninitialize ()
{
	HRESULT hr;

	// Unadvise read notifications:
	if (m_dwCookieRead != 0)
	{
		ASSERT (m_pIDataObject != NULL);
		hr = m_pIDataObject->DUnadvise (m_dwCookieRead);

		if (FAILED (hr))
		{
			TRACE (_T("OTC: Failed on IDataObject::DUnadvise on group %s, server %s (%08X)\r\n"),
				GetName (), m_pServer->GetProgID (), hr);
		}

		m_dwCookieRead = 0;
	}

	// Unadvise write notifications:
	if (m_dwCookieWrite != 0)
	{
		ASSERT (m_pIDataObject != NULL);
		hr = m_pIDataObject->DUnadvise (m_dwCookieWrite);

		if (FAILED (hr))
		{
			TRACE (_T("OTC: Failed on IDataObject::DUnadvise on group %s, server %s (%08X)\r\n"),
				GetName (), m_pServer->GetProgID (), hr);
		}

		m_dwCookieWrite = 0;
	}

	// Release our reference to our 1.0 sink:
	if (m_pIAdviseSink != NULL)
	{
		m_pIAdviseSink->Release ();
		m_pIAdviseSink = NULL;
	}

	// Unadvise data access 2.0 notifications:
	if (m_dwCookieDataSink20 != 0)
	{
		ASSERT (m_pIConnPtContainer != NULL);

		hr = E_FAIL;
		IConnectionPoint *pCP = NULL;

		hr = m_pIConnPtContainer->FindConnectionPoint (IID_IOPCDataCallback, &pCP);

		if (SUCCEEDED (hr))
		{
			hr = pCP->Unadvise (m_dwCookieDataSink20);
			pCP->Release ();
		}

		if (FAILED (hr))
		{
			TRACE (_T("OTC: CKGroup::Uninitialize failed to unadvise 2.0 updates\r\n"));
		}

		m_dwCookieDataSink20 = 0;
	}

	// Release our reference to our 2.0 sink:
	if (m_pIDataSink20 != NULL)
	{
		m_pIDataSink20->Release ();
		m_pIDataSink20 = NULL;
	}

	// Remove all outstanding items:

	// Release outstanding group references:
	if (m_pIGroupState)
	{
		m_pIGroupState->Release ();
		m_pIGroupState = NULL;
	}

	if (m_pIPublicGroupState)
	{
		m_pIPublicGroupState->Release ();
		m_pIPublicGroupState = NULL;
	}

	if (m_pIItemMgt)
	{
		m_pIItemMgt->Release ();
		m_pIItemMgt = NULL;
	}

	if (m_pISync)
	{
		m_pISync->Release ();
		m_pISync = NULL;
	}

	if (m_pIAsync)
	{
		m_pIAsync->Release ();
		m_pIAsync = NULL;
	}

	if (m_pIDataObject)
	{
		m_pIDataObject->Release ();
		m_pIDataObject = NULL;
	}

	if (m_pIAsync2)
	{
		m_pIAsync2->Release ();
		m_pIAsync2 = NULL;
	}

	if (m_pIConnPtContainer)
	{
		m_pIConnPtContainer->Release ();
		m_pIConnPtContainer = NULL;
	}

	TRACE (_T("OTC: Uninitialized group %s on server %s.\r\n"), 
		GetName (), m_pServer->GetProgID ());
}

/////////////////////////////////////////////////////////////////////////////
// CKItem construction/destruction
/////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// CKItem ()
//
// Description:
//	Constructor
//
// Parameters:
//  CKGroup		*pParent	Pointer to parent group.
//
// Returns:
//  none
// **************************************************************************
CKItem::CKItem ()
{
	// Initialize other member variables:

	m_pGroup = NULL;

	m_bActive			= TRUE;
	m_vtDataType		= VT_EMPTY;
	m_dwAccessRights	= OPC_READABLE;

	m_bValid			= FALSE;
	m_bTimeStamped		= FALSE;

	ZeroMemory (&m_bfFlags, sizeof (m_bfFlags));
	
	GetSystemTimeAsFileTime (&m_ftTimeStamp);
	VariantInit (&m_vtValue);

	m_wQuality	= OPC_QUALITY_BAD_OUT_OF_SERVICE;

	m_hServer	= NULL;
	m_pParam = NULL;
	m_pParam2 = NULL;

#ifdef _DABA_OUT_OF_SERVICE_FILTER
	m_bOutOfServiceFilter = FALSE;
#endif

}

// **************************************************************************
// ~CKItem ()
//
// Description:
//	Destructor
//
// Parameters:
//  none
//
// Returns:
//  none
// **************************************************************************
CKItem::~CKItem ()
	{
	// Make sure our variant members get cleaned up properly:
	VariantClear (&m_vtValue);
	}
/*
void CTime2EcdTime(CTime *pct,ECDEVENTTIME *pecdt)
{
	pecdt->wYear = (WORD)pct->GetYear();
	pecdt->wMon = (WORD)pct->GetMonth();
	pecdt->wDay = (WORD)pct->GetDay();
	pecdt->wHour = (WORD)pct->GetHour();
	pecdt->wMin = (WORD)pct->GetMinute();
	pecdt->wSec = (WORD)pct->GetSecond();
	pecdt->wMsec = 0;
	pecdt->wRes = 0;
}*/
/////////////////////////////////////////////////////////////////////////////
// CKItem manipulators
/////////////////////////////////////////////////////////////////////////////

short CKItem::ToDBPQuality(WORD wQuality)
{
	BYTE btHi = (wQuality & 0xF0) >> 8; // 高8位由OPC服务器厂家自定义

	// 低8位由主质量、质量子状态和限制状态三部分组成，可表示为QQSSSSLL
	BYTE btQQ = (wQuality & 0xC0) >> 6;
	BYTE btSSSS = (wQuality & 0x3C) >> 2;
	BYTE btLL = (wQuality & 0x03);

	short nQuality = (short)wQuality;
	switch(btQQ)
	{
	case 0:	// Bad
		switch(btSSSS)
		{
		case 0:
			nQuality = DBP_QUALITY_BAD;
			break;
		case 1:
			nQuality = DBP_QUALITY_BAD_CONFIG_ERROR;
			break;
		case 2:
			nQuality = DBP_QUALITY_BAD_NOT_CONNECTED;
			break;
		case 3:
			nQuality = DBP_QUALITY_BAD_DEVICE_FAIL;
			break;
		case 4:
			nQuality = DBP_QUALITY_BAD_SENSOR_FAIL;
			break;
		case 5:
			nQuality = DBP_QUALITY_BAD_LAST;
			break;
		case 6:
			nQuality = DBP_QUALITY_BAD_COMM_FAIL;
			break;
		case 7:
			nQuality = DBP_QUALITY_BAD_OUT_OF_SERVICE;
			break;
		}
		break;
	case 1: // Uncertain
		if (btLL == 1)
			nQuality = DBP_QUALITY_LOLO_LIMIT;
		else if (btLL == 2)
			nQuality = DBP_QUALITY_HIHI_LIMIT;
		else {
			switch(btSSSS)
			{
			case 0:
				nQuality = DBP_QUALITY_UNCERTAIN;
				break;
			case 1:
				nQuality = DBP_QUALITY_UNCERTAIN_LAST;
				break;
			case 4:
				nQuality = DBP_QUALITY_UNCERTAIN_SENSOR;
				break;
			case 5:
				nQuality = DBP_QUALITY_UNCERTAIN_EU_EXCEEDED;
				break;
			case 6:
				nQuality = DBP_QUALITY_UNCERTAIN_SUB_NORMAL;
				break;
			}
		}
		break;
	case 3:	// Good
		switch(btLL)
		{
		case 1:
			nQuality = DBP_QUALITY_LO_LIMIT;
			break;
		case 2:
			nQuality = DBP_QUALITY_HI_LIMIT;
			break;
		default:
			nQuality = DBP_QUALITY_GOOD;
			break;
		}
		break;
	}

	return nQuality;
}

// **************************************************************************
// UpdateData ()
//
// Description:
//	Called to update the item's data and quality.
//
// Parameters:
//  VARIANT		&vtVal		Item's data.
//	WORD		wQuality	Item's quality.
//
// Returns:
//  void
// **************************************************************************
BOOL Varint2Drvrec(VARIANT &vtVal,CRdRec* prec, BOOL bZeroFilter)
{
	prec->ClearData();
	switch (vtVal.vt)
	{
		case VT_BOOL:
			prec->val.vt = DRT_DIGITAL;
			prec->val.lval = (vtVal.boolVal == VARIANT_FALSE) ? 0 : 1;

			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_UI1:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.bVal;

			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_I1:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.cVal;
			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_UI2:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.uiVal;
			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_I2:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.iVal;
			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_UI4:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.ulVal;
			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_I4:
			prec->val.vt = DRT_INT32;
			prec->val.lval = vtVal.lVal;
			if (bZeroFilter && (prec->val.lval == 0))
				return FALSE;
			break;

		case VT_R4:
			prec->val.vt = DRT_FLOAT32;
			prec->val.fval = vtVal.fltVal;
			if (bZeroFilter && (prec->val.fval == 0.0))
				return FALSE;
			break;
		case VT_I8:
		case VT_UI8:
			prec->val.vt = DRT_INT64;
			prec->val.llval = vtVal.lVal;
			if (bZeroFilter && (prec->val.llval == 0))
				return FALSE;
			break;
		case VT_R8:
			prec->val.vt = DRT_FLOAT64;
			prec->val.dblval = vtVal.dblVal;
			if (bZeroFilter && (prec->val.dblval == 0.0))
				return FALSE;
			break;
		case VT_BSTR:
			{
				CString strValue = vtVal.bstrVal; //不支持字符串
				prec->val.vt = DRT_STRING;
				*((CVarVal*)&prec->val) = strValue;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

void CKItem::UpdateData (VARIANT &vtVal, WORD wQuality)
{
#ifdef _DABA_OUT_OF_SERVICE_FILTER
	if (!m_bOutOfServiceFilter && ((wQuality == 0x1C) || (wQuality == 0x00)))
	{
		m_bOutOfServiceFilter = TRUE;
		return;
	}
	m_bOutOfServiceFilter = TRUE;
#endif
//	TRACE("UpdateData item <%s> SUCCESS\n",m_strItemID);
	if((m_pParam == NULL)||(m_pParam2 == NULL))
		return;
	CItemDef *pi=(CItemDef*)m_pParam;
	COpcDrv *pDrv=(COpcDrv*)m_pParam2;
	if(pDrv->m_cbOnDataOk == NULL)
		return;

	CRdRec rec;//值
	if(!Varint2Drvrec(vtVal, &rec, pi->m_bZeroFilter)) //转换值
		return;//转换不成功

	VariantCopy (&m_vtValue, &vtVal);//复制数据到m_vtValue
	m_wQuality = wQuality;

	// This function is used for non-timestamped data:
	m_bTimeStamped = FALSE;
	//CTime ct=CTime::GetCurrentTime(); //使用本地时间
	//long ltime = (long)ct.GetTime();
	long ltime = (long)::time(NULL);
	
	rec.SetName(pi->m_szName); //设置名称

	rec.val.ltime = ltime; //时标

	/*
	if((wQuality & 0xFC) != OPC_QUALITY_GOOD_NON_SPECIFIC)
		rec.val.snqa = QA_ERR;//不可靠值
	else
		rec.val.snqa = QA_OK;//可靠值  数据质量
	*/
	//{{ heqing 2011/06/24
	rec.val.snqa = ToDBPQuality(wQuality);
	//}}

	//{{ 测试代码
	/*
	if (rec.val.snqa > 0)
	{
		COleVariant val = vtVal;
		val.ChangeType(VT_BSTR);

		USES_CONVERSION;
		pDrv->AddInfoFmt(_T("OPC读到标签<%s>：类型 = %u, 值 = %s, 时标 = %d, 质量=0x%02X -> %d"), pi->m_szName, (DWORD)vtVal.vt, W2A(val.bstrVal), ltime, (DWORD)wQuality, rec.val.snqa);
	}
	*/
	//}}
	pDrv->AddDrvRec(&rec); //加入待回调区
}

// **************************************************************************
// UpdateData ()
//
// Description:
//	Called to update the item's data, quality and timestamp.
//
// Parameters:
//  VARIANT		&vtVal			Item's data.
//	WORD		wQuality		Item's quality.
//	FILETIME	&ftTimeStamp	Item's timestap.
//
// Returns:
//  void
// **************************************************************************

void CKItem::UpdateData (VARIANT &vtVal, WORD wQuality, FILETIME &ftTimeStamp)
{
#ifdef _DABA_OUT_OF_SERVICE_FILTER
	if (!m_bOutOfServiceFilter && ((wQuality == 0x1C) || (wQuality == 0x00)))
	{
		m_bOutOfServiceFilter = TRUE;
		return;
	}
	m_bOutOfServiceFilter = TRUE;
#endif

	if((m_pParam == NULL)||(m_pParam2 == NULL))
		return;
	CItemDef *pi=(CItemDef*)m_pParam;
	COpcDrv *pDrv=(COpcDrv*)m_pParam2;
	if(pDrv->m_cbOnDataOk == NULL)
		return;

#if 0
	if ((pi->m_szName.Find("EW001") != -1) && ((vtVal.fltVal < 100.0) || (vtVal.fltVal > 360.0)))
	{
		if ((m_ftTimeStamp.dwLowDateTime != ftTimeStamp.dwLowDateTime) ||
			(m_ftTimeStamp.dwHighDateTime != ftTimeStamp.dwHighDateTime) ||
			(m_vtValue.fltVal != vtVal.fltVal) || (m_wQuality != wQuality))
		{
			CTime tt(ftTimeStamp);

			pDrv->AddErrorFmt(_T("OPC读到标签<%s>异常值：值=%f, 时标=%s, 质量=0x%02X"), pi->m_szName, vtVal.fltVal, 
				tt.Format("%Y-%m-%d %H:%M:%S"), 
				(DWORD)wQuality);
		}
	}
#endif
	CRdRec rec;//值
	
	if(!Varint2Drvrec(vtVal,&rec, pi->m_bZeroFilter)) //转换值
		return;//转换不成功

	VariantCopy (&m_vtValue, &vtVal);//复制数据到m_vtValue
	m_wQuality = wQuality;

	m_bTimeStamped = TRUE;
	m_ftTimeStamp = ftTimeStamp;

	rec.SetName(pi->m_szName);//标签名

	long ltime = 0;

	if (pDrv->m_nOpcTime == 0) // 使用OPC时戳
	{
		SYSTEMTIME syst;
		if(FileTimeToSystemTime(&m_ftTimeStamp,&syst))
		{
			if((syst.wYear >= 1970)&&(syst.wYear < 2038))
			{
				CTime ct(syst) ;
				ltime = (long)ct.GetTime(); 
				ltime += pDrv->m_nTimeArea * 3600; //时区偏移
			//	TRACE("pDrv->m_nTimeArea = %d\n",pDrv->m_nTimeArea);
			}
		}
	}

	if (ltime <= 0)
	{
		ltime = (long)::time(NULL);  // 使用本地时戳
		pDrv->AddDebugFmt(_T("OPC读到标签<%s>：时标无效"), pi->m_szName);
	}

	rec.val.ltime = ltime; //时标
	rec.val.snerr = 0;
	/*
	if((wQuality & 0xFC) != OPC_QUALITY_GOOD_NON_SPECIFIC)
		rec.val.snqa = QA_ERR;//不可靠值
	else
		rec.val.snqa = QA_OK;//可靠值  数据质量
	*/
	//{{ heqing 2011/06/24
	rec.val.snqa = ToDBPQuality(wQuality);
	//}}

	//{{ 测试代码
	/*
	if (rec.val.snqa > 0)
	{
		COleVariant val = vtVal;
		val.ChangeType(VT_BSTR);

		USES_CONVERSION;
		pDrv->AddInfoFmt(_T("OPC读到标签<%s>：类型 = %u, 值 = %s, 时标 = %d, 质量=0x%02X -> %d"), pi->m_szName, (DWORD)vtVal.vt, W2A(val.bstrVal), ltime, (DWORD)wQuality, rec.val.snqa);
	}
	*/
	//}}
	pDrv->AddDrvRec(&rec); //加入待回调区
}

void	COpcDrv::AddDrvRec(CRdRec* prec) //添加待回调的数据记录,复制方式
{
	if(m_nMultiValSize >= MULTIMAXTAGS) //空间不够先回调
	{
		TRACE(_T("COpcDrv::AddDrvRec() 调用 DoMultiVal()\n"));

		DoMultiVal();
	}
	//复制名称
	m_recs[m_nMultiValSize].SetName(prec->sname);
	m_recs[m_nMultiValSize].ReSetType(DT_VOID);//清空类型
	m_recs[m_nMultiValSize].SetVal(&prec->val); //赋值
	m_nMultiValSize++;
}

void	COpcDrv::DoMultiVal()//不到4000的要发送
{
	TRACE(_T("COpcDrv::DoMultiVal() - thread id = 0x%02X\n"), GetCurrentThreadId());

	if(m_nMultiValSize> 0) 
	{
#if 0
		for (int i = 0; i < m_nMultiValSize; i++)
		{
			if ((strcmp(m_recs[i].sname, "REAL_MW") == 0) && ((m_recs[i].val.fval < 100.0) || (m_recs[i].val.fval > 400.0)))
			{
				CTime tf(m_recs[i].val.ltime);

				AddInfoFmt(_T("OPC上传标签<REAL_MW(%d, %d)>：值=%f, 时标=%s"), m_nMultiValSize, i , (double)m_recs[i].val.fval, 
					tf.Format("%Y-%m-%d %H:%M:%S"));
			}
		}
#endif
		if(m_cbOnDataOk)
			m_cbOnDataOk(m_handle,m_recs,m_nMultiValSize);
	}
	m_nMultiValSize=0;
}

// **************************************************************************
// GetValue ()
//
// Description:
//	Returns a string representation of the current value.
//
// Parameters:
//  CString		&strValue	Output string.
//
// Returns:
//  void
// **************************************************************************
void CKItem::GetValue (CString &strValue)
	{
	strValue="";
	// If item is active and valid, we can rely on server's data:
	if (IsValid ())
		{
		TCHAR szNum [32];

		// If data item is an array:
		if (m_vtValue.vt & VT_ARRAY)
			{
			// Format output string:
			switch (m_vtValue.vt)
				{
				// Supported types:
				case VT_UI1	| VT_ARRAY:
				case VT_I1	| VT_ARRAY:
				case VT_UI2	| VT_ARRAY:
				case VT_I2	| VT_ARRAY:
				case VT_UI4	| VT_ARRAY:
				case VT_I4	| VT_ARRAY:
				case VT_R4	| VT_ARRAY:
				case VT_R8	| VT_ARRAY:
					{
					// Get pointer to array data:
					CSafeArray *pSafeArr = (CSafeArray *) m_vtValue.parray;

					// Process array data if pointer appears to be good:
					if (pSafeArr)
						{
						// Get the array dimensions:
						DWORD dwCols = pSafeArr->GetNumCols ();
						DWORD dwSize = pSafeArr->GetByteLength ();
						ULONG cbElements = pSafeArr->cbElements;
						LPBYTE lpByte = (LPBYTE)pSafeArr->pvData;
						DWORD dwCol = 0;

						// Start delimiter out putput string:
						strValue = _T("[ ");

						// Cycle through the elements:
						for (DWORD i = 0; i < dwSize; i += cbElements, lpByte += cbElements)
							{
							// Format a string for each element according to data type:

							// Single-byte types:
							if (cbElements == 1)
								{
								if (m_vtValue.vt ==	(VT_UI1 | VT_ARRAY))
									_stprintf (szNum, _T("%u"), *lpByte);
								else
									_stprintf (szNum, _T("%d"), *(char *)lpByte);
								}
							
							// Two-byte types:
							else if (cbElements == 2)
								{
								if (m_vtValue.vt ==	(VT_UI2 | VT_ARRAY))
									_stprintf (szNum, _T("%u"), *(WORD *)lpByte);
								else 
									_stprintf (szNum, _T("%d"), *(short *)lpByte);
								}
							
							// Four-byte types:
							else if (cbElements == 4)
								{
								if (m_vtValue.vt ==	(VT_R4	| VT_ARRAY))
									_stprintf (szNum, _T("%f"), *(float *)lpByte);
								else if (m_vtValue.vt ==	(VT_UI4	| VT_ARRAY))
									_stprintf (szNum, _T("%u"), *(DWORD *)lpByte);
								else if (m_vtValue.vt ==	(VT_I4	| VT_ARRAY))
									_stprintf (szNum, _T("%d"), *(DWORD *)lpByte);
								}

							// Eight-byte types:
							else if (cbElements == 8)
								_stprintf (szNum, _T("%f"), *(double *)lpByte);

							// Else something is foobar!
							else
								{
								ASSERT (FALSE);
								}

							// Delimit each element within the row:
							if (dwCol != 0)
								strValue += _T(", ");

							// Append the formatted element data:
							strValue += szNum;

							// Terminate each row (except the last):
							if (++dwCol == dwCols)
								{
								if (i < dwSize - cbElements) 
									strValue += _T(" ] [ ");

								dwCol = 0;
								}
							}

						// End delimiter:
						strValue += _T(" ]");
						}

					// Else bad array data:
					else
						{
						strValue = _T("???");
//						LogMsg (IDS_BADITEMARRAYDATA, GetItemID ());
						}
					}
					break;

				default:
//					strValue = cStringContainer.m_lpszUnknown;
					break;
				}
			}
		
		// Else if data item is not an array:
		else
			{
			// Format output string according to data type:
			switch (m_vtValue.vt)
				{
				case VT_BOOL:
					wsprintf (szNum, _T("%d"), m_vtValue.boolVal ? 1 : 0);
					break;

				case VT_UI1:
					wsprintf (szNum, _T("%u"), m_vtValue.bVal);
					break;

				case VT_I1:
					wsprintf (szNum, _T("%d"), m_vtValue.cVal);
					break;

				case VT_UI2:
					wsprintf (szNum, _T("%u"), m_vtValue.uiVal);
					break;

				case VT_I2:
					wsprintf (szNum, _T("%d"), m_vtValue.iVal);
					break;

				case VT_UI4:
					wsprintf (szNum, _T("%u"), m_vtValue.ulVal);
					break;

				case VT_I4:
					wsprintf (szNum, _T("%d"), m_vtValue.lVal);
					break;

				case VT_R4:
					_stprintf (szNum, _T("%f"), m_vtValue.fltVal);
					break;

				case VT_R8:
					_stprintf (szNum, _T("%f"), m_vtValue.dblVal);
					break;

				case VT_BSTR:
//					strValue = m_vtValue.bstrVal; //不支持字符串
					return;

				default:

					return;
				}

			strValue = szNum;
			}
		}
	}


static const CLSID CATID_OPCDAServer10 = 
{ 0x63d5f430, 0xcfe4, 0x11d1, { 0xb2, 0xc8, 0x0, 0x60, 0x8, 0x3b, 0xa1, 0xfb } };


static const CLSID CATID_OPCDAServer20 = 
{ 0x63d5f432, 0xcfe4, 0x11d1, { 0xb2, 0xc8, 0x0, 0x60, 0x8, 0x3b, 0xa1, 0xfb } };

//--------------------------------------------------------------------------------------------------------------

//枚举OPC服务
#include "OpcEnum.h"

BOOL OPCServerList(LPCTSTR lpszIP,CString &szProgID)
{
	szProgID = "";

    CLSID clsid;
    clsid = CLSID_OpcServerList;    
    IOPCServerList *gpOPC = NULL;

    DWORD clsctx;
    clsctx = CLSCTX_LOCAL_SERVER;    
    
	COSERVERINFO tCoServerInfo;		//该结构只使用了pwszName成员

	ZeroMemory (&tCoServerInfo, sizeof (tCoServerInfo)); //情空服务器计算机信息结构

	// 分配计算机名空间,注意OPC都要使用UNICODE
	int nSize = strlen(lpszIP) * sizeof (WCHAR);
	tCoServerInfo.pwszName = new WCHAR [nSize];

	if (tCoServerInfo.pwszName == NULL)//判断计算机名UNICODE空间NEW是否成功
		return FALSE;

	MytoWchar(tCoServerInfo.pwszName,lpszIP);//转换为BSTR

	//填写需要获取的接口
	MULTI_QI arrMultiQI[1];
	arrMultiQI[0].pIID = &IID_IOPCServerList;
	arrMultiQI [0].pItf = NULL;
	arrMultiQI [0].hr = 0;

	HRESULT hr = CoCreateInstanceEx (
		clsid,										// CLSID
		NULL,										// No aggregation
		CLSCTX_SERVER,								// connect to local, inproc and remote servers
		&tCoServerInfo,								// remote machine name 
		1,	// number of IIDS to query		
		arrMultiQI);								// array of IID pointers to query
	delete [] tCoServerInfo.pwszName; //释放计算机名称UNICODE占用内存

	if (S_OK != hr)
	{
		TRACE("\nCoCreateInstanceEx Err\n");
		return FALSE;
	}

	gpOPC = (IOPCServerList *)arrMultiQI[0].pItf; //取得接口

    CLSID catid;    
    catid=CATID_OPCDAServer20;    
    IOPCEnumGUID *pEnumGUID;

    hr = gpOPC->EnumClassesOfCategories( 1, &catid, 1, &catid, (IEnumGUID**)&pEnumGUID); 
    if(S_OK != hr)
	{
		if (gpOPC)
			gpOPC->Release();

        return FALSE;
	}

   TRACE("\n--------------------\n");
    unsigned long c;
    while (S_OK == pEnumGUID->Next(1, &clsid, &c)) 
    {
        LPOLESTR pszProgID;
        LPOLESTR pszUserType;
        hr = gpOPC->GetClassDetails(&clsid, &pszProgID, &pszUserType);
       
        TRACE("ProgID = %ls, UserType = %ls\n", pszProgID, pszUserType);

		szProgID += pszProgID;
		szProgID += "\n";

        CoTaskMemFree(pszProgID);
        CoTaskMemFree(pszUserType);
    }
    TRACE("\n--------------------\n");

	if (pEnumGUID)
		pEnumGUID->Release();

    if(gpOPC)
        gpOPC->Release();

	return TRUE;
}

BOOL GetOPCServerClsID(LPCTSTR lpszIP,LPCTSTR lpszProgID,CLSID *pclsid) //使用IOPCServerList
{
    CLSID clsid;
    clsid = CLSID_OpcServerList;    
    IOPCServerList *gpOPC = NULL;

    DWORD clsctx;
    clsctx = CLSCTX_LOCAL_SERVER;    
    
	COSERVERINFO tCoServerInfo;		//该结构只使用了pwszName成员

	ZeroMemory (&tCoServerInfo, sizeof (tCoServerInfo)); //情空服务器计算机信息结构

	// 分配计算机名空间,注意OPC都要使用UNICODE
	int nSize = strlen(lpszIP) * sizeof (WCHAR);
	tCoServerInfo.pwszName = new WCHAR [nSize];

	if (tCoServerInfo.pwszName == NULL)//判断计算机名UNICODE空间NEW是否成功
		return FALSE;

	MytoWchar(tCoServerInfo.pwszName,lpszIP);//转换为BSTR

	//填写需要获取的接口
	MULTI_QI arrMultiQI[1];
	arrMultiQI[0].pIID = &IID_IOPCServerList;
	arrMultiQI [0].pItf = NULL;
	arrMultiQI [0].hr = 0;

	HRESULT hr = CoCreateInstanceEx (
		clsid,										// CLSID
		NULL,										// No aggregation
		CLSCTX_SERVER,								// connect to local, inproc and remote servers
		&tCoServerInfo,								// remote machine name 
		1,	// number of IIDS to query		
		arrMultiQI);								// array of IID pointers to query
	delete [] tCoServerInfo.pwszName; //释放计算机名称UNICODE占用内存

	if (S_OK != hr)
	{
		TRACE("\nCoCreateInstanceEx Err\n");
		return FALSE;
	}

	gpOPC = (IOPCServerList *)arrMultiQI[0].pItf; //取得接口

    CLSID catid;    
    catid=CATID_OPCDAServer20;    
    IOPCEnumGUID *pEnumGUID;

    hr = gpOPC->EnumClassesOfCategories( 1, &catid, 1, &catid, (IEnumGUID**)&pEnumGUID); 
	if(S_OK != hr)
	{
		if (gpOPC)
			gpOPC->Release();

		return FALSE;
	}

   TRACE("\n--------------------\n");
    unsigned long c;
	CString szproid;
    while (S_OK == pEnumGUID->Next(1, &clsid, &c)) 
    {
        LPOLESTR pszProgID;
        LPOLESTR pszUserType;
        hr = gpOPC->GetClassDetails(&clsid, &pszProgID, &pszUserType);
		
        TRACE("ProgID = %ls, UserType = %ls\n", pszProgID, pszUserType);
		szproid = pszProgID;

		if(szproid == lpszProgID)
		{
			memcpy(	pclsid,&clsid,sizeof(clsid));
			CoTaskMemFree(pszProgID);
			CoTaskMemFree(pszUserType);
			gpOPC->Release();
			return TRUE;
		}
        CoTaskMemFree(pszProgID);
        CoTaskMemFree(pszUserType);
    }
    TRACE("\n--------------------\n");
    
	if (pEnumGUID)
		pEnumGUID->Release();

    if(gpOPC)
        gpOPC->Release();

	return FALSE;
}

/*
BOOL GetOPCServerClsID(LPCTSTR lpszIP,LPCTSTR lpszProgID,CLSID *pclsid) //使用IOPCServerList2
{
    CLSID clsid;
    clsid = CLSID_OpcServerList;    
    IOPCServerList2 *gpOPC = NULL; //版本2

    DWORD clsctx;
    clsctx = CLSCTX_LOCAL_SERVER;    
    
	COSERVERINFO tCoServerInfo;		//该结构只使用了pwszName成员

	ZeroMemory (&tCoServerInfo, sizeof (tCoServerInfo)); //情空服务器计算机信息结构

	// 分配计算机名空间,注意OPC都要使用UNICODE
	int nSize = strlen(lpszIP) * sizeof (WCHAR);
	tCoServerInfo.pwszName = new WCHAR [nSize];

	if (tCoServerInfo.pwszName == NULL)//判断计算机名UNICODE空间NEW是否成功
		return FALSE;

	MytoWchar(tCoServerInfo.pwszName,lpszIP);//转换为BSTR

	//填写需要获取的接口
	MULTI_QI arrMultiQI[1];
	arrMultiQI[0].pIID = &IID_IOPCServerList;
	arrMultiQI [0].pItf = NULL;
	arrMultiQI [0].hr = 0;

	HRESULT hr = CoCreateInstance (
		clsid,
		NULL, 
		CLSCTX_SERVER, 
		IID_IOPCServerList2, 
		(void **)&gpOPC); 

	delete [] tCoServerInfo.pwszName; //释放计算机名称UNICODE占用内存

	if (S_OK != hr)
	{
		TRACE("\nCoCreateInstanceEx Err\n");
		return FALSE;
	}

    CLSID catid;    
    catid=CATID_OPCDAServer20;    
    IOPCEnumGUID *pEnumGUID;

    hr = gpOPC->EnumClassesOfCategories( 1, &catid, 1, &catid, (IOPCEnumGUID**)&pEnumGUID); 
    if(S_OK != hr)
        return FALSE;

   TRACE("\n--------------------\n");
    unsigned long c;
	CString szproid;
    while (S_OK == pEnumGUID->Next(1, &clsid, &c)) 
    {
        LPOLESTR pszProgID;
        LPOLESTR pszUserType;
		LPOLESTR pszVerIndProgID;
	
        
        hr = gpOPC->GetClassDetails(&clsid, &pszProgID, &pszUserType,&pszVerIndProgID);
		if (SUCCEEDED (hr)) 
		{
		    TRACE("ProgID = %ls, UserType = %ls,VerIndProgID = \n", pszProgID, pszUserType,pszVerIndProgID);
			szproid = pszProgID;

			if(szproid == lpszProgID)
			{
				memcpy(	pclsid,&clsid,sizeof(clsid));
				CoTaskMemFree(pszProgID);
				CoTaskMemFree(pszUserType);
				gpOPC->Release();
				return TRUE;
			}
			CoTaskMemFree(pszProgID);
			CoTaskMemFree(pszUserType);
			CoTaskMemFree(pszVerIndProgID);
		}
    }
    TRACE("\n--------------------\n");
    
    if(gpOPC)
        gpOPC->Release();

	return FALSE;
}
*/
/*
void CKServerGeneralPage::DisplayComponentCatList (HTREEITEM hParent, CATID catid) 
{ 
	HRESULT hr; 
	
	// Make sure COM is initialized: 
	hr = CoInitializeEx (NULL, COINIT_MULTITHREADED); 
	
	if (SUCCEEDED (hr)) 
	{ 
		IOPCServerList2 *pCat = NULL; 
		
		// Get component category manager:CLSID_StdComponentCategoriesMgr 
		hr = CoCreateInstance (CLSID_OPCServerList, 
			NULL, 
			CLSCTX_SERVER, 
			//IID_ICatInformation, 
			IID_IOPCServerList2, 
			(void **)&pCat); 
		
		// If succeeded, enumerate registered components: 
		if (SUCCEEDED (hr)) 
		{ 
			IOPCEnumGUID *pEnum = NULL; 
			
			CATID arrcatid [1]; 
			arrcatid [0] = catid; 
			
			// Enumerate registered components based on clsid: 
			hr = pCat->EnumClassesOfCategories ( 
				sizeof (arrcatid) / sizeof (CATID),//number of catids in the array that follows 
				arrcatid, // catid array 
				0, 
				NULL, 
				(IOPCEnumGUID**)&pEnum); // clsid enumerator for registered components under this category 
			
			// If succeeded, process results: 
			if (SUCCEEDED (hr)) 
			{ 
				GUID guid; 
				ULONG fetched; 
				
				// Loop over enumerated components. Call enemerator's next 
				// member function to reference next component and get its 
				// guid: 
				while ((hr = pEnum->Next (1, &guid, &fetched)) == S_OK) 
				{ 
					// Get the ProgID from the guid: 
					WCHAR *wszProgID; 
					hr = ProgIDFromCLSID (guid, &wszProgID); 
					
					// If succeeded, add component to list: 
					if (SUCCEEDED (hr)) 
					{ 
						// ProgID string will be in UNICODE format. Convert to 
						// ANSI format if this is and ANSI build. Insert component 
						// into list: 
#ifdef _UNICODE 
						m_pServerList->InsertItem (wszProgID, ILI_COMPONENT, ILI_COMPONENT, hParent); 
#else 
						TCHAR szProgID [DEFBUFFSIZE]; 
						
						_wcstombsz (szProgID, wszProgID, sizeof (szProgID) / sizeof (TCHAR)); 
						m_pServerList->InsertItem (szProgID, ILI_COMPONENT, ILI_COMPONENT, hParent); 
#endif 
						// It is up to us to free the Prog ID string memory: 
						CoTaskMemFree (wszProgID); 
					} 
				} 
				
				// Release our enumerator: 
				pEnum->Release (); 
			} 
			
			// release our category mamager 
			pCat->Release (); 
		} 
		
		// Uninitialize COM: 
		CoUninitialize (); 
	} 
	
}*/