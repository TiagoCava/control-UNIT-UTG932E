/********************************************************************
	created:	2014/12/22	
	author:		M.Yang
	purpose:	UCI(united communicate interface)
*********************************************************************/

#ifndef uci_h__
#define uci_h__

#include "ucidef.h"

//////////////////////////////////////////////////////////////////////////
// 关于接口返回的状态值：
//    < 0 : 表示错误； 
//    错误码对应的错误信息可以通过uci_GetLastError获取,错误码的定义在ucidef.h文件中。
//    其中 < UCI_ERR的错误码为libusb的错误码。
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
namespace uci {
extern "C"{
#endif
	//@brief : 执行与具体会话无关的查询操作
	//@param PRParams _params  : 查询操作的参数
	//@param u_byte * _data    : 输出数据的缓冲区地址
	//@param u_size _dataCount : 输出数据的缓冲区大小（Bytes）
	//@return u_status :  查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_Query(_in_out PRParams _params, _in_out u_byte* _data, _in u_size _dataCount);

	//@brief : 查询所有满足条件的通信节点
	//@param _in const QParams * _params : 查询参数 
	//@param _out Node * _outBuf         : 存放查询到的节点数据
	//@param _in_out u_size * _nodeCnt   : 入参为要查获取的节点数量，出参为实际读取到的节点数量。
	//@param _in u_size _timeOut         : 查询超时
	//@return u_status  : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks :	
	u_status _UCIAPI uci_QueryNodes(_in const QParams* _params, _out Node* _outBuf, 
									_in_out u_size* _nodeCnt, _in u_size _timeOut);

	//@brief : 查询所有满足条件的通信节点
	//@param _msg        :  查询字符串，格式如：  "LAN:4162,5000;USB:0x1234&0x5345,0x7777&0x5345;"
	//@param _nodes      :  存放查询到的节点数据
	//@param _node_count :  要查询的节点个数。
	//@param _timeout    : 查询超时
	//@return  : < 0 错误码； >=0 查询到的节点（设备）个数。
	//@remarks : 
	u_status _UCIAPI uci_QueryNodesX(u_cstring _msg, Node* _nodes, u_size _node_count, _in u_size _timeout);

	//@brief : 与设备建立连接
	//@param u_cstring _addr      : 设备连接字符串， 以'\0'结尾
	//@param u_session * _session : 返回已建立的会话ID.
	//@param u_uint32 _timeOut    : 连接超时时间 ms
	//@return u_status : 查看本文件中的“关于接口返回的状态值”说明块
	// UCI_SUCCESS
	// UCI_ERR_ARGS_WRONG; 
	// UCI_ERR_RES_INIT_ERROR; 
	// UCI_ERR_CONNECT_ADDR_WRONG;
	// UCI_ERR_CONNECT_FAILED;
	// UCI_ERR_CONNECT_TIMEOUT]
	//@remarks : 见文档
	u_status _UCIAPI uci_Open(u_cstring _addr, u_session* _session, u_uint32 _timeOut);

	//@brief          : 与设备建立连接
	//@param _addr    : 设备连接字符串， 以'\0'结尾
	//@param _timeOut : 连接超时时间 ms
	//@return :  < 0 错误码； >=0 会话ID
	//@remarks : 本接口是uci_Open的简化版本。
	u_status _UCIAPI uci_OpenX(u_cstring _addr, u_uint32 _timeOut);

	//@brief : 执行命令
	//@param u_session _session : 会话ID
	//@param PCommandParams _params : 命令参数
	//@return u_status : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_SendCommand(u_session _session, PCommandParams _params);

	//@brief : 添加UCI的事件订阅
	//@param uciNotify _pNotify : 响应UCI事件的回调函数
	//@return u_status _UCIAPI  :  查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_SetNotify(UCIMSGProc _pNotify);
	
	//@brief          : 配置属性
	//@param _msg     : 属性字符串
	//@param _obj     : 属性数据缓冲区地址
	//@param _objSize : 属性数据缓冲区大小
	//@return         : 查看本文件中的“关于接口返回的状态值”说明块s
	//@remarks        : 目前只支持切换错误描述语言版本和订阅设备接入和发生更改的事件，具体请查看文档。
	u_status _UCIAPI uci_SetAttribute(u_session _sesn, u_cstring _msg, const u_object* _obj, u_size _objSize);
	
	//@brief                   : 查询属性
	//@param u_session _sesn   : 
	//@param u_attr _name      : 
	//@param u_attr_v * _value : 
	//@return u_status _UCIAPI : 
	//@remarks : 	
	u_status _UCIAPI uci_GetAttribute(u_session _sesn, u_cstring _msg, u_object* _obj, u_size _objSize);

	//@brief : 写操作
	//@param u_session _session   : 会话ID
	//@param PWParam _params      : 写操作参数
	//@param const u_byte * _data : 要写入的数据缓冲区地址， 可以为NULL
	//@param u_size _len          : 要写入的数据缓冲区的长度， 如果_buf == NULL,则_len会被忽略可以为任何值
	//@return u_status : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks :  当_data为null，表示_params.CMDString中已经包含了要写入的数据字段了，这种情况建议使用uci_FormatWrite接口；
	//            如果_data != NULL，则命令字符串_params.CMDString只能包含一条命令语句。
	u_status _UCIAPI uci_Write(u_session _session, PWParams _params, const u_byte* _data, u_size _len);

	//@brief          :  写操作
	//@param _session :  会话ID
	//@param _msg     :  命令字符串，以'\0'结尾. 具体格式见文档；
	//@param _timeout :  写超时(单位：ms)
	//@param _data    :  要写入的数据缓冲区地址， 可以为NULL
	//@param _len     :  要写入的数据缓冲区的长度， 如果_buf == NULL,则_len会被忽略可以为任何值
	//@return         :  查看本文件中的“关于接口返回的状态值”说明块
	//@remarks        :  当_data为null，表示_msg中已经包含了要写入的数据字段了，
	//                   如果_data != NULL，则命令字符串_msg只能包含一条命令语句。
	u_status _UCIAPI uci_WriteX(u_session _session, u_cstring _msg, 
		u_uint32 _timeout, const u_byte* _data, u_size _len);

	//@brief          : 只写指令字符串的写指令接口
	//@param _session : 会话ID
	//@param _msg     : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param _timeout : 写超时(单位：ms)
	//@return         : 查看本文件中的“关于接口返回的状态值”说明块	
	//@remarks        : 
	u_status _UCIAPI uci_WriteSimple(u_session _session, u_cstring _msg, u_uint32 _timeout);

	//@brief : 格式化写入数据
	//@param u_session _sesn        : 会话ID
	//@param u_uint32 _timeOut      : 写超时(单位：ms)
	//@param const u_tchar * format : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param ... : 
	//@return u_status              : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_FormatWrite(u_session _sesn, u_uint32 _timeOut, const u_tchar *format, ...);

	//@brief : 读取数据
	//@param u_session _session : 会话ID
	//@param PRParams _params   : 读操作的命令参数
	//@param u_byte * _data     : 接收数据的缓冲区
	//@param u_size _dataLen    : 接收数据的缓冲区大小，大小由协议决定。
	//@return u_status :    查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 建议使用uci_ReadX版本。
	u_status _UCIAPI uci_Read(u_session _session, PRParams _params, u_byte* _data, u_size _dataLen);

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

	//@brief : 写文件
	//@param u_session _session  : 会话ID
	//@param WFileParams * _info : 文件信息包
	//@return u_status : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_WriteFromFile(u_session _session, WFileParams* _info);

	//@brief           : 写文件
	//@param _session  : 会话ID
	//@param _msg      : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param _filePath : 要写入的文件路径（包括文件名）
	//@param _timeout  : 写超时(单位：ms)
	//@return : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 本接口是uci_WriteFromFile的简化版本。
	u_status _UCIAPI uci_WriteFromFileX(u_session _session, u_cstring _msg,
		u_cstring _filePath, u_uint32 _timeout);

	//@brief : Read data synchronously and store the transferred data in a file.
	//@param u_session _session  : 会话ID
	//@param RFileParams _params :  要存放数据的文件的路径
	//@return u_status  : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	u_status _UCIAPI uci_ReadToFile(u_session _session, RFileParams* _params);

	//@brief : 读取文件缓冲区到本地
	//@param _session   : 会话ID.
	//@param _msg       : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param _filePath  : 缓冲数据到本地磁盘的文件路径。（包括文件名和后缀）
	//可以是绝对路径也可以是相对路径。如果是相对路径可以通过_filePathFinal获取绝对路径。
	//@param _timeout   : 超时。
	//@param _filePathFinal       : 输出缓冲到本地的文件的绝对路径
	//@param _filePathFinalLength : _filePathFinal的长度，按字符计数。
	//@return : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 本接口是uci_ReadToFile的非封装版本。
	u_status _UCIAPI uci_ReadToFileX(u_session _session, u_cstring _msg,
		u_cstring _filePath, u_uint32 _timeout, 
		u_tchar *_filePathFinal, u_int32 _filePathFinalLength);

	//@brief : 关闭会话
	//@param u_session _session : 将要关闭的会话ID
	//@return u_status  : 查看本文件中的“关于接口返回的状态值”说明块
	u_status _UCIAPI uci_Close(u_session _session);

	//@brief : 返回最后一次操作对应的错误状态信息
	//@return u_cstring : 错误描述信息
	//@remarks : 可以通过uci_SetAttribute设置错误描述信息的语言版本。
	u_cstring _UCIAPI uci_GetLastError(void);

	//@brief   : 退出UCI实例时调用。		
	//@remarks : 比如在主程序退出时调用，以释放内部资源。  
	// 本接口并不是一定要使用。 只是在.NET等托管环境下，
	// 且通过uci_SetAttribute和 uci_SetNotify两个接口
	// 订阅了设备接入和移除事件时，在退出主程序时使用。
	void _UCIAPI uci_ExInstance();
#ifdef __cplusplus
}

}
#endif
#endif // uci_h__
