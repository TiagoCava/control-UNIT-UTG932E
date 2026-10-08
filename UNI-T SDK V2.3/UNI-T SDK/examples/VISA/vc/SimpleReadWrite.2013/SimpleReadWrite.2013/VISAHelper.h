/********************************************************************
 * Copyright(c) 2017-2018 UNI-T,
 *
 * All Rights Reserved.
 *
 * FileName: VisaHleper.h
 * Author  : M.Jiang
 * Created : 2017/09/14
 *
 * Description:VISA帮助类，采用单例模式设计
 *
 * History : 14-9-2017 16:11 by M.J Created
*********************************************************************/
#include "./include/visa.h"
#include "Singleton.h"

#pragma once

class VISAHelper
{
public:
	VISAHelper();
	~VISAHelper();

private:
	ViStatus status;
	ViSession defaultRM;
	ViSession instrHandle;

public:
	ViSession GetInstrHandle() { return instrHandle; }

public:
	ViStatus FindDev(ViString expression, ViChar findAddressList[][VI_FIND_BUFLEN], ViUInt32 numInstrs, ViPUInt32 retNumInstrs);
	ViStatus OpenDev(ViString instrDescriptor, ViUInt32 timeout);
	ViStatus OpenDev(CString instrDescriptor, ViUInt32 timeout);
	ViStatus CloseDev();
	
	//同步读写API
	//viRead是同步执行的方法，直到满足以下某个条件才会返回：
	//<1>超时
	//<2>接收到希望的字节数
	//<3>接收到终结字符或END指示符
	ViStatus Write(ViBuf buf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout);
	ViStatus Read(ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout);
	ViStatus Query(ViPBuf writeBuf, ViUInt32 writeCnt, ViPBuf readBuf, ViUInt32 readCnt, ViPUInt32 retCnt, ViUInt32 timeout);
	ViStatus Query(ViString writeBuf, ViPBuf readBuf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout);
	
	//适配接口
	int Write(const TCHAR* _cmd, UINT _timeOut = 2000);
	int FormatWrite(UINT _timeOut, _In_z_ _FormatMessage_format_string_ const TCHAR *_pszFormat, ...);//可变参数如果是字符串类型，必须为char类型字符
	int FormatVWrite(UINT _timeOut, const TCHAR *_pszFormat, va_list _argList);
	int WriteFile(const TCHAR* _cmd, const TCHAR* _filename, UINT _dataLen = 0, UINT _timeOut = 2000);//不具备解析文件路径，必需输入当前运行目录下的文件
	int Read(const TCHAR* _cmd, void* _data, UINT _dataLen = 0, UINT _timeOut = 2000);
	int FormatRead(void* _data, UINT _dataLen, UINT _timeOut, _In_z_ _FormatMessage_format_string_ const TCHAR *_pszFormat, ...);//可变参数如果是字符串类型，必须为char类型字符
	int FormatVRead(void* _data, UINT _dataLen, UINT _timeOut, const TCHAR *_pszFormat, va_list _argList);
	int ReadFile(const TCHAR* _cmd, const TCHAR* _filename, UINT _dataLen = 0, UINT _timeOut = 2000);
	int FormatQuery(UINT _timeOut, const TCHAR *_cmd, _In_z_ _FormatMessage_format_string_ const TCHAR *_readFmt, ...);//可变参数如果是字符串类型，必须为char类型字符
	int FormatVQuery(UINT _timeOut, const TCHAR *_cmd, const TCHAR *_readFmt, va_list _argList);

	int ReadFileNoIEEEBlock(const TCHAR* _cmd, const TCHAR* _filename, UINT _timeOut = 2000);

	//异步读写API
	ViStatus AsyncInstall();
	ViStatus AsyncUnInstall();
	ViStatus AsyncRead(ViPBuf buf, ViUInt32 cnt);

	CCriticalSection m_csAccessSession;//用于线程间同步读写，析构时自动释放锁。
};

//VISA帮助类的全局访问接口，采用单例模式，只实例化一个对象。
namespace visa{
	static VISAHelper* GetInstance(){ return CSingleton<VISAHelper>::Instance(); };
}