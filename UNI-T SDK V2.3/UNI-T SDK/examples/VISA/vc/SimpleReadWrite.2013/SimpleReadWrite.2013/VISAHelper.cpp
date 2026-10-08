/********************************************************************
 * Copyright(c) 2017-2018 UNI-T,
 *
 * All Rights Reserved.
 *
 * FileName: VisaHleper.cpp
 * Author  : M.Jiang
 * Created : 2017/09/14
 *
 * Description:
 *
 * History : 14-9-2017 16:12 by M.J Created
*********************************************************************/

#include "stdafx.h"
#include "VISAHelper.h"
#include <vector>

#pragma comment(lib,"visa32.lib")

using std::vector;
/*********************************************************
 * @version : Ver1.0 
 * @brief   : 超时判断定时器
 * @date    : 2017/11/08 16:45:26
 * @author  : M.Jiang
 * @param   : const double & start
 * @param   : const double & length
 * @return  : bool
 * @remarks : 循环体前面定义const double start=clock();然后将其做为timer的第一个参数，持续时间为第二个参数，单位ms
 ********************************************************/
bool timer(const double & start, const double& length)
{
	if (((double)clock() - start) >= length)
		return false;
	else
		return true;
}

VISAHelper::VISAHelper(): 
status(-1),
defaultRM(VI_NULL),
instrHandle(VI_NULL)
{
	
}

VISAHelper::~VISAHelper()
{

}

/******************************************************************
 * @version : Ver1.0 
 * @brief   : 搜索VISA设备
 * @date    : 2017/09/15 9:23:19
 * @author  : M.Jiang
 * @param   : ViString expression 设置类型限定字符串
 * 
			  Interface         Expression
			  --------------------------------------
			  GPIB              "GPIB[0-9]*::?*INSTR"
			  VXI               "VXI?*INSTR"
			  GPIB-VXI          "GPIB-VXI?*INSTR"
			  Any VXI           "?*VXI[0-9]*::?*INSTR"
			  Serial            "ASRL[0-9]*::?*INSTR"
			  PXI               "PXI?*INSTR"
			  USB				"USB?*"
			  All instruments   "?*INSTR"
			  All resources     "?*"
 *
 * @param   : ViChar findAddressList[][VI_FIND_BUFLEN] 设备列表
 * @param   : ViUInt32 numInstrs 需要搜索的设备数量
 * @param   : ViPUInt32 retNumInstrs 实际搜索到的设备数量
 * @return  : ViStatus 
 * @remarks : 
 ******************************************************************/
ViStatus VISAHelper::FindDev(ViString expression, ViChar findAddressList[][VI_FIND_BUFLEN], ViUInt32 numInstrs, ViPUInt32 retNumInstrs)
{
	ViFindList findList = VI_NULL;
	char instrDescriptor[VI_FIND_BUFLEN];

	/* 打开默认的VISA资源管理器 */
	status = viOpenDefaultRM(&defaultRM);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not open a session to the VISA Resource Manager!\n"));
	}
	//获得设备地址数量和资源列表句柄
	status = viFindRsrc(defaultRM, expression, &findList, retNumInstrs, instrDescriptor);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("An error occurred while finding resources\n"));
		viClose(defaultRM);
		return status;
	}

	//获得第一个地址描述符
	memcpy(findAddressList[0], instrDescriptor, strlen(instrDescriptor)+1);
	
	//根据输入的findAddressList大小填装设备地址列表
	int num = numInstrs <= *retNumInstrs ? numInstrs : *retNumInstrs;
	for (int i = 1; i < num ; i++)
	{
		status = viFindNext(findList, instrDescriptor);//搜索下一个设备地址
		memcpy(findAddressList[i], instrDescriptor, strlen(instrDescriptor)+1);
		if (status < VI_SUCCESS)
		{
			TRACE(_T("An error occurred finding the next resource\n"));
			viClose(defaultRM);
			return status;
		}
	}
	
	//释放VISA列表资源
	status = viClose(findList);
	findList = VI_NULL;

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 打开仪器
 * @date    : 2017/09/15 11:21:30
 * @author  : M.Jiang
 * @param   : ViString instrDescriptor 仪器资源描述符
 * @param   : ViUInt32 timeout 超时时间
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::OpenDev(ViString instrDescriptor, ViUInt32 timeout = 0)
{
	/* 打开默认的VISA资源管理器 */
	status = viOpenDefaultRM(&defaultRM);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not open a session to the VISA Resource Manager!\n"));
	}

	status = viOpen(defaultRM, instrDescriptor, VI_NULL, VI_NULL, &instrHandle);//timeout指定该操作在返回错误之前等待的最大时间周期(以毫秒为单位)
	if (status < VI_SUCCESS)
	{
		TRACE(_T("An error occurred opening a session to %s\n"), instrDescriptor);
	}

	return status;
}

/*********************************************************
* @version : Ver1.0
* @brief   : 打开仪器
* @date    : 2017/09/15 11:21:30
* @author  : M.Jiang
* @param   : ViString instrDescriptor 仪器资源描述符
* @param   : ViUInt32 timeout 超时时间
* @return  : ViStatus
* @remarks :
********************************************************/
ViStatus VISAHelper::OpenDev(CString instrDescriptor, ViUInt32 timeout = 0)
{
	char visaAddr[VI_FIND_BUFLEN];
	memset(visaAddr, 0, sizeof(visaAddr));
	memcpy(visaAddr, CStringA(instrDescriptor), CStringA(instrDescriptor).GetLength());

	/* 打开默认的VISA资源管理器 */
	status = viOpenDefaultRM(&defaultRM);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not open a session to the VISA Resource Manager!\n"));
	}

	status = viOpen(defaultRM, visaAddr, VI_NULL, timeout, &instrHandle);//VISA打开超时指定“VISA打开”在返回错误前等待的最大超时值，以毫秒为单位。它不设置I/O超时。VISA设置超时指定VISA会话句柄之后的操作超时值
	if (status < VI_SUCCESS)
	{
		TRACE(_T("An error occurred opening a session to %s\n"), instrDescriptor);
	}
	
	status = viLock(instrHandle, VI_EXCLUSIVE_LOCK, 2000, NULL, NULL);
	status = viSetAttribute(instrHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE);
	status = viSetAttribute(instrHandle, VI_ATTR_SEND_END_EN, VI_TRUE);//使能发送END指示符，与写操作有关,一般为‘\0’

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 只写操作
 * @date    : 2017/09/15 11:22:02
 * @author  : M.Jiang
 * @param   : ViBuf buf 写缓冲
 * @param   : ViUInt32 cnt 想要写入字节数
 * @param   : ViPUInt32 retCnt 实际写入的字节数
 * @param   : ViUInt32 timeout 超时时间
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::Write(ViBuf buf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout)
{
	status = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, timeout);
	status = viWrite(instrHandle, buf, cnt, retCnt);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Error writing to the device\n"));
	}

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 只读操作
 * @date    : 2017/09/15 11:24:14
 * @author  : M.Jiang
 * @param   : ViPBuf buf 读缓冲
 * @param   : ViUInt32 cnt 想要读取字节数
 * @param   : ViPUInt32 retCnt 实际读取的字节数
 * @param   : ViUInt32 timeout 超时时间
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::Read(ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout)
{
	status = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, timeout);

	status = viRead(instrHandle, buf, cnt, retCnt);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Error reading a response from the device\n"));
	}
	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 
 * @date    : 2017/10/24 11:16:21
 * @author  : M.Jiang
 * @param   : ViPBuf writeBuf	写入的指令
 * @param   : ViUInt32 writeCnt 写入的指令长度
 * @param   : ViPBuf readBuf	读缓冲
 * @param   : ViUInt32 readCnt	想要读取字节数
 * @param   : ViPUInt32 retCnt	实际读取的字节数
 * @param   : ViUInt32 timeout	超时时间
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::Query(ViPBuf writeBuf, ViUInt32 writeCnt, ViPBuf readBuf, ViUInt32 readCnt, ViPUInt32 retCnt, ViUInt32 timeout)
{
	ViUInt32 retWCnt;

	status = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, timeout);
	status = viWrite(instrHandle, writeBuf, writeCnt, &retWCnt);
	Sleep(50);
	status = viRead(instrHandle, readBuf, readCnt, retCnt);

	if (status < VI_SUCCESS)
	{
		TRACE(_T("Error query from the device\n"));
	}

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 请求返回数据(先写后读)
 * @date    : 2017/09/15 11:25:01
 * @author  : M.Jiang
 * @param   : ViString writeBuf 写入的指令
 * @param   : ViPBuf readBuf 读缓冲
 * @param   : ViUInt32 cnt 想要读取字节数
 * @param   : ViPUInt32 retCnt 实际读取的字节数
 * @param   : ViUInt32 timeout 超时时间
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::Query(ViString writeBuf, ViPBuf readBuf, ViUInt32 cnt, ViPUInt32 retCnt, ViUInt32 timeout)
{
	ViUInt32 writeCount;

	status = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, timeout);

	status = viWrite(instrHandle, (ViBuf)writeBuf, (ViUInt32)strlen(writeBuf), &writeCount);
	status = viRead(instrHandle, readBuf, cnt, retCnt);

	if (status < VI_SUCCESS)
	{
		TRACE(_T("Error query from the device\n"));
	}

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 异步处理回调函数
 * @date    : 2017/09/19 9:38:12
 * @author  : M.Jiang
 * @param   : ViSession vi
 * @param   : ViEventType etype
 * @param   : ViEvent event
 * @param   : ViAddr userHandle
 * @return  : ViStatus _VI_FUNCH
 * @remarks : 
 ********************************************************/
ViStatus _VI_FUNCH AsyncHandler(ViSession vi, ViEventType etype, ViEvent event, ViAddr userHandle)
{
	ViUInt32 retCount = 0;
	viGetAttribute(event, VI_ATTR_RET_COUNT, &retCount);
	return VI_SUCCESS;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 异步IO安装
 * @date    : 2017/09/19 9:38:52
 * @author  : M.Jiang
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::AsyncInstall()
{
	ViAddr userHandle = nullptr;

	status = viInstallHandler(instrHandle, VI_EVENT_IO_COMPLETION, AsyncHandler, userHandle);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not install the interrupt handler\n"));
	}
	status = viEnableEvent(instrHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_NULL);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not enable the interrupt event\n"));
	}

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 异步IO卸载
 * @date    : 2017/09/19 9:39:09
 * @author  : M.Jiang
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::AsyncUnInstall()
{
	status = viUninstallHandler(instrHandle, VI_EVENT_IO_COMPLETION, AsyncHandler, VI_NULL);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Could not uninstall the interrupt handler\n"));
	}
	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 异步读取，jobId是每一个异步操作的指示ID
 * @date    : 2017/09/19 9:39:29
 * @author  : M.Jiang
 * @param   : ViPBuf buf 读取缓存
 * @param   : ViUInt32 cnt 读取个数
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::AsyncRead(ViPBuf buf, ViUInt32 cnt)
{
	ViJobId jobId;
	
	status = viReadAsync(instrHandle, buf, cnt, &jobId);
	if (status < VI_SUCCESS)
	{
		TRACE(_T("Error Async Read\n"));
	}

	return status;
}

/*********************************************************
 * @version : Ver1.0 
 * @brief   : 关闭仪器并释放相关资源
 * @date    : 2017/09/15 11:28:10
 * @author  : M.Jiang
 * @return  : ViStatus
 * @remarks : 
 ********************************************************/
ViStatus VISAHelper::CloseDev()
{
	status = VI_SUCCESS;

	//释放仪器资源
	if (instrHandle != VI_NULL) 
	{
		status = viUnlock(instrHandle);
		status = viClose(instrHandle);
		instrHandle = VI_NULL;
	}
	//释放仪器资源管理器资源
	if (defaultRM != VI_NULL) 
	{
		status = viClose(defaultRM);
		defaultRM = VI_NULL;
	}

	return status;
}


int VISAHelper::Write(const TCHAR* _cmd, UINT _timeOut)
{
	CStringA cmd = CStringA(_cmd);
	char* pCmd = cmd.GetBuffer();

	int ret = 0;
	DWORD retCnt = 0;
	
	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	ret = viWrite(instrHandle, (byte*)pCmd, cmd.GetLength(), &retCnt);

	cmd.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}
int VISAHelper::FormatWrite(UINT _timeOut, _In_z_ _FormatMessage_format_string_ const TCHAR *_pszFormat, ...)
{
	CStringA pszFormat = CStringA(_pszFormat);
	char* ppszFormat = pszFormat.GetBuffer();

	ppszFormat[pszFormat.GetLength()] = '\0';

	va_list argList;

	int ret = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	va_start(argList, _pszFormat);
	ret = viVPrintf(instrHandle, ppszFormat, argList);
	va_end(argList);

	pszFormat.ReleaseBuffer();

	return ret;
}
int VISAHelper::FormatVWrite(UINT _timeOut, const TCHAR *_pszFormat, va_list _argList)
{
	CStringA pszFormat = CStringA(_pszFormat);
	char* ppszFormat = pszFormat.GetBuffer();

	ppszFormat[pszFormat.GetLength()] = '\0';

	int ret = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	ret = viVPrintf(instrHandle, ppszFormat, _argList);

	pszFormat.ReleaseBuffer();

	return ret;
}
int VISAHelper::WriteFile(const TCHAR* _cmd, const TCHAR* _filename, UINT _dataLen, UINT _timeOut)
{	
	CStringA fileName = CStringA(_filename);
	char* pFileName = fileName.GetBuffer();

	CStringA cmd = CStringA(_cmd);
	char* pCmd = cmd.GetBuffer();

	int ret = 0;
	DWORD retCnt = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	ret = viWrite(instrHandle, (byte*)pCmd, cmd.GetLength(), &retCnt);
	ret = viWriteFromFile(instrHandle, pFileName, _dataLen, &retCnt);

	cmd.ReleaseBuffer();
	fileName.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}

int VISAHelper::Read(const TCHAR* _cmd, void* _data, UINT _dataLen, UINT _timeOut)
{
	CStringA cmd = CStringA(_cmd);
	char* pCmd = cmd.GetBuffer();

	int ret = 0;
	DWORD retCnt = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	//ret = viFlush(instrHandle, 0x10);
	ret = viWrite(instrHandle, (byte*)pCmd, cmd.GetLength(), &retCnt);

	double start = clock();
	while (timer(start, _timeOut)){
		//ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
		ret = viRead(instrHandle, (byte*)_data, _dataLen, &retCnt);
		if (retCnt > 0) {
			break;
		}
	}
	
	cmd.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}
int VISAHelper::FormatRead(void* _data, UINT _dataLen, UINT _timeOut, _In_z_ _FormatMessage_format_string_ const TCHAR *_pszFormat, ...)
{
	CStringA pszFormat = CStringA(_pszFormat);
	char* ppszFormat = pszFormat.GetBuffer();

	ppszFormat[pszFormat.GetLength()] = '\0';

	va_list argList;

	int ret = 0;
	DWORD retCnt = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	va_start(argList, _pszFormat);
	ret = viVPrintf(instrHandle, ppszFormat, argList);
	va_end(argList);
	ret = viRead(instrHandle, (byte*)_data, _dataLen, &retCnt);

	pszFormat.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}
int VISAHelper::FormatVRead(void* _data, UINT _dataLen, UINT _timeOut, const TCHAR *_pszFormat, va_list _argList)
{
	CStringA pszFormat = CStringA(_pszFormat);
	char* ppszFormat = pszFormat.GetBuffer();

	ppszFormat[pszFormat.GetLength()] = '\0';

	int ret = 0;
	DWORD retCnt = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	//ret = viFlush(instrHandle, 0x10);
	ret = viVPrintf(instrHandle, ppszFormat, _argList);

	double start = clock();
	unsigned int count = 0;
	while (timer(start, _timeOut))
	{
		//ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
		ret = viRead(instrHandle, (byte*)_data + count, _dataLen, &retCnt);
		count += retCnt;
		if (count >= _dataLen){
			break;
		}
	}

	pszFormat.ReleaseBuffer();

	if (count == _dataLen)return count;
	else return ret;

	/*return retCnt > 0 ? retCnt : ret;*/
}
int VISAHelper::ReadFile(const TCHAR* _cmd, const TCHAR* _filename, UINT _dataLen, UINT _timeOut)
{
	CStringA fileName = CStringA(_filename);
	char* pFileName = fileName.GetBuffer();

	CStringA cmd = CStringA(_cmd);
	char* pCmd = cmd.GetBuffer();

	int ret = 0;
	DWORD retCnt = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	//ret = viFlush(instrHandle, 0x10);
	ret = viWrite(instrHandle, (byte*)pCmd, cmd.GetLength(), &retCnt);

	double start = clock();
	unsigned int count = 0;
	while (timer(start, _timeOut))
	{
		ret = viReadToFile(instrHandle, pFileName, _dataLen, &retCnt);
		count += retCnt;
		if (count == _dataLen){
			break;
		}
	}

	cmd.ReleaseBuffer();
	fileName.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}
int VISAHelper::FormatQuery(UINT _timeOut, const TCHAR *_cmd, _In_z_ _FormatMessage_format_string_ const TCHAR *_readFmt, ...)
{
	CStringA cmd = CStringA(_cmd);
	char* pcmd = cmd.GetBuffer();
	pcmd[cmd.GetLength()] = '\0';

	CStringA readFmt = CStringA(_readFmt);
	char* preadFmt = readFmt.GetBuffer();
	preadFmt[readFmt.GetLength()] = '\0';
	
	va_list argList;

	int ret = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);

	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	va_start(argList, _readFmt);
	ret = viVQueryf(instrHandle, pcmd, preadFmt, argList);
	va_end(argList);

	cmd.ReleaseBuffer();
	readFmt.ReleaseBuffer();

	return ret;
}
int VISAHelper::FormatVQuery(UINT _timeOut, const TCHAR *_cmd, const TCHAR *_readFmt, va_list _argList)
{
	CStringA cmd = CStringA(_cmd);
	char* pcmd = cmd.GetBuffer();
	pcmd[cmd.GetLength()] = '\0';

	CStringA readFmt = CStringA(_readFmt);
	char* preadFmt = readFmt.GetBuffer();
	preadFmt[readFmt.GetLength()] = '\0';

	int ret = 0;

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	ret = viVQueryf(instrHandle, pcmd, preadFmt, _argList);

	cmd.ReleaseBuffer();
	readFmt.ReleaseBuffer();

	return ret;
}

int VISAHelper::ReadFileNoIEEEBlock(const TCHAR* _cmd, const TCHAR* _filename , UINT _timeOut)
{
	CStringA fileName = CStringA(_filename);
	char* pFileName = fileName.GetBuffer();

	CStringA cmd = CStringA(_cmd);
	char* pCmd = cmd.GetBuffer();

	int ret = 0;
	DWORD retCnt = 0;
	byte tmp[9];

	CSingleLock slIO(&m_csAccessSession, TRUE);
	ret = viSetAttribute(instrHandle, VI_ATTR_TMO_VALUE, _timeOut);
	ret = viWrite(instrHandle, (byte*)pCmd, cmd.GetLength(), &retCnt);

	/*解析IEEEBlock数据包头*/
	ret = viRead(instrHandle, tmp, 2, &retCnt);//首先读取"#+长度位数"共2个字节
	if (ret < 0) return ret;
	int lengthLen = tmp[1] - '0';//然后获得数据长度所占的字节数,其值不大于9
	ret = viRead(instrHandle, tmp, lengthLen, &retCnt);//读取数据长度字符串
	if (ret < 0) return ret;
	tmp[lengthLen] = '\0';//字符串结束符
	int dataLen = atoi((char*)tmp);//获得要读取的有效数据长度
	
	vector<byte> buf;
	buf.resize(dataLen);
	
	double start = clock();
	unsigned int count = 0;
	while (timer(start, _timeOut))
	{
		ret = viRead(instrHandle, &buf[0] + count, dataLen, &retCnt);
		count += retCnt;
		if (count == dataLen){
			break;
		}
	}

	//表示读到完整的数据流并写入文件
	if (count == dataLen){
		CFile f(CString(pFileName), CFile::modeCreate | CFile::modeWrite);
		f.Write(&buf[0], dataLen);
		f.Close();
	}

	//释放内存
	buf.clear();
	buf.shrink_to_fit();
	cmd.ReleaseBuffer();
	fileName.ReleaseBuffer();

	return retCnt > 0 ? retCnt : ret;
}