#pragma once

#include "resource.h"

#include <vector>
#include "TestApp.h"

class TestP330XC : public TestApp {
	typedef TestApp _base;
public:
	TestP330XC() {}
	virtual ~TestP330XC() {}

	enum E_CC_CV_MODE{
		CC = 0,
		CV = 1
	};
	
	enum E_TRACK_MODE {
		TRACK_UNKNOWN = -1,
		TRACK_INDEPENDENT = 0,
		TRACK_SERIES = 1,
		TRACK_PARA = 2
	};

	struct Status {
		E_CC_CV_MODE  CH1_CC_CV;
		E_CC_CV_MODE  CH2_CC_CV;
		E_TRACK_MODE  Track;
		bool IsOVPEnable;
		bool IsOCPEnable;
		bool IsCH1ON;
		bool IsCH2ON;
	};

	virtual u_status Run(const TCHAR* _addr) override;
	u_status Run_UTP3305S(const TCHAR* _addr);
	u_status QueryStauts(Status& _st, byte* _stautsCode = nullptr);
	u_status QueryParam(const u_tchar* _msg, char* _data, int _data_count, int _timeout = 50) {
		auto r = _base::Write(_timeout, nullptr, 0, _msg);
		Sleep(50);
		return UCIERR(r) ? r: _base::Read(_msg, (UINT)_timeout, (byte*)_data, (UINT)_data_count);
	}

	u_status QueryParam_Double(const u_tchar* _msg, double& _v, int _timeout = 50, bool _traceEnable = true);

	u_status QueryParam_CString(const u_tchar* _msg, CString& _v, int _timeout = 60, bool _traceEnable = true) {
		_v = _T("");

		char buf[64];
		ZeroMemory(buf, sizeof(buf));
		auto r = QueryParam(_msg, buf, sizeof(buf), _timeout);
		if (UCISUCCESS(r)) {
			_v = CStringA(buf);
			if (_traceEnable)
				_tprintf(_T("[%s] -> 回读OK, 数据 ： %s \n"), _msg, _v);
		} else {
			if (_traceEnable) {
				_tprintf(_T("[%s] -> 回读错误， 数据 ： %s \n"), _msg, _v);
				m_result.push_back(r);
			}
		}
		return r;
	}
	u_status SendMsg(const u_tchar* _msg, int _timeout = 50);

	u_status SetParam(const u_tchar* _msgWrite, const u_tchar* _msgQuery, 
		double _vWrite, int _readTimeout = 50);

	u_status SetTrackMode(E_TRACK_MODE _mode);

	u_status SetStatusParams(const u_tchar* _msg, byte _v, char _statusBitOffset, char _statusMaskBits);
protected:
	inline char GetBit(byte _byte, byte _offset, byte _mask) {
		return (char)((_byte >> _offset) & _mask);
	}

	Status ParseStatus(byte _byte);


	bool IsTestOk();

	bool IsContinue(bool _isok);
private:
	std::vector<u_status>  m_result;
};
#if 0
u_session  m_session;

u_status r = uci_Open("[C:POWER][D:P330XC][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x2][CFG:1][I:1]",
	&m_session, 1000);

auto r = uci_WriteX(m_session, "*IDN?", 1000, NULL, 0);

char buf[20];
ZeroMemory(buf, sizeof(buf));
auto r = uci_ReadX(m_session, "*IDN?", 100, buf, sizeof(buf));//读取时的_msg可以为空！

uci_Close(m_session);

/*
//@brief          : 读取数据
//@param _session : 会话ID
//@param _msg     : 命令字符串，以'\0'结尾. 具体格式见文档；
//@param _timeout : 读取超时。
//@param _data    : 接收读取到的数据的缓冲区地址。
//@param _dataLen : _data指向的缓冲区长度（Bytes）
//@return : 	查看本文件中的“关于接口返回的状态值”说明块
//@remarks : 本接口是uci_Read的简化版本
u_status _UCIAPI uci_ReadX(u_session _session, u_cstring _msg,
u_uint32 _timeout, u_byte* _data, u_size _dataLen);
*/
#endif