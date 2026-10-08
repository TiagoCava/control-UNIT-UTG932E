#include "stdafx.h"
#include "SimpleUCI.h"
#include <vector>

#include "uci.h"
#include "dso_base.h"
#include "unit.h"

#include "uci_cpp.h"

#pragma comment(lib, "uci.lib")

using namespace uci;

int HandleResult(const TCHAR* _msg, int _r) {
	if (UCISUCCESS(_r)) {
		_tprintf(_T("%s:  OK, r = %d;\n"), _msg, _r);
	} else {
		_tprintf(_T("%s:  Error, r = %d; msg = %s\n"), _msg, _r, uci_GetLastError());
	}
	return _r;
}

void ParseParamsMeasure(byte* _data, int _data_len) {
	if (_data_len != (sizeof(cb::MeaValue) * EMeaParam::MP_MAX_COUNT))
		return;

	cb::MeaValue* pmv = reinterpret_cast<cb::MeaValue*>(_data);

	for (size_t i = 0; i < EMeaParam::MP_MAX_COUNT; i++) {
		_tprintf(_T("\r\nIsExist=%d,IsValid=%d,Value=%f, Unit(Type=%d,Scale=%d)"),
			pmv->IsExist, pmv->IsValid, pmv->Value, pmv->Unit.Type, pmv->Unit.Scale);
		
		if (pmv->IsExist && pmv->IsValid)
		{
			printf("- %s%s", unit::uci_UnitFindScaleName(pmv->Unit.Scale),
				unit::uci_UnitFindTypeName(pmv->Unit.Type));

			printf("- 标准单位： %f", unit::uci_UnitConvertTo(pmv->Value, pmv->Unit.Scale, unit::SCALE_STD, 1000.0));
		}

		pmv++;
	}
}

namespace dat {
	struct MeaUserParams {
		char ID;   //-1 : 代表无测量参数；
		char CH;   //0 起始的通道号；  ID == -1 ： CH无效。
	};

	enum ECH {
		CH1,
		CH2,
		CH3,
		CH4,
	};
}
void _main_c();
void _main_cpp();

void _main() {
	//_main_c();
	_main_cpp();
}

void _main_c() {
	u_session session = INVALID_SESSION;
	//u_cstring addr = _T("[C:UPO][D:UPO][T:TCPIP][IP:192.168.1.113][Port:0x1388]");	
	u_cstring addr = _T("[C:UPO][D:UPO][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x3][CFG:3][I:0]");
	
	try {
		u_status r = uci_Open(addr, &session, 1000);
		if (UCIERR(r)) {
			_tprintf(_T("错误:  设备打开失败！ 地址：%s, 错误信息： %s\n"), addr, uci_GetLastError());
			throw r;
		}
		
	} catch (u_status r) {

		if (UCIERR(r))
			_tprintf(_T("错误信息： %s\n"), uci_GetLastError());
	}

	if (session != INVALID_SESSION) {
		uci_Close(session);
	}
}

void _main_cpp() {
	uci::Session ses;

	u_cstring addr = _T("[C:UPO][D:UPO][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x3][CFG:3][I:0]");	
	u_status r = ses.Open(addr);
	if (UCIERR(r)) {
		_tprintf(_T("错误:  设备打开失败！ 地址：%s, 错误信息： %s\n"), addr, uci_GetLastError());
		return;
	}
	
	std::vector<byte> buf(400, 0);
	r = ses.Read(_T("mea:all?;"), 1000, buf.data(), buf.size());
	HandleResult(_T("mea:all?"), r);
	if (UCISUCCESS(r))
		ParseParamsMeasure(buf.data(), buf.size());
}