// UTP3000C_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "UTP3000C_Test.h"
#include "TestApp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DELAY_WRITE_AND_READ (50)

// 唯一的应用程序对象

CWinApp theApp;

TestP330XC testApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	setlocale(LC_ALL, "CHS");

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO:  更改错误代码以符合您的需要
			_tprintf(_T("错误:  MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else {
			// TODO:  在此处为应用程序的行为编写代码。
			if (argc < 2) {
				_tprintf(_T("直接启动，默认采用USB通信。\n"));
				testApp.Run(_T("[C:POWER][D:P330XC][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x2][CFG:1][I:1]"));
			} else {
				_tprintf(_T("命令行启动，启动参数：%s\n"), argv[1]);
				if (CString(argv[1]).Find(_T("UTP3305S"), 0) != -1)
				{
					testApp.Run_UTP3305S(argv[1]);
				}
				else
				{
					testApp.Run(argv[1]);
				}
			}
			system("pause");
		}
	}
	else
	{
		// TODO:  更改错误代码以符合您的需要
		_tprintf(_T("错误:  GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

u_status TestP330XC::Run_UTP3305S(const TCHAR* _addr)
{
	auto r = HandleResult(_T("连接设备"), _base::Open(_addr));
	if (UCIERR(r)) {
		_tprintf(_T("错误:  打开设备 失败\n"));
		AfxMessageBox(_T("设备连接失败！请检查驱动是否安装成功？数据线是否连接？如果是串口连接，请检查run_utp3000c_test.bat文件中的[PORT:4]指定的串口号是否正确！"));
		return r;
	}

	_tprintf(_T("------------测试注意--------------\n"));
	_tprintf(_T("1、测试读写指令是否一致；\n"));
	_tprintf(_T("2、测试读写指令是否与设备状态一致\n"));


	_tprintf(_T("------------ 测试第1条--------------\n"));

	CString strIDN;
	r = QueryParam_CString(_T("*IDN?\r\n"), strIDN);
	if (UCISUCCESS(r)) {
		CString strName = _T("无法识别！");
		if (strIDN.Find(_T("P3303"), 0) != -1) {
			strName = _T("UTP3303S");
		}
		else if (strIDN.Find(_T("P3305"), 0) != -1) {
			strName = _T("UTP3305S");
		}
		else if (strIDN.Find(_T("P8303"), 0) != -1) {
			strName = _T("UTP8303Z");
		}
		else if (strIDN.Find(_T("P8305"), 0) != -1) {
			strName = _T("UTP8305Z");
		}
		_tprintf(_T("识别机型 ：%s \n"), strName);
	}

	_tprintf(_T("关于设备名称请注意： 分内销和外销\n"));
	_tprintf(_T("内销 ： USB 固件中的产品名 ： P830XZ；   *IDN?  ：  P8303Z 和 P8305Z\n"));
	_tprintf(_T("外销 ： USB 固件中的产品名 ： P330XS；   *IDN?  ：  P3303S 和 P3305S\n"));

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	double v = 0.0;
	std::vector<u_status>  result;

	CString strMsg;

	_tprintf(_T("------------ 测试第2条--------------\n"));
	r = SetParam(_T(":SYSTem:BEEPer 0\r\n"), _T(":SYSTem:BEEPer?\r\n"), 0);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发:SYSTem:BEEPer 0，请检查【蜂鸣器】是否已经【关闭】？"),
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [:SYSTem:BEEPer 0]控制与设备状态不匹配！！！！\n"));
	}

	r = SetParam(_T(":SYSTem:BEEPer 1\r\n"), _T(":SYSTem:BEEPer?\r\n"), 1);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发:SYSTem:BEEPer 1，请检查【蜂鸣器】是否已经【打开】？"),
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [:SYSTem:BEEPer 1]控制与设备状态不匹配！！！！\n"));
	}

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("------------ 测试第3条--------------\n"));
	r = SetParam(_T(":SYSTem:LOCK 1\r\n"), _T(":SYSTem:LOCK?\r\n"), 1);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经设置键盘锁，请检查【键盘】是否已经【锁定】？"),
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [:SYSTem:LOCK 1]控制与设备状态不匹配！！！！\n"));
	}

	Sleep(1000);

	r = SetParam(_T(":SYSTem:LOCK 0\r\n"), _T(":SYSTem:LOCK?\r\n"), 0);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发:SYSTem:LOCK 0，请检查【键盘】是否已经【解锁】？"),
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [:SYSTem:LOCK 0]控制与设备状态不匹配！！！！\n"));
	}

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("------------ 测试第4条--------------\n"));
	_tprintf(_T("准备测试:KEY:<key>指令，请检查M1~M5是否依次被调用: \n"));
	Sleep(3000);
	r = SendMsg(_T(":KEY:F1\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":KEY:F2\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":KEY:F3\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":KEY:F4\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":KEY:F5\r\n"));
	Sleep(1000);
	r = AfxMessageBox(_T("是否M1~M5依次被调用"), MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
	m_result.push_back(r);
	if (r < 0)
		_tprintf(_T("--ERROR-- [KEY:<key>]控制与设备状态不匹配！！！！\n"));

	_tprintf(_T("------------ 测试第5条--------------\n"));
	_tprintf(_T("准备测试:OUTPut:MODE指令\n"));
	r = SendMsg(_T(":OUTP:MODE SER\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":OUTP:MODE PARA\r\n"));
	Sleep(1000);
	r = SendMsg(_T(":OUTP:MODE NOMAL\r\n"));
	Sleep(1000);

	/*_tprintf(_T("------------ 测试第6条--------------\n"));
	_tprintf(_T("准备测试OVP开关）\n"));
	SetStatusParams(_T("OVP1"), 1, 4, 0x1);
	SetStatusParams(_T("OVP0"), 0, 4, 0x1);

	_tprintf(_T("------------ 测试第7条--------------\n"));
	_tprintf(_T("准备测试OCP开关）\n"));
	SetStatusParams(_T("OCP1"), 1, 5, 0x1);
	SetStatusParams(_T("OCP0"), 0, 5, 0x1);
	Sleep(1000);*/

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("------------ 测试第6条--------------\n"));
	_tprintf(_T("准备测试通道开关指令（:OUTPut<n>）注意：UTP3305S没有OCP与OVP开关，其启动之后自动开启相关保护功能\n"));
	_tprintf(_T("准备测试通道一开关\n"));
	SetParam(_T(":OUTPut1 1\r\n"), _T(":OUTPut1?\r\n"), 1);
	Sleep(1000);
	SetParam(_T(":OUTPut1 0\r\n"), _T(":OUTPut1?\r\n"), 0);
	_tprintf(_T("准备测试通道二开关\n"));
	SetParam(_T(":OUTPut2 1\r\n"), _T(":OUTPut2?\r\n"), 1);
	Sleep(1000);
	SetParam(_T(":OUTPut2 0\r\n"), _T(":OUTPut2?\r\n"), 0);
	_tprintf(_T("准备测试通道三开关\n"));
	SetParam(_T(":OUTPut3 1\r\n"), _T(":OUTPut3?\r\n"), 1);
	Sleep(1000);
	SetParam(_T(":OUTPut3 0\r\n"), _T(":OUTPut3?\r\n"), 0);

	_tprintf(_T("------------ 测试第7条--------------\n"));
	QueryParam_Double(_T(":SOURce1:CURRent?\r\n"), v);
	QueryParam_Double(_T(":SOURce2:CURRent?\r\n"), v);
	QueryParam_Double(_T(":SOURce3:CURRent?\r\n"), v);

	_tprintf(_T("------------ 测试第8条--------------\n"));
	QueryParam_Double(_T(":SOURce1:VOLTage?\r\n"), v);
	QueryParam_Double(_T(":SOURce2:VOLTage?\r\n"), v);
	QueryParam_Double(_T(":SOURce3:VOLTage?\r\n"), v);

	_tprintf(_T("------------ 测试第9条--------------\n"));
	SetParam(_T(":SOURce1:VOLTage 20\r\n"), _T(":SOURce1:VOLTage?\r\n"), 20.000);
	SetParam(_T(":SOURce1:VOLTage 10\r\n"), _T(":SOURce1:VOLTage?\r\n"), 10.000);

	SetParam(_T(":SOURce2:VOLTage 20\r\n"), _T(":SOURce2:VOLTage?\r\n"), 20.000);
	SetParam(_T(":SOURce2:VOLTage 10\r\n"), _T(":SOURce2:VOLTage?\r\n"), 10.000);

	SetParam(_T(":SOURce3:VOLTage 3.3\r\n"), _T(":SOURce3:VOLTage?\r\n"), 3.300);
	SetParam(_T(":SOURce3:VOLTage 5\r\n"), _T(":SOURce3:VOLTage?\r\n"), 5.000);

	_tprintf(_T("------------ 测试第10条--------------\n"));
	SetParam(_T(":SOURce1:CURRent 3\r\n"), _T(":SOURce1:CURRent?\r\n"), 3.000);
	SetParam(_T(":SOURce1:CURRent 2\r\n"), _T(":SOURce1:CURRent?\r\n?"), 2.000);

	SetParam(_T(":SOURce2:CURRent 3\r\n"), _T(":SOURce2:CURRent?\r\n"), 3.000);
	SetParam(_T(":SOURce2:CURRent 2\r\n"), _T(":SOURce2:CURRent?\r\n?"), 2.000);

	_tprintf(_T("------------ 测试第11条--------------\n"));
	_tprintf(_T("注意:过压值必须大于当前设置电压的10%以上才能设置成功\n"));

	SetParam(_T(":SOURce1:VOLTage:PROTection 30\r\n"), _T(":SOURce1:VOLTage:PROTection?\r\n"), 30.000);
	SetParam(_T(":SOURce1:VOLTage:PROTection 20\r\n"), _T(":SOURce1:VOLTage:PROTection?\r\n"), 20.000);

	SetParam(_T(":SOURce2:VOLTage:PROTection 30\r\n"), _T(":SOURce2:VOLTage:PROTection?\r\n"), 30.000);
	SetParam(_T(":SOURce2:VOLTage:PROTection 20\r\n"), _T(":SOURce2:VOLTage:PROTection?\r\n"), 20.000);

	_tprintf(_T("------------ 测试第12条--------------\n"));
	_tprintf(_T("注意:过流值必须大于当前设置电流的10%以上才能设置成功\n"));

	SetParam(_T(":SOURce1:CURRent:PROTection 3\r\n"), _T(":SOURce1:CURRent:PROTection?\r\n"), 3.000);
	SetParam(_T(":SOURce1:CURRent:PROTection 2.2\r\n"), _T(":SOURce1:CURRent:PROTection?\r\n"), 2.200);

	SetParam(_T(":SOURce2:CURRent:PROTection 3\r\n"), _T(":SOURce2:CURRent:PROTection?\r\n"), 3.000);
	SetParam(_T(":SOURce2:CURRent:PROTection 2.2\r\n"), _T(":SOURce2:CURRent:PROTection?\r\n"), 2.200);

	_tprintf(_T("测试结果: \n"));
	_tprintf(_T("指令与指令间需要一个延时，否则设置不成功，比如OVPSET1:10.00后，必须延时50ms，才能执行OVPSET1?的查询指令； \n"));

	if (IsTestOk())
		AfxMessageBox(_T("所有指令测试通过！"));
	else
		AfxMessageBox(_T("所有指令测试失败！"));
	return r;
}

u_status TestP330XC::Run(const TCHAR* _addr) {
	auto r = HandleResult(_T("连接设备"), _base::Open(_addr));
	if (UCIERR(r)) {
		_tprintf(_T("错误:  打开设备 失败\n"));
		AfxMessageBox(_T("设备连接失败！请检查驱动是否安装成功？数据线是否连接？如果是串口连接，请检查run_utp3000c_test.bat文件中的[PORT:4]指定的串口号是否正确！"));
		return r;
	}

	_tprintf(_T("------------测试注意--------------\n"));
	_tprintf(_T("1、测试读写指令是否一致；\n"));
	_tprintf(_T("2、测试读写指令是否与设备状态一致\n"));


	_tprintf(_T("------------ 测试第1条--------------\n"));

	CString strIDN;
	r = QueryParam_CString(_T("*IDN?"), strIDN);
	if (UCISUCCESS(r)) {
		CString strName = _T("无法识别！");
		if (strIDN.Find(_T("P3303"), 0) != -1) {
			strName = _T("UTP3303C");
		} else if (strIDN.Find(_T("P3305"), 0) != -1) {
			strName = _T("UTP3305C");
		} else if (strIDN.Find(_T("P8303M"), 0) != -1) {
			strName = _T("UTP8303M");
		} else if (strIDN.Find(_T("P8305M"), 0) != -1) {
			strName = _T("UTP8305M");
		}
		_tprintf(_T("识别机型 ：%s \n"), strName);
	}

	_tprintf(_T("关于设备名称请注意： 分内销和外销\n"));
	_tprintf(_T("内销 ： USB 固件中的产品名 ： P830XM；   *IDN?  ：  P8303M 和 P8305M\n"));
	_tprintf(_T("外销 ： USB 固件中的产品名 ： P330XC；   *IDN?  ：  P3303C 和 P3305C\n"));	

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	double v = 0.0;
	std::vector<u_status>  result;

	CString strMsg ;

	_tprintf(_T("------------ 测试第2条--------------\n"));
	r = SetParam(_T("BEEP0"), _T("BEEP?"), 0);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发BEEP0，请检查【蜂鸣器】是否已经【关闭】？"),
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [BEEP0]控制与设备状态不匹配！！！！\n"));
	}

	r = SetParam(_T("BEEP1"), _T("BEEP?"), 1);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发BEEP1，请检查【蜂鸣器】是否已经【打开】？"), 
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [BEEP1]控制与设备状态不匹配！！！！\n"));
	}

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("------------ 测试第3条--------------\n"));
	r = SetParam(_T("LOCK1"), _T("LOCK?"), 1);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经设置键盘锁，请检查【键盘】是否已经【锁定】？"), 
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [LOCK1]控制与设备状态不匹配！！！！\n"));
	}

	r = SetParam(_T("LOCK0"), _T("LOCK?"), 0);
	if (UCISUCCESS(r)) {
		r = AfxMessageBox(_T("已经下发LOCK0，请检查【键盘】是否已经【解锁】？"), 
			MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
		m_result.push_back(r);

		if (r < 0)
			_tprintf(_T("--ERROR-- [LOCK0]控制与设备状态不匹配！！！！\n"));
	}

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("请注意： 第二批电源的，BEEP1和BEEP0是反的！！！！！ \n"));

	_tprintf(_T("------------ 测试第4条--------------\n"));
	_tprintf(_T("准备测试RCL指令，请检查M1~M5是否依次被调用: \n"));
	Sleep(3000);
	r = SendMsg(_T("RCL1"));
	Sleep(1000);
	r = SendMsg(_T("RCL2"));
	Sleep(1000);
	r = SendMsg(_T("RCL3"));
	Sleep(1000);
	r = SendMsg(_T("RCL4"));
	Sleep(1000);
	r = SendMsg(_T("RCL5"));
	Sleep(1000);
	r = AfxMessageBox(_T("是否M1~M5依次被调用"), MB_YESNO | MB_ICONQUESTION) == IDYES ? 0 : -1;
	m_result.push_back(r);
	if (r < 0)
		_tprintf(_T("--ERROR-- [RCL]控制与设备状态不匹配！！！！\n"));

	_tprintf(_T("------------ 测试第5条--------------\n"));
	_tprintf(_T("准备测试TRACK指令\n"));	
	SetTrackMode(E_TRACK_MODE::TRACK_SERIES);
	Sleep(1000);
	SetTrackMode(E_TRACK_MODE::TRACK_PARA);
	Sleep(1000);
	SetTrackMode(E_TRACK_MODE::TRACK_INDEPENDENT);
	Sleep(1000);

	_tprintf(_T("------------ 测试第6条--------------\n"));
	_tprintf(_T("准备测试OVP开关）\n"));
	SetStatusParams(_T("OVP1"), 1, 4, 0x1);
	SetStatusParams(_T("OVP0"), 0, 4, 0x1);

	_tprintf(_T("------------ 测试第7条--------------\n"));
	_tprintf(_T("准备测试OCP开关，注意：第二批电源中，OCP0无法关闭OCP功能呢！！！！！）\n"));
	SetStatusParams(_T("OCP1"), 1, 5, 0x1);
	SetStatusParams(_T("OCP0"), 0, 5, 0x1);
	Sleep(1000);

	if (!IsContinue(UCISUCCESS(r)))
		return -1;

	_tprintf(_T("------------ 测试第8条--------------\n"));
	_tprintf(_T("准备测试通道开关指令（OUT） 请确保先关闭OCP和OVP再测试。\n"));
	SetStatusParams(_T("OUT1"), 0x3, 6, 0x3);
	Sleep(1000);
	SetStatusParams(_T("OUT0"), 0x0, 6, 0x3);	

	_tprintf(_T("------------ 测试第9条--------------\n"));
	QueryParam_Double(_T("IOUT1?"), v);
	QueryParam_Double(_T("IOUT2?"), v);

	_tprintf(_T("------------ 测试第10条--------------\n"));
	QueryParam_Double(_T("VOUT1?"), v);
	QueryParam_Double(_T("VOUT2?"), v);

	_tprintf(_T("------------ 测试第11条--------------\n"));
	SetParam(_T("VSET1:20.00"), _T("VSET1?"), 20.00);
	SetParam(_T("VSET1:10.00"), _T("VSET1?"), 10.00);

	SetParam(_T("VSET2:20.00"), _T("VSET2?"), 20.00);
	SetParam(_T("VSET2:10.00"), _T("VSET2?"), 10.00);

	_tprintf(_T("------------ 测试第12条--------------\n"));
	SetParam(_T("ISET1:2.000"), _T("ISET1?"), 2.000);
	SetParam(_T("ISET1:3.000"), _T("ISET1?"), 3.000);

	SetParam(_T("ISET2:2.000"), _T("ISET2?"), 2.000);
	SetParam(_T("ISET2:3.000"), _T("ISET2?"), 3.000);

	_tprintf(_T("------------ 测试第13条--------------\n"));
	SetParam(_T("OVPSET1:30.00"), _T("OVPSET1?"), 30.00);
	SetParam(_T("OVPSET1:20.00"), _T("OVPSET1?"), 20.00);

	SetParam(_T("OVPSET2:30.00"), _T("OVPSET2?"), 30.00);
	SetParam(_T("OVPSET2:20.00"), _T("OVPSET2?"), 20.00);

	_tprintf(_T("------------ 测试第14条--------------\n"));
	SetParam(_T("OCPSET1:3.000"), _T("OCPSET1?"), 3.000);
	SetParam(_T("OCPSET1:2.000"), _T("OCPSET1?"), 2.000);

	SetParam(_T("OCPSET2:3.000"), _T("OCPSET2?"), 3.000);
	SetParam(_T("OCPSET2:2.000"), _T("OCPSET2?"), 2.000);

	_tprintf(_T("测试结果: \n"));
	_tprintf(_T("指令与指令间需要一个延时，否则设置不成功，比如OVPSET1:10.00后，必须延时50ms，才能执行OVPSET1?的查询指令； \n"));

	_tprintf(_T("对设置数据的小数位有要求： 电压类必须是2位，电流是3位。 比如：下面就不成功 \n"));
	SetParam(_T("OVPSET1:15.000"), _T("OVPSET1?"), 15.00);
	_tprintf(_T("修改成两位小数点位就OK了 \n"));
	SetParam(_T("OVPSET1:16.00"), _T("OVPSET1?"), 16.00);

	_tprintf(_T("另请注意: \n"));
	_tprintf(_T("说明书中是否有 BEEP?\\LOCK? 的描述？ ； \n对STATUS?的描述是否完整（与最早的协议文档不一致）: \n"));
	
	if (IsTestOk())
		AfxMessageBox(_T("所有指令测试通过！"));
	else
		AfxMessageBox(_T("所有指令测试失败！"));
	return r;
}

u_status TestP330XC::QueryStauts(Status& _st, byte* _stautsCode) {
	char v = 0;
	auto r = QueryParam(_T("STATUS?"), &v, sizeof(v));
	if (UCISUCCESS(r)) {
		_st = ParseStatus(v);
		if (_stautsCode != nullptr)
			*_stautsCode = v;
	} else {
		m_result.push_back(r);
	}
	return r;
}

u_status TestP330XC::QueryParam_Double(const u_tchar* _msg, double& _v, int _timeout /*= 50*/, bool _traceEnable /*= true*/) {
	char buf[20];
	ZeroMemory(buf, sizeof(buf));

	auto r = QueryParam(_msg, buf, sizeof(buf), _timeout);

	if (UCISUCCESS(r)) {
		_v = atof(buf);
		if (_traceEnable)
			_tprintf(_T("[%s] -> OK, Value = %f \n"), _msg, _v);
	} else {
		if (_traceEnable) {
			_tprintf(_T("[%s] -> Error, r = %d, msg = %s \n"), _msg, r, uci::uci_GetLastError());
			m_result.push_back(r);
		}
	}
	return r;
}

u_status TestP330XC::SendMsg(const u_tchar* _msg, int _timeout /*= 50*/) {
	auto r = _base::Write(_timeout, nullptr, 0, _msg);
	if (UCISUCCESS(r)) {
		_tprintf(_T("[%s] -> 发送OK! \n"), _msg);
	} else {
		_tprintf(_T("[%s] -> 发送Error, r = %d, msg = %s! \n"), _msg, r, uci::uci_GetLastError());
		m_result.push_back(r);
	}
	return r;
}

u_status TestP330XC::SetParam(const u_tchar* _msgWrite, const u_tchar* _msgQuery,
	double _vWrite, int _readTimeout /*= 50*/) {
	auto r = _base::Write(50, nullptr, 0, _msgWrite);
	if (UCISUCCESS(r)) {
		double vRead = 0.0;
		Sleep(DELAY_WRITE_AND_READ);
		r = QueryParam_Double(_msgQuery, vRead, _readTimeout, false);
		if (UCISUCCESS(r)) {
			if (_vWrite == vRead) {
				_tprintf(_T("[%s] -> 回读值[%s] = %f ->设置OK! \n"), _msgWrite, _msgQuery, vRead);
			} else {
				_tprintf(_T("[%s] -> 回读值[%s] = %f ->设置Error! \n"), _msgWrite, _msgQuery, vRead);
				m_result.push_back(r);
			}
			return _vWrite == vRead ? 0 : -1;
		}
		else {
			_tprintf(_T("[%s] -> 回读[%s]数据失败， r = %d : %s ->设置Error!! \n"), _msgWrite, _msgQuery, r, uci::uci_GetLastError());
			m_result.push_back(r);
		}
	} else {
		_tprintf(_T("[%s] -> 数据失败， r = %d : %s ->设置Error!! \n"), _msgWrite, r, uci::uci_GetLastError());
		m_result.push_back(r);
	}
	return r;
}

u_status TestP330XC::SetTrackMode(E_TRACK_MODE _mode) {
	CString s;
	s.Format(_T("TRACK%d"), _mode);
	auto r = SendMsg(s);

	Status st;
	if (UCISUCCESS(r)) {
		Sleep(DELAY_WRITE_AND_READ);
		r = QueryStauts(st);
		if (UCISUCCESS(r)) {
			_tprintf(_T("[%s] -> 回读[STATU?]-> TRACK = %d -> 设置%s！\n"), 
				s, st.Track, st.Track == _mode ? _T("OK") : _T("Error"));
		} else {
			_tprintf(_T("[%s] -> 回读[STATU?] -> 失败！！\n"), s);
		}
	}
	return r;
}

u_status TestP330XC::SetStatusParams(const u_tchar* _msg, byte _v, char _statusBitOffset, char _statusMaskBits) {
	auto r = SendMsg(_msg);

	Status st;
	if (UCISUCCESS(r)) {
		Sleep(DELAY_WRITE_AND_READ);
		byte st_code = 0;
		r = QueryStauts(st, &st_code);
		if (UCISUCCESS(r)) {
			auto bits = GetBit(st_code, _statusBitOffset, _statusMaskBits);
			_tprintf(_T("[%s] -> 回读[STATU? = 0x%x]-> Bits = 0x%x -> 设置%s！\n"),
				_msg, st_code , bits, bits == _v ? _T("OK") : _T("Error"));
		} else {
			_tprintf(_T("[%s] -> 回读[STATU?] -> 失败！！\n"), _msg);
			m_result.push_back(r);
		}
	}
	return r;
}

TestP330XC::Status TestP330XC::ParseStatus(byte _byte) {
	Status st;
	ZeroMemory(&st, sizeof(st));
	st.CH1_CC_CV = (_byte & 0x1) == 0 ? E_CC_CV_MODE::CC : E_CC_CV_MODE::CV;
	st.CH2_CC_CV = (_byte & 0x2) == 0 ? E_CC_CV_MODE::CC : E_CC_CV_MODE::CV;

	switch (GetBit(_byte, 2, 0x3)) {
	case 0:
		st.Track = E_TRACK_MODE::TRACK_INDEPENDENT;
		break;
	case 1:
		st.Track = E_TRACK_MODE::TRACK_SERIES;
		break;
	case 2:
		st.Track = E_TRACK_MODE::TRACK_PARA;
		break;
	default:
		st.Track = E_TRACK_MODE::TRACK_UNKNOWN;
		break;
	}

	st.IsOVPEnable = GetBit(_byte, 4, 0x1) == 1;
	st.IsOCPEnable = GetBit(_byte, 5, 0x1) == 1;

	st.IsCH1ON = GetBit(_byte, 6, 0x1) == 1;
	st.IsCH2ON = GetBit(_byte, 7, 0x1) == 1;

	return st;
}

bool TestP330XC::IsTestOk() {
	for each (auto var in m_result) {
		if (var < 0)
			return false;
	}
	return true;
}

bool TestP330XC::IsContinue(bool _isok) {
	if (!_isok && AfxMessageBox(_T("发现错误是否继续？"),
		MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return false;
	}
	return true;
}
