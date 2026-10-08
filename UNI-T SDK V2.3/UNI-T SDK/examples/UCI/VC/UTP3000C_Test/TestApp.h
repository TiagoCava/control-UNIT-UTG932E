#pragma once

#include "lib\uci.h"

class TestApp {
public:
	TestApp();
	virtual ~TestApp();

	virtual u_status Run(const TCHAR* _addr);

protected:
	virtual int HandleResult(const TCHAR* _msg, int _r);

	virtual u_status Open(const u_tchar* _addr, int _timeout = 1000);

	virtual u_status Write(int _timeout, const void* _data, unsigned int _data_len, const TCHAR *format, ...);

	virtual u_status Read(u_cstring _msg, u_uint32 _timeout, u_byte* _data, u_size _dataLen);

	virtual u_status Read(u_uint32 _timeout, u_byte* _data, u_size _dataLen, u_cstring format, ...);
protected:
	u_session  m_session;
	DWORD m_dwTime;
};

