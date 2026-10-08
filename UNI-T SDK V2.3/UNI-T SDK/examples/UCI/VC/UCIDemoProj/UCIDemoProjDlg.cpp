#include "stdafx.h"
#include "UCIDemoProj.h"
#include "UCIDemoProjDlg.h"
#include "afxdialogex.h"

#include "File.h"
#include "lib\UPO.h"
#include "lib\UTG4162.h"

using namespace uci;

#pragma comment(lib, "lib\\uci.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TCPIP_DISCONNECT  (WM_USER+2)
u_session g_curSession = -1;

// CUCIDemoProjDlg 对话框

DWORD g_di = GetTickCount();

void ResetTimer() {
	g_di = GetTickCount();
}

DWORD GetTimeSpan() {
	return GetTickCount() - g_di;
}

CUCIDemoProjDlg::CUCIDemoProjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUCIDemoProjDlg::IDD, pParent)
	, m_Val_Timeout(1000)
	, m_IsConnect(false)
	, m_Val_BytesToRead(4)
	, m_MaxCMDHistoryCount(20)
	, m_nConnectAddrPort(0)
	, m_cbBoxText(_T("")) {
}

void CUCIDemoProjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MSG, m_Ctrl_Edit_Msg);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_Val_Timeout);
	DDX_Text(pDX, IDC_EDIT_READ_BYTES, m_Val_BytesToRead);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_CtrlFileChoose);
	DDX_Control(pDX, IDC_COMBO_CMD, m_cbxCMD);
	DDX_Control(pDX, IDC_BTN_WRITE_IN_FILE, m_btnWriteParam_File);
	DDX_Control(pDX, IDC_COMBO_DEV_ADDR, m_cbxDevAddr);
	DDX_CBString(pDX, IDC_COMBO_CMD, m_cbBoxText);
	DDV_MaxChars(pDX, m_cbBoxText, 500);
}

CUCIDemoProjDlg* CUCIDemoProjDlg::c_object = nullptr;

BEGIN_MESSAGE_MAP(CUCIDemoProjDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_QUERY_NODES, &CUCIDemoProjDlg::OnBnClickedButtonQueryNodes)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CUCIDemoProjDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CUCIDemoProjDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BTN_WRITE, &CUCIDemoProjDlg::OnBnClickedBtnWrite)
	ON_BN_CLICKED(IDC_BTN_READ, &CUCIDemoProjDlg::OnBnClickedBtnRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_FILE, &CUCIDemoProjDlg::OnBnClickedButtonWriteFile)
	ON_BN_CLICKED(IDC_BUTTON_READ_FILE, &CUCIDemoProjDlg::OnBnClickedButtonReadFile)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_WRITE_IN_FILE, &CUCIDemoProjDlg::OnBnClickedBtnWriteInFile)
	ON_BN_SETFOCUS(IDC_BTN_WRITE_IN_FILE, &CUCIDemoProjDlg::OnBnSetfocusBtnWriteInFile)
	ON_MESSAGE(WM_TCPIP_DISCONNECT, &CUCIDemoProjDlg::OnLANDisconnect)
END_MESSAGE_MAP()

BOOL CUCIDemoProjDlg::OnInitDialog()

{
	auto r = CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);

	//{{initialize for The tips of widgets;
	m_tips.Create(this);
	m_tips.SetDelayTime(500);
	m_tips.SetTipTextColor(RGB(0, 0, 255));
	m_tips.SetTipBkColor(RGB(255, 255, 255));
	m_tips.Activate(true);
	m_tips.AddTool(GetDlgItem(IDC_BUTTON_OPEN), IDC_BUTTON_OPEN);
	m_tips.AddTool(GetDlgItem(IDC_BUTTON_QUERY_NODES), IDC_BUTTON_QUERY_NODES);
	m_tips.AddTool(GetDlgItem(IDC_EDIT_TIMEOUT), IDC_EDIT_TIMEOUT);
	//m_tips.AddTool(GetDlgItem(IDC_EDIT_DEV_ADDR), IDC_EDIT_DEV_ADDR);
	//m_tips.AddTool(GetDlgItem(IDC_EDIT_DEV_ADDR), IDC_EDIT_DEV_ADDR);
	m_tips.AddTool(GetDlgItem(IDC_COMBO_CMD), IDC_COMBO_CMD);
	m_tips.AddTool(GetDlgItem(IDC_MFCEDITBROWSE1), IDC_MFCEDITBROWSE1);
	m_tips.AddTool(GetDlgItem(IDC_EDIT_READ_BYTES), IDC_EDIT_READ_BYTES);
	m_tips.AddTool(GetDlgItem(IDC_BTN_READ), IDC_BTN_READ);
	m_tips.AddTool(GetDlgItem(IDC_BTN_WRITE), IDC_BTN_WRITE);
	m_tips.AddTool(GetDlgItem(IDC_BUTTON_WRITE_FILE), IDC_BUTTON_WRITE_FILE);
	m_tips.AddTool(GetDlgItem(IDC_BUTTON_READ_FILE), IDC_BUTTON_READ_FILE);
	m_tips.AddTool(GetDlgItem(IDC_BUTTON_CLEAR), IDC_BUTTON_CLEAR);
	//}}

	//{{Get the file path of saved the history of command
	GetAppFolderPath(m_strCMDListPath);
	m_strCMDListPath += _T("\\cmd.txt");
	//}}

	//{{Get app configuration
	m_Val_Timeout = theApp.GetProfileInt(_T("Params"), _T("Timeout"), 1000);
	m_Val_BytesToRead = theApp.GetProfileInt(_T("RParams"), _T("BYTES"), 50);
	CString addr = _T("[C:UPO][D:UPO2000CS][T:TCPIP][IP:192.168.1.172][Port:0x1388]");
	addr = theApp.GetProfileString(_T("Connect"), _T("Addr"), addr);
	m_COMType = theApp.GetProfileInt(_T("RParams"), _T("COMTYPE"), 50);
	//}}

	m_cbxDevAddr.AddString(addr);
	m_cbxDevAddr.SetCurSel(0);	

	InitUI();

	LoadCMDList();

	c_object = this;

	//{{ Config for UCI
	ELANGE lang = theApp.GetSystemLange();
	switch (lang) {
	case E_LANG_CHINESE:
		uci_SetAttribute(INVALID_SESSION, _T("lang:zh-Hans;devchange:1;"), nullptr, 0);
		break;
	case E_LANG_ENGLISH:
		uci_SetAttribute(INVALID_SESSION, _T("lang:en-US;devchange:1;"), nullptr, 0);
		break;
	default:
		break;
	}	
	uci_SetNotify(UCI_MessageProc);

//	m_cbxCMD.ModifyStyle(0, CBS_AUTOHSCROLL);
// 	auto dwStyle = m_cbxCMD.GetStyle();
// 	dwStyle |= CBS_AUTOHSCROLL;
// 	m_cbxCMD.ModifyStyle(0, dwStyle);
// 	m_cbxCMD.LimitText(0);
	

	//}}
	return r;
	//return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUCIDemoProjDlg::InitUI() {
	//{{UI initialization
	CheckDlgButton(IDC_CHECK_LAN, m_COMType & uci::NodeType::LAN);
	CheckDlgButton(IDC_CHECK_USB, m_COMType & uci::NodeType::USB);
	EnableWidgets(FALSE);
	m_btnWriteParam_File.SetDelayFullTextTooltipSet(FALSE);
	UpdateData(FALSE);
	//}
}

void CUCIDemoProjDlg::LoadCMDList() {
	try {
		CStdioFile f(m_strCMDListPath, CFile::modeRead | CFile::typeText);

		CString s;
		while (f.ReadString(s) && m_cbxCMD.GetCount() <= m_MaxCMDHistoryCount) {		
			if (m_cbxCMD.GetCount() == 0) {
				m_cbxCMD.AddString(s);
			} else {
				m_cbxCMD.InsertString(0, s);
			}
		}
	} catch (CException* e) {
		m_cbxCMD.AddString(_T("CH:0@SEL;"));
		m_cbxCMD.AddString(_T("CH:0@EN:1;"));
		m_cbxCMD.AddString(_T("CH:0@vp:128;"));
		m_cbxCMD.AddString(_T("trig@pos:25;"));
		m_cbxCMD.AddString(_T("math@t:f;"));
		e->Delete();
	}
}

bool CUCIDemoProjDlg::GetAppFolderPath(CString& _strPath) {
	TCHAR path[MAX_PATH];
	ZeroMemory(path, sizeof(path));
	auto r = GetModuleFileName(AfxGetInstanceHandle(), path, MAX_PATH);
	if (r == 0) {
		ASSERT(false);
		return false;
	}

	CString strAppPath(path);
	auto i = strAppPath.ReverseFind(_T('\\'));
	if (i == -1) {
		ASSERT(false);
		return false;
	}

	_strPath = strAppPath.Left(i);
	//AfxMessageBox(_strPath);
	return true;
}

void CUCIDemoProjDlg::EnableWidgets(BOOL _isConnect) {
	GetDlgItem(IDC_BTN_WRITE)->EnableWindow(_isConnect);
	GetDlgItem(IDC_BTN_READ)->EnableWindow(_isConnect);
	GetDlgItem(IDC_BUTTON_WRITE_FILE)->EnableWindow(_isConnect);
	GetDlgItem(IDC_BUTTON_READ_FILE)->EnableWindow(_isConnect);
	GetDlgItem(IDC_BTN_WRITE_IN_FILE)->EnableWindow(_isConnect);
	//GetDlgItem(IDC_CHECK_WITE_FILE)->EnableWindow(_isConnect);
}

void CUCIDemoProjDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

HCURSOR CUCIDemoProjDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUCIDemoProjDlg::GetCMDString(CString& _msg) {
	GetDlgItemText(IDC_COMBO_CMD, _msg);
}

int CUCIDemoProjDlg::UCIWrite(const TCHAR* _cmd, UINT _timeOut) {
	WParams wp;
	ZeroMemory(&wp, sizeof(wp));	
	//You can also use the simple interface "uci_WriteX", defined in uci.h
	auto r =  uci_Write(g_curSession, uci_CreateWParams(wp, _cmd, _timeOut), nullptr, 0);
	if (r >= 0)
		RecordCMD(_cmd);
	return r;
}

int CUCIDemoProjDlg::UCIRead(const TCHAR* _cmd, u_byte* _data, u_size _dataLen, UINT _timeOut) {
#if 0
	//You can also use the simple interface "uci_ReadX", defined in uci.h
	RParams rp;
	auto r = uci_Read(g_curSession, uci_CreateRParams(rp, _cmd, _timeOut, NULL, 0), _data, _dataLen);
#else	
	auto r = uci_ReadX(g_curSession, _cmd, _timeOut, _data, _dataLen);
#endif	
	if (r >= 0)
		RecordCMD(_cmd);
	return r;
}

int __stdcall CUCIDemoProjDlg::UCI_MessageProc(UCIMSG* _msg) {
	if (_msg == nullptr) {
		ASSERT(FALSE);
		return 0;
	}

	switch (_msg->Message) {
		case uci::EUCIMSG::UMSG_DEVICE_NOTIFY:
		{
#if 1
			//if (c_object->m_DevName.CompareNoCase(CString(_msg->Reserved)) == 0) {
				if (c_object->m_IsConnect && g_curSession != Invalid_Session) {
					c_object->ShowResult(uci_Close(g_curSession), IDS_TIPS_CLOSE_CONNECT);
					c_object->SetWidgetText(IDC_BUTTON_OPEN, IDS_BTN_CONNECT);
					c_object->m_IsConnect = false;
				}
			//}
#endif
		}
			break;
		case uci::EUCIMSG::UMSG_CONNECT_CLOSED:
			//TCIPIP连接的断开事件触发时，是线程调用该接口，所以在此接口中不能更新UI
			//This event can been invoked when the disconnection of tcpip, and it been call in thread, you can not update UI here;
			break;
		default:
			break;
	}

	return 1;
}

void CUCIDemoProjDlg::Disconnect() {
	if (m_IsConnect && g_curSession != Invalid_Session) {	
			ShowResult(uci_Close(g_curSession), IDS_TIPS_CLOSE_CONNECT);		
			SetWidgetText(IDC_BUTTON_OPEN, IDS_BTN_CONNECT);
			m_IsConnect = false;
	}
}


//@brief             : recode cmd for input history
//@param _msg : 
//@return            : 
//@remarks           : 
void CUCIDemoProjDlg::RecordCMD(const CString& _msg) {
	// Delete all items that begin with the specified string. 
	int nItem = 0;
	while ((nItem = m_cbxCMD.FindString(nItem, _msg)) != CB_ERR) {
		m_cbxCMD.DeleteString(nItem);
	}

	while (m_cbxCMD.GetCount() > m_MaxCMDHistoryCount) {
		m_cbxCMD.DeleteString(m_cbxCMD.GetCount() - 1);
	}

	m_cbxCMD.InsertString(0, _msg);
	m_cbxCMD.SetWindowText(_msg);
}

void CUCIDemoProjDlg::ShowResult(int _r, int _idsSOerationString, const TCHAR* _format, ...)
{
	CString msg;
	if (_format != nullptr)
	{
		va_list argptr;
		va_start(argptr, _format);
		msg.FormatV(_format, argptr);
		va_end(argptr);

	}	
	if (_idsSOerationString != -1)
		msg = LoadString(_idsSOerationString) + msg;

	CString str;
	if (_r < 0) {
		str.Format(_T("\r\n[%s], Error id = 0x%x(hex) , %d(dec);\r\n\t->%s;"), msg, _r, _r, uci_GetLastError());
	} else {
		str.Format(_T("\r\n[%s], return = %d, Succeed!, use time = %d ms;\r\n"), msg, _r, GetTimeSpan());
	}

	ASSERT(IsWindow(m_Ctrl_Edit_Msg.GetSafeHwnd()));
	int indexEnd = m_Ctrl_Edit_Msg.GetWindowTextLength(), index = indexEnd;
	m_Ctrl_Edit_Msg.SetSel(index, indexEnd);
	m_Ctrl_Edit_Msg.ReplaceSel(str);
}

void CUCIDemoProjDlg::AppendTraceMsg(const u_tchar *format, ...) {
	CString msg;
	va_list argptr;
	va_start(argptr, format);
	msg.FormatV(format, argptr);
	va_end(argptr);

	int indexEnd = m_Ctrl_Edit_Msg.GetWindowTextLength(), index = indexEnd;
	m_Ctrl_Edit_Msg.SetSel(index, indexEnd);
	m_Ctrl_Edit_Msg.ReplaceSel(msg);
}

void CUCIDemoProjDlg::OnBnClickedButtonClear() {	
	m_Ctrl_Edit_Msg.SetWindowText(_T(""));
}

void CUCIDemoProjDlg::OnBnClickedButtonQueryNodes() {
	UpdateData();
	if (m_Val_Timeout == 0) {
		AfxMessageBox(IDS_TIPS_TIMEOUT_ZERO);
		return;
	}

	CWaitCursor wc;

	//*******************************delete device addr list:**************************
	ResetTimer();
	while (m_cbxDevAddr.GetCount() > 0) {
		m_cbxDevAddr.DeleteString(m_cbxDevAddr.GetCount() - 1);
	}
		
	CString strResult;
	//*******************************query devices:**************************	
	//example1: uci_QueryNodes
#if 0
	QParams qp;

	int pvids[] = {
		MakePVID(0x1234, 0x5345),
		MakePVID(0x7777, 0x5345),
		MakePVID(0x0834, 0x5656),
		MakePVID(0x5537, 0x4348),
		MakePVID(0xE008, 0x1A86),
		MakePVID(0xEA80, 0x10C4),
	};

	int ports[] = {
		4162,
		5000,
	};

	//int type = NodeType::LAN | NodeType::USB;
	int type = 0;
	if (IsDlgButtonChecked (IDC_CHECK_USB))
		type |= NodeType::USB;
	
	if (IsDlgButtonChecked(IDC_CHECK_LAN))
		type |= NodeType::LAN;

	if (type == 0)
	{
		AfxMessageBox(_T("请选择要查询的通信接口类型！"));
		return;
	}
	UCI_CreateQParam(qp, type, ports, sizeof(ports) / sizeof(int), pvids, sizeof(pvids) / sizeof(int), nullptr);

	Node nodes[10];
	ZeroMemory(nodes, sizeof(nodes));
	u_size notesCNT = sizeof(nodes) / sizeof(nodes[0]);

	int r = uci_QueryNodes(&qp, nodes, &notesCNT, m_Val_Timeout);
#else
	//example2: uci_QueryNodesX
	//
	CString strQuery;

	//int type = NodeType::LAN | NodeType::USB;
	int type = 0;
	if (IsDlgButtonChecked(IDC_CHECK_USB)) {
		type |= NodeType::USB;		
		//+ DSO,SG,POWER
		strQuery.AppendFormat(_T("USB:0x1234&0x5345,0x7777&0x5345,0x0834&0x5656,0x5537&0x4348"));
		//+ DM(HID)
		strQuery.AppendFormat(_T(",0xE008&0x1A86,0xEA80&0x10C4;"));
	}		
	
	if (IsDlgButtonChecked(IDC_CHECK_LAN)) {
		type |= NodeType::LAN;		
		//8000:UTG2000A series; 5000: UPO2000S/UPO7000Z series; 4162 : UTG4000A series		
		strQuery.AppendFormat(_T("LAN:5000,4162,8000,18191;"));
	}		
	m_COMType = type;
	
	ZeroMemory(m_Nodes, sizeof(m_Nodes));
	u_size notesCNT = sizeof(m_Nodes) / sizeof(m_Nodes[0]);

	int r = uci_QueryNodesX(strQuery, m_Nodes, notesCNT, m_Val_Timeout);
#endif

	if (r < 0) {
		ShowResult(r, IDS_QUERY_DEVICES_FAILED);// Error to query nodes
		return;
	}

	if (notesCNT == 0) {
		AfxMessageBox(IDS_TIPS_NOT_FOUND_ANY_DEVICES);
		return;
	}

	//*******************************show result:**************************
	notesCNT = r;
	for (size_t i = 0; i < notesCNT; i++) {
		switch (m_Nodes[i].Type) {
			case NodeType::LAN:
			{
				auto n = &m_Nodes[i].LAN;
				TRACE(_T("\r\nIP : %s, Port : %d"), CString(n->IP), n->Port);
				strResult.AppendFormat(_T("\r\nIP : %s, Port : %d"), CString(n->IP), n->Port);
				break;
			}
			case NodeType::USB:
			{
				auto n = &m_Nodes[i].USB;
				TRACE(_T("\r\nPID : 0x%x, VID : 0x%x, Addr : %d"), n->PID, n->VID, n->Addr);
				strResult.AppendFormat(_T("\r\nPID : %d, VID : %d, Addr : %d"), n->PID, n->VID, n->Addr);
				break;
			}
			default:
				ASSERT(FALSE);
				break;
		}

		strResult.AppendFormat(_T(",%s,SN:%s, status = %d, IDN = %s"), 
			m_Nodes[i].Name, m_Nodes[i].SN, m_Nodes[i].Status, m_Nodes[i].IDN);
		strResult.AppendFormat(_T("\r\n\t->UCI Addr: %s"), m_Nodes[i].UCIAddr);

		m_cbxDevAddr.AddString(m_Nodes[i].UCIAddr);
	}

	if (!strResult.IsEmpty ())
		AppendTraceMsg(_T("\r\n--------QUERY Result------------%s"), strResult);
	ShowResult(r, -1, _T("Query"));

	AppendTraceMsg(_T("\r\n--------------------"));
	AppendTraceMsg(LoadString (IDS_TIPS_COPY_UCI_ADDR));
	AppendTraceMsg(_T("\r\n--------------------\r\n"));
	
	//SetDlgItemText(IDC_EDIT_DEV_ADDR, nodes[0].UCIAddr);
	m_cbxDevAddr.SetWindowText(m_Nodes[0].UCIAddr);
}

LRESULT AFX_MSG_CALL CUCIDemoProjDlg::OnLANDisconnect(WPARAM wParam, LPARAM lParam) {
	Disconnect();
	return 1;
}

void CUCIDemoProjDlg::OnBnClickedButtonOpen() {
	ResetTimer();

	//{UI
	if (m_IsConnect && g_curSession != INVALID_SESSION) {
		ShowResult(uci_Close(g_curSession), IDS_TIPS_CLOSE_CONNECT);
		SetWidgetText(IDC_BUTTON_OPEN, IDS_BTN_CONNECT);
		m_IsConnect = false;
		EnableWidgets(FALSE);
		return;
	}
	//}

	CString msg;
	m_cbxDevAddr.GetWindowText(msg);
	if (msg == _T("")) {
		AfxMessageBox(LoadString(IDS_CMD_MSG_EMPTY));
		return;
	}

	auto r = uci_Open(msg, &g_curSession, 2000);
	ShowResult(r, IDS_TIPS_BUILD_COONNECTION);
	m_IsConnect = UCISUCCESS(r);

	if (m_IsConnect) {
		//{UI 
		SetWidgetText(IDC_BUTTON_OPEN, IDS_BTN_DISCONNECT);
		EnableWidgets(TRUE);
		//}

		//{record the IDN string
		m_IDN.Empty();
		for (int count = 0; count < sizeof(m_Nodes) / sizeof(m_Nodes[0]); count++) {
			if (msg.CompareNoCase(m_Nodes[count].UCIAddr) == 0)
				m_IDN = m_Nodes[count].IDN;
		}
		//}

		//{parse Fields of UCI Address
		m_DevName = FindConnectAddrField(msg, _T("[D:"), _T("]"));
		auto comType = FindConnectAddrField(msg, _T("[T:"), _T("]"));
		if (comType.CompareNoCase(_T("TCPIP")) == 0) {
			m_ConnectAddrIP = FindConnectAddrField(msg, _T("[IP:"), _T("]"));
			m_ConnectAddrPort = FindConnectAddrField(msg, _T("[PORT:"), _T("]"));
			m_nConnectAddrPort = _tcstol(m_ConnectAddrPort, nullptr, m_ConnectAddrPort.Find(_T("0x")) == -1 ? 10 : 16);
		}
		//}

		CString tips;
		tips.Format(_T("winmsg:0x%x@closed:%d;"), GetSafeHwnd(), WM_TCPIP_DISCONNECT);
		uci_SetAttribute(g_curSession, tips, nullptr, 0);
	}

	//save the UCI Address for next startup
	theApp.WriteProfileString(_T("Connect"), _T("Addr"), msg);	
}

CString CUCIDemoProjDlg::FindConnectAddrField(const CString& _addr,
	const CString& _fieldBegin, const CString&_fieldEnd) {
	CString addr(_addr), begin(_fieldBegin), end(_fieldEnd);
	addr.MakeUpper();
	begin.MakeUpper();
	end.MakeUpper();
	auto i = addr.Find(_fieldBegin);
	auto iEnd = addr.Find(_fieldEnd, i);
	return addr.Mid(i + _fieldBegin.GetLength(), iEnd - i - _fieldBegin.GetLength());
}

void CUCIDemoProjDlg::OnBnClickedBtnWrite() {	
	if (!CheckConnectiion())
		return;

	UpdateData();

	CString msg;
	GetCMDString(msg);
	if (msg.IsEmpty()) {
		AfxMessageBox(IDS_CMD_MSG_EMPTY);
		return;
	}

	ResetTimer();

	ShowResult(UCIWrite(msg, m_Val_Timeout), IDS_TIPS_CMD_WRITE, msg);
}

void CUCIDemoProjDlg::OnBnClickedBtnWriteInFile() {
	if (!CheckConnectiion())
		return;

	UpdateData();

	if (!GetCommandString ())
		return;

	CString strPath;
	m_CtrlFileChoose.GetWindowText(strPath);
	if (strPath.IsEmpty()) {
		AfxMessageBox(_T("文件路径不能为空！"));
		return;
	}

	strPath.Trim(_T(" "));

	if (!::PathFileExists(strPath)) {
		AfxMessageBox(_T("文件路径错误！"));
		return;
	}

	ResetTimer();

	try {
		CFile f(strPath, CFile::modeRead);

		std::vector<byte> buf((size_t)f.GetLength(), 0);

		f.Read(buf.data(), buf.size());	

		WParams wp;
		ZeroMemory(&wp, sizeof(wp));
		auto r = uci_Write(g_curSession, uci_CreateWParams(wp, m_CommandString, m_Val_Timeout), buf.data(), buf.size());
		if (UCISUCCESS(r))
			RecordCMD(m_CommandString);

		ShowResult(r, IDS_TIPS_CMD_WRITE_FILE_AS_WRITE_PARAMS, m_CommandString);
	} catch (CException* e) {
		e->ReportError();
		e->Delete();
	}
}

bool CUCIDemoProjDlg::CheckConnectiion() {
	if (g_curSession == Invalid_Session || !m_IsConnect) {
		AfxMessageBox(IDS_TIPS_NOT_CONNECT);
		return false;
	}
	return true;
}

void CUCIDemoProjDlg::OnBnClickedBtnRead() {
	SetDlgItemText(IDC_EDIT_MSG, _T(""));

	UpdateData(TRUE);

	auto sz = sizeof(upo2k::CellBaseValue);

	if (m_Val_BytesToRead < 2) {
		AfxMessageBox(_T("要读取的字节数，最少2Bytes<CEX>, 4Bytes<UPO2000CS>！"));
		return;
	}

	if (m_Val_Timeout == 0) {
		AfxMessageBox(_T("超时时间不能为0！"));
		return;
	}

	theApp.WriteProfileInt(_T("RParams"), _T("BYTES"), m_Val_BytesToRead);
	theApp.WriteProfileInt(_T("Params"), _T("Timeout"), m_Val_Timeout);

	if (g_curSession == Invalid_Session || !m_IsConnect) {
		AfxMessageBox(_T("请先建立连接！"));
		return;
	}

	//"USB BIN"通信方式msg不再需要！ 
	CString msg;
	GetCMDString(msg);
	if (msg == _T("")) {
		AfxMessageBox(_T("命令字符串不能为空！"));
		return;
	}

	std::vector<byte>  buf;
	try {
		buf.resize(m_Val_BytesToRead, 0);
	} catch (CException* e) {
		e->ReportError();
		e->Delete();
		return;
	}

	ResetTimer();

	//读取数据
	auto r = UCIRead(msg, &buf[0], buf.size(), m_Val_Timeout);
	ShowResult(r, IDS_TIPS_CMD_READ);
	if (UCIERR(r))
		return;

	//对截图指令获得的截图文件进行重命名
	CString path, fileName = _T("RData.dat");
	fileName = CreateBmpFileName(msg, fileName);
	if (r > m_Val_BytesToRead)
		r = m_Val_BytesToRead;

	//缓冲原始数据到本地文件
	Files::WriteAllBytes(fileName, &buf[0], r, &path);
	AppendTraceMsg(_T("The received data had been stored in file：\r\n -> %s"), path);
	
	//二进制输出一部分
	auto pBin = reinterpret_cast<byte*>(&buf[0]);
	size_t szBin = buf.size() > 100 ? 100 : buf.size();
	CString s, str;
	size_t line_count = 10;
	for (size_t i = 0; i < szBin; i++) {
		if (i > 0 && i % line_count == 0) {
			s += _T("| ") + str;
			str.Empty();
			s.AppendFormat(_T("\r\n"));
		}
		s.AppendFormat(_T("%02x\t"), pBin[i]);
		str.AppendFormat(_T("%c"), pBin[i] == 0 ? '.' : pBin[i]);
	}
	//if (szBin < line_count) {
		s += _T("| ") + str;
	//}

	AppendTraceMsg(_T("\r\n----------------------------"));
	AppendTraceMsg(_T("\r\nBinary Data(%dBytes) : \r\n%s"), szBin, s);

	AppendTraceMsg(_T("\r\n----------------------------"));
	AppendTraceMsg(_T("\r\nTry parse build-in struct："));
	
	//对常用数据进行解析并显示到窗口
	auto szV = buf.size();
	switch (szV) {
		case 2:
		{
			auto pb_short = reinterpret_cast<short*>(&buf[0]);
			auto pb_ushort = reinterpret_cast<USHORT*>(&buf[0]);
			auto pb_byte = reinterpret_cast<short*>(pb_ushort);
			AppendTraceMsg(_T("\r\nShort = 0x%x(%d), UShort = 0x%x(%d)  -> L-Word(%d) H-Word(%d)"),
				*pb_short, *pb_short, *pb_ushort, *pb_ushort, pb_ushort[0], pb_ushort[1]);
			break;
		}			
		case 4:
		{
			auto pb = reinterpret_cast<int*>(&buf[0]);
			auto pb_uint = reinterpret_cast<UINT*>(&buf[0]);
			auto pb_short = reinterpret_cast<short*>(pb);
			AppendTraceMsg(_T("\r\nInt = 0x%x(%d), UInt = 0x%x(%d) -> L-Word(%d) H-Word(%d)"), 
				*pb, *pb, *pb_uint, *pb_uint, pb_short[0], pb_short[1]);

			auto pf = reinterpret_cast<float*>(&buf[0]);		
			AppendTraceMsg(_T("\r\nfloat = %f"), *pf);
			break;
		}		

		case 8:{
			auto pb = reinterpret_cast<int*>(&buf[0]);
			auto pb_int_64 = reinterpret_cast<long long*>(&buf[0]);
			auto pb_uint_64 = reinterpret_cast<unsigned long long*>(pb);			
			auto pb_double = reinterpret_cast<double*>(pb);
			double v = *pb_double;
			AppendTraceMsg(_T("\r\nInt64 = 0x%x(%d), UInt64 = 0x%x(%u)"),
				*pb_int_64, *pb_int_64, *pb_uint_64, *pb_uint_64);		
			AppendTraceMsg(_T("\r\ndouble = %f"), *pb_double);
			break;
		}
		default:
			break;
	}

	AppendTraceMsg(_T("\r\n----------------------------"));
	AppendTraceMsg(_T("\r\nTry parse："));
	if (buf.size() == sizeof(upo2k::CellBaseValue)) {
		upo2k::CellBaseValue * pb = reinterpret_cast<upo2k::CellBaseValue*>(&buf[0]);
		AppendTraceMsg(_T("\r\nV = %f\r\nU = %d(type = %d, code = %d)\r\nIsLimit = %d"),
			pb->Value, pb->Unit, upo2k::GetUnitType(pb->Unit), upo2k::GetUnitCode(pb->Unit), pb->IsLimit);
		if (buf.size() == sizeof(double)) {
			AppendTraceMsg(_T("\r\n----"));
			AppendTraceMsg(_T("\r\nV = %f,  %d"), *(reinterpret_cast<double*>(&buf[0])), *(reinterpret_cast<LONGLONG *>(&buf[0])));
		}
	} else if (buf.size() == sizeof(upo2k::CellBaseValue) * 2) {
		upo2k::CellBaseValue * pb = reinterpret_cast<upo2k::CellBaseValue*>(&buf[0]);
		AppendTraceMsg(_T("\r\n[Low]V = %f,U = %d,IsLimit = %d;\r\n[High]V = %f,U = %d,IsLimit = %d"),
			pb->Value, pb->Unit, pb->IsLimit, pb[1].Value, pb[1].Unit, pb[1].IsLimit);
	}else if (buf.size() == sizeof(utg4162::LEDStatus)) {
		auto pb = reinterpret_cast<utg4162::LEDStatus*>(&buf[0]);
		AppendTraceMsg(_T("\r\n[LED] CH1 = %d, CH2 = %d, Sine = %d"), pb->CH1, pb->CH2, pb->SINE);
	} else if (buf.size() == sizeof(upo2k::UValue)) {
		upo2k::UValue * pb = reinterpret_cast<upo2k::UValue*>(&buf[0]);
		AppendTraceMsg(_T("\r\nV = %f\r\nU = %d(0x%x)"), pb->value, pb->unit, pb->unit);
	} else if (buf.size() == sizeof(upo2k::CursorMeasurePack)) {
		auto pb = reinterpret_cast<upo2k::CursorMeasurePack*>(&buf[0]);
		AppendTraceMsg(_T("\r\nAx : V = %f,U = %d[0x%x  t(0x%x)  c(%d)]"),
			pb->Ax.value, pb->Ax.unit, pb->Ax.unit, upo2k::GetUnitType(pb->Ax.unit), upo2k::GetUnitCode(pb->Ax.unit));
		AppendTraceMsg(_T("\r\nAy : V = %f,U = %d[0x%x  t(0x%x)  c(%d)]"),
			pb->Ay.value, pb->Ay.unit, pb->Ay.unit, upo2k::GetUnitType(pb->Ay.unit), upo2k::GetUnitCode(pb->Ay.unit));
	} else if (buf.size() == sizeof(upo2k::Time)) {
		auto pt = reinterpret_cast<upo2k::Time*>(&buf[0]);
		AppendTraceMsg(_T("\r\nTime: %d-%d-%d, %d:%d:%d"),
			pt->Year, pt->Month, pt->Day, pt->Hour, pt->Minute, pt->Second);
	}

	if (msg.Find(_T("mea:all?")) != -1) {
		ParseParamsMeasure(buf.data(), buf.size());
	} else if (msg.Compare(_T(":MEA:All?")) == 0) {
		ParseParamsMeasure(buf.data() + 8 , buf.size() - 8);
	} else if (msg.Find(_T("mea:user?")) != -1) {
		ParseParamsMeasure_User(buf.data(), buf.size());
	}
}

const CString& CUCIDemoProjDlg::CreateBmpFileName(CString &msg, CString& fileName) {
	if (msg.CompareNoCase(_T("PrtScn:bmp;")) == 0 || msg.CompareNoCase(_T("PrtScn:.bmp;")) == 0) {
		if (!m_IDN.IsEmpty())
			fileName = m_IDN;
		else
			fileName = m_DevName;
		fileName += _T("_");
		fileName += GetCurrentTime();
		fileName += ".bmp";
		Sleep(300);
	}
	return fileName;
}

void CUCIDemoProjDlg::ParseParamsMeasure(byte* _data, int _data_len) {
	float v = 12.0;
	float * pv = &v;
	cb::MeaValue* pmv = reinterpret_cast<cb::MeaValue*>(_data);
	AppendTraceMsg(_T("\r\n"));
	PrintMeaParam(_T("MAX"), pmv[cb::MP_MAX]);
	PrintMeaParam(_T("MIN"), pmv[cb::MP_MIN]);
	PrintMeaParam(_T("HIGH"), pmv[cb::MP_HIGH]);
	PrintMeaParam(_T("MIDDLE"), pmv[cb::MP_MIDDLE]);
	PrintMeaParam(_T("LOW"), pmv[cb::MP_LOW]);
	PrintMeaParam(_T("PKPK"), pmv[cb::MP_PKPK]);
	PrintMeaParam(_T("AMP"), pmv[cb::MP_AMP]);
	PrintMeaParam(_T("MEAN"), pmv[cb::MP_MEAN]);
	PrintMeaParam(_T("CYCMEAN"), pmv[cb::MP_CYCMEAN]);
	PrintMeaParam(_T("RMS"), pmv[cb::MP_RMS]);

	PrintMeaParam(_T("CYCRMS"), pmv[cb::MP_CYCRMS]);
	PrintMeaParam(_T("AREA"), pmv[cb::MP_AREA]);
	PrintMeaParam(_T("CYCAREA"), pmv[cb::MP_CYCAREA]);
	PrintMeaParam(_T("OVERSHOOT"), pmv[cb::MP_OVERSHOOT]);
	PrintMeaParam(_T("PRESHOOT"), pmv[cb::MP_PRESHOOT]);
	PrintMeaParam(_T("PERIOD"), pmv[cb::MP_PERIOD]);
	PrintMeaParam(_T("FREQ"), pmv[cb::MP_FREQ]);

	PrintMeaParam(_T("RISE_TIME"), pmv[cb::MP_RISE_TIME]);
	PrintMeaParam(_T("FALL_TIME"), pmv[cb::MP_FALL_TIME]);
	PrintMeaParam(_T("PWIDTH"), pmv[cb::MP_PWIDTH]);
	PrintMeaParam(_T("NWIDTH"), pmv[cb::MP_NWIDTH]);
	PrintMeaParam(_T("PDUTY"), pmv[cb::MP_PDUTY]);
	PrintMeaParam(_T("NDUTY"), pmv[cb::MP_NDUTY]);
	PrintMeaParam(_T("RISEDELAY"), pmv[cb::MP_RISEDELAY]);

	PrintMeaParam(_T("FALLDELAY"), pmv[cb::MP_FALLDELAY]);
	PrintMeaParam(_T("PHASE"), pmv[cb::MP_PHASE]);
	PrintMeaParam(_T("FRR"), pmv[cb::MP_FRR]);
	PrintMeaParam(_T("FRF"), pmv[cb::MP_FRF]);

	PrintMeaParam(_T("FFR"), pmv[cb::MP_FFR]);
	PrintMeaParam(_T("FFF"), pmv[cb::MP_FFF]);
	PrintMeaParam(_T("LRF"), pmv[cb::MP_LRF]);
	PrintMeaParam(_T("LRR"), pmv[cb::MP_LRR]);
	PrintMeaParam(_T("LFR"), pmv[cb::MP_LFR]);
	PrintMeaParam(_T("LFF"), pmv[cb::MP_LFF]);

	PrintMeaParam(_T("BURST_WIDTH"), pmv[cb::MP_BURST_WIDTH]);
}

void CUCIDemoProjDlg::ParseParamsMeasure_User(byte* _data, int _data_len) {
	if (_data_len != (sizeof(cb::MeaValue) * 5))
		return;

	cb::MeaValue* pmv = reinterpret_cast<cb::MeaValue*>(_data);
	
	for (size_t i = 0; i < 5; i++) {
		AppendTraceMsg(_T("\r\nIsExist=%d,IsValid=%d,Value=%f, Unit(Type=%d,Scale=%d)"), 
			pmv->IsExist, pmv->IsValid, pmv->Value, pmv->Unit.Type, pmv->Unit.Scale);
		pmv++;
	}
}

void CUCIDemoProjDlg::PrintMeaParam(const TCHAR * _name, const comAPICommon::MeaValue& _p) {
	AppendTraceMsg(_T("%s\t\t\t=\t"), _name);
	if (_p.IsExist) {
		if (_p.IsValid)
			AppendTraceMsg(_T("%f "), _p.Value);
		else
			AppendTraceMsg(_T("--"));
	} else {
		AppendTraceMsg(_T("NotExit"));
	}

	AppendTraceMsg(_T(" %s%s\n"), unit::uci_UnitFindScaleName(_p.Unit.Scale),
		CString(unit::uci_UnitFindTypeName(_p.Unit.Type)));
}

bool CUCIDemoProjDlg::GetCommandString() {
	m_CommandString.Empty();
	GetCMDString(m_CommandString);
	if (m_CommandString.IsEmpty()) {
		AfxMessageBox(IDS_CMD_MSG_EMPTY);//_T("The command string cannot be empty!")
		return false;
	}
	return true;
}

bool CUCIDemoProjDlg::GetFilePath() {
	m_Path.Empty();
	m_CtrlFileChoose.GetWindowText(m_Path);
	if (m_Path.IsEmpty()) {
		AfxMessageBox(IDS_TIPS_PATH_EMPTY);//The path cannot be empty！
		return false;
	}
	return true;
}

void CUCIDemoProjDlg::OnBnClickedButtonWriteFile() {
	UpdateData();

	ResetTimer();

	if (!GetCommandString())
		return;

	if (!GetFilePath())
		return;

	if (!::PathFileExists(m_Path)) {
		AfxMessageBox(IDS_TIPS_FILE_NOT_EXIST);
		return;
	}

	//////////////////////////////////////写文件操作演示///////////////////////////
	uci::WFileParams wf;
	wf.CMDString = m_CommandString;
	wf.FilePath = m_Path;
	wf.Timeout = m_Val_Timeout;

	auto r = uci_WriteFromFile(g_curSession, &wf);
	ShowResult(r, IDS_TIPS_CMD_WRITE_FILE, m_CommandString);
	if (UCISUCCESS(r))
		RecordCMD(m_CommandString);
}


void CUCIDemoProjDlg::OnBnClickedButtonReadFile() {
	if (!GetCommandString())
		return;

	UpdateData();
	ResetTimer();

	//////////////////////////////////////read file example///////////////////////////
	CString fileName("RFile.bin");
	RFileParams rfp;
	ZeroMemory(&rfp, sizeof(rfp));
	rfp.CMDString = m_CommandString;
	rfp.FilePath = CreateBmpFileName(m_CommandString, fileName); 
	//msg.CompareNoCase(_T("prtscn:bmp;")) == 0 ? _T("screen_short.bmp") : _T("RFile.bin");
	rfp.Timeout = m_Val_Timeout;

	auto r = uci_ReadToFile(g_curSession, &rfp);
	ShowResult(r, IDS_TIPS_CMD_READFILE, m_CommandString);

	if (UCISUCCESS(r)) {
		if (fileName.Right(4).CompareNoCase(_T(".bmp")) == 0) {
			AppendTraceMsg(_T("Save path：%s"), rfp.FilePathFinal);
		} else {
			AppendTraceMsg(_T("Save path：%s,\r\nIf you want view the file,please change file extention, and use appropriate Viewer to view!"),
				rfp.FilePathFinal);
		}
		RecordCMD(rfp.CMDString);
	}
}

void CUCIDemoProjDlg::OnDestroy() {
	CDialogEx::OnDestroy();

	//////////////////////////////////////remember app data////////////////////////////
	UpdateData();

	theApp.WriteProfileInt(_T("Params"), _T("Timeout"), m_Val_Timeout);
	theApp.WriteProfileInt(_T("RParams"), _T("BYTES"), m_Val_BytesToRead);
	theApp.WriteProfileInt(_T("RParams"), _T("COMTYPE"), m_COMType);

	try {	
		CStdioFile f(m_strCMDListPath, CFile::modeCreate| CFile::modeWrite | CFile::typeText);
			
		CString str1, str2;
		int n;
		for (int i = m_cbxCMD.GetCount() - 1; i >= 0; i--) {
			n = m_cbxCMD.GetLBTextLen(i);
			m_cbxCMD.GetLBText(i, str1.GetBuffer(n));
			str1.ReleaseBuffer();

			f.WriteString(str1 + _T("\n"));
		}
	} catch (CException* e) {
		e->ReportError();
		e->Delete();
	}
}

void CUCIDemoProjDlg::OnBnSetfocusBtnWriteInFile() {	
	m_btnWriteParam_File.SetTooltip(_T("hello"));
}

BOOL CUCIDemoProjDlg::PreTranslateMessage(MSG* pMsg) {	
	if (pMsg->message == WM_MOUSEMOVE)
		m_tips.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}