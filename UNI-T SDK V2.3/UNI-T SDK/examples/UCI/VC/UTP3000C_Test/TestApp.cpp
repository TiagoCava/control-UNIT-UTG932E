#include "stdafx.h"
#include "TestApp.h"

#pragma  comment(lib, "lib\\uci.lib")

using namespace uci;

TestApp::TestApp() : m_session(INVALID_SESSION){}


TestApp::~TestApp() {}

u_status TestApp::Run(const TCHAR* _addr) {	
	return 0;
}

int TestApp::HandleResult(const TCHAR* _msg, int _r) {
	if (UCISUCCESS(_r)) {
		_tprintf(_T("%s:  OK, r = %d;\n"), _msg, _r);
	} else {
		_tprintf(_T("%s:  Error, r = %d; msg = %s\n"), _msg, _r, uci_GetLastError());
	}
	return _r;
}

u_status TestApp::Open(const u_tchar* _addr, int _timeout) {
	uci_SetAttribute(INVALID_SESSION, _T("lang:zh-Hans;"), nullptr, 0);
	return uci_Open(_addr, &m_session, _timeout);
}

u_status TestApp::Write(int _timeout, const void* _data, unsigned int _data_len, const TCHAR *format, ...) {
	m_dwTime = GetTickCount();
	
	CString msg;
	va_list argptr;
	va_start(argptr, format);
	msg.FormatV(format, argptr);

	auto r = uci::uci_WriteX(m_session, msg, _timeout, (const byte*)_data, _data_len);
// 	_tprintf(_T("Write [%s], r = %s, %s, use time = %d ms\n"), msg, r,
// 		UCISUCCESS(r) ? _T("OK") : _T("Error"), uci_GetLastError(), GetTickCount() - m_dwTime);
// 	ASSERT(r >= 0);
	va_end(argptr);
	return r;
}

u_status TestApp::Read(u_cstring _msg, u_uint32 _timeout, u_byte* _data, u_size _dataLen) {
	m_dwTime = GetTickCount();
	auto r = uci::uci_ReadX(m_session, _msg, _timeout, _data, _dataLen);
// 	_tprintf(_T("Read %s, r = %s, time = %d\n"), _msg, uci_GetLastError(), GetTickCount() - m_dwTime);
// 	ASSERT(r >= 0);
	return r;
}

u_status TestApp::Read(u_uint32 _timeout, u_byte* _data, u_size _dataLen, u_cstring format, ...) {
	CString msg;
	va_list argptr;
	va_start(argptr, format);
	msg.FormatV(format, argptr);

	m_dwTime = GetTickCount();
	auto r = uci_ReadX(m_session, msg, _timeout, _data, _dataLen);
// 	_tprintf(_T("Read %s, r = %s, time = %d\n"), msg, uci_GetLastError(), GetTickCount() - m_dwTime);
// 	ASSERT(r >= 0);

	va_end(argptr);
	return r;
}
