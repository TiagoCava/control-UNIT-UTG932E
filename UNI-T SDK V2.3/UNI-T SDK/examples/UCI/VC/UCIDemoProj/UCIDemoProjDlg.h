
// UCIDemoProjDlg.h : 头文件
//

#pragma once

#include "lib\uci.h"
#include <queue>
#include "afxwin.h"
#include "lib\dso_base.h"//参数测量结果解析使用
#include "lib\unit.h"//参数测量结果解析使用

class CMyMFCButton : public CMFCButton {
public:
	void SetDelayFullTextTooltipSet(bool DelayFullTextTooltipSet) {
		m_bDelayFullTextTooltipSet = DelayFullTextTooltipSet;
	}
};

// CUCIDemoProjDlg 对话框
class CUCIDemoProjDlg : public CDialogEx
{
// 构造
public:
	CUCIDemoProjDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UCIDEMOPROJ_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	static CUCIDemoProjDlg*  c_object;
// 实现
protected:
	HICON m_hIcon;
	CEdit m_Ctrl_Edit_Msg;
	UINT m_Val_Timeout;
	int m_Val_BytesToRead;
	CMFCEditBrowseCtrl m_CtrlFileChoose;
	CComboBox   m_cbxCMD;
	CMyMFCButton  m_btnWriteParam_File;

protected:
	CToolTipCtrl   m_tips;
protected:
	bool  m_IsConnect;
	int   m_MaxCMDHistoryCount;
	CString m_strCMDListPath;
	CString m_DevName, m_IDN;
	CString m_ConnectAddrIP;
	CString m_ConnectAddrPort;
	int m_nConnectAddrPort;
	int m_COMType;

	uci::Node m_Nodes[100];

	CString m_CommandString, m_Path;
	
protected:
	void GetCMDString(CString& _msg);
	int UCIWrite(const TCHAR* _cmd, UINT _timeOut);
	int UCIRead(const TCHAR* _cmd, u_byte* _data, u_size _dataLen, UINT _timeOut);
	
	static int __stdcall UCI_MessageProc(uci::UCIMSG* _msg);

	void Disconnect();
	void RecordCMD(const CString& _msg);

	void ShowResult(int _r, int _idsSOerationString, const TCHAR* _format = NULL, ...);

	// @brief   :  追加消息文本到消息显示区域
	// @param   :  const u_tchar * _format
	// @param   :  ...
	// @return  :  void
	// @remarks :
	void AppendTraceMsg(const u_tchar * _format, ...);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	void InitUI();
	void LoadCMDList();
	bool GetAppFolderPath(CString& _strPath);
	void EnableWidgets(BOOL _isConnect);

	CString FindConnectAddrField(const CString& _addr, const CString& _fieldBegin, const CString&_fieldEnd);

	//解析参数测量数据
	void ParseParamsMeasure(byte* _data, int _data_len);		
	void ParseParamsMeasure_User(byte* _data, int _data_len);
	void PrintMeaParam(const TCHAR * _name, const comAPICommon::MeaValue& _p);

	bool GetCommandString();
	bool GetFilePath();
protected:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnBnClickedButtonQueryNodes();
	afx_msg LRESULT AFX_MSG_CALL OnLANDisconnect(WPARAM wParam, LPARAM lParam);
	
public:
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedBtnWrite();
	afx_msg void OnBnClickedBtnRead();

	const CString& CreateBmpFileName(CString &msg, CString& fileName);

	afx_msg void OnBnClickedButtonWriteFile();
	afx_msg void OnBnClickedButtonReadFile();	
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnWriteInFile();

	bool CheckConnectiion();

	afx_msg void OnBnSetfocusBtnWriteInFile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 显示设备地址列表
	CComboBox m_cbxDevAddr;

	inline CString GetCurrentTime() {
		SYSTEMTIME t = { 0 };
		GetLocalTime(&t);

		CString s;
		s.Format(_T("%02d-%02d-%02d"), t.wHour, t.wMinute, t.wSecond);
		return s;
	}


	inline CString LoadString(UINT idRes) {
		CString str;
		BOOL bNameValid = str.LoadString(idRes);
		ASSERT(bNameValid);
		return str;	
	}	

	inline void SetWidgetText(UINT _idWidget, UINT _idString) {
		SetDlgItemText(_idWidget, LoadString(_idString));
	}
	CString m_cbBoxText;
};
