/********************************************************************
	created:	2014/12/23
	author:		M.Yang
	purpose:	UCI库的全局定义文件
	modify:
	*********************************************************************/
#ifndef ucidef_h__
#define ucidef_h__

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(_NI_mswin16_)
#define _UCIAPI   __stdcall
#define _UCICallBack   __stdcall
#endif

#define _in
#define _out
#define _in_out

#ifdef __cplusplus
namespace uci {
#endif // __cplusplus

#ifdef _CVI_
#define u_inline static
#else
#define u_inline inline
#endif // _CVI_

#ifndef MAX_PATH
#define MAX_PATH          260
#endif // !MAX_PATH
	
	//////////////////////////////////////////////////////////////////////////
	//{                       基础数据类型定义
#ifdef _UNICODE
	//@brief :   表示一个 Unicode 字符。
#define u_tchar       wchar_t
#else
	//@brief :   表示一个 ASCII 字符。
#define u_tchar       char
#endif // _UNICODE

#if defined(_WIN64)
	typedef unsigned __int64 u_unit_ptr;
	typedef __int64 u_long_ptr;
#else
	typedef unsigned int u_unit_ptr;
	typedef long u_long_ptr;
#endif   

	//@brief : Boolean型
#define u_bool       bool
	//@brief : 8位无符号整数
#define u_byte       unsigned char
	//@brief : 8位有符号整数
#define u_char       char
	//@brief : 不指定数据类型的缓冲区指针
#define u_buf        void*
	//@brief : 对象类型的抽象类型
#define u_object     void
	//@brief : 表示 16 位有符号整数。
#define u_short      short
	//@brief : 表示 16 位无符号整数。
#define u_ushort     unsigned short
	//@brief : 表示 16 位有符号整数。
#define u_int16      u_short
	//@brief : 表示 16 位无符号整数。
#define u_uint16     u_ushort
	//@brief : 表示 32 位有符号整数。
#define u_int32      int
	//@brief : 表示 32 位无符号整数。
#define u_uint32     unsigned int
	//@brief : 表示UCI接口返回的状态值(32位有符号数)
#define u_status     u_int32
	//@brief : 表示大小，32位无符号数
#define u_size       u_uint32
	//@brief : 表示会话ID，32位无符号数
#define u_session    u_uint32
	//@brief : 表示字符串类型
#define u_string     u_tchar*
	//@brief : 表示只读字符串类型
#define u_cstring    const u_tchar*
	//@brief : 表示属性ID类型，32位无符号数
#define u_attr       u_int32
	//@brief : 表示属性值类型，32位无符号数
#define u_attr_v     u_int32
	//@brief : 表示一个16位无符号类型的单字
#define u_word       u_ushort
	//@brief : 表示一个32位无符号类型的双字
#define u_dword      u_uint32
	//@brief : 消息参数
#define u_wparam     u_unit_ptr
	//@brief : 消息参数
#define u_lparam     u_long_ptr
	//}

	//////////////////////////////////////////////////////////////////////////
	//{                       接口返回状态值定义
	//@brief : 操作成功
#define UCI_SUCCESS                  (0)
	//@brief : 定义错误码的起始值
#define UCI_ERR                      (-1000)
	//@brief : 判断当前接口是否执行失败
	//@brief : 判断当前接口是否执行成功
#define UCIERR(r)                    (r < 0)
#define UCISUCCESS(r)                (!UCIERR(r))

	//@brief : 定义错误码
	typedef enum _UCIErr {
		NoError = 0,

		//@brief : 资源初始化错误
		InitResourceError = UCI_ERR - 20,
		//@brief : 无效的会话
		Invalid_Session,
		//@brief : 操作超时 
		Timeout,
		//@brief : 操作失败
		Failed,
		//@brief : 不支持的操作
		Unsupported,
		//@brief : 内存空间不足
		Insufficient_Memory,
		//@brief : 系统繁忙，无法响应
		Busy,
		//@brief : 通信异常，不可逆！
		COMExpection,
		//@brief : 通道未打开！
		ChannelNotOpened,
		//@brief : WinAPI 错误
		API_Failed,

		//@brief : 未知的错误
		Unknown = UCI_ERR - 40 - 1,

		//@brief : 建立连接的字符串地址格式错误
		Connect_InvalidAddress = UCI_ERR - 40,
		//@brief : 连接还未建立
		Connect_NotBuild,
		//@brief : 连接已断开
		Connect_Break,
		//@brief   : 不支持的通信方式
		Connect_Unsupported_COMType,

		//@brief : 未发现指定的设备
		Device_NoFound = UCI_ERR - 60,
		//@brief : 不支持的设备
		Device_Unsupported,
		//@brief : 需要先执行查询设备的操作
		Device_QueryFirst,
		//@brief : 机型不匹配
		Device_NotMatch,

		//@brief : 查询网络设备失败
		Query_LANNodesFailed,

		//@brief : USB设备地址只在执行查询设备操作后才有效，
		//@remarks : 从UCI库导出的USB设备地址，只是一个列表索引，不是真正的设备地址。
		AddrValid_AfterQueryDeviceOper,

		//@brief   : 未发现U盘接入
		UDisk_NotFound,
		//@brief   : 按键已经锁定
		Key_Locked,

		//@brief : 命令字符串格式错误
		CMD_Invalid_StringFormat = UCI_ERR - 80,
		//@brief : 只支持单条命令
		CMD_OnlySupportSingle,
		//@brief : 一条命令只支持一个属性
		CMD_OnlySupportSingleAttr,
		//@brief : 不支持的命令
		CMD_Unsupported,
		//@brief : 发送命令失败
		CMD_SendFailed,
		//@brief : 协议数据格式错误
		CMD_Invalid_ProtocolFormat,
		//@brief : 设备端写文件到flash失败！
		CMD_WriteFileToFlash_Failed,
		//@brief   : 未发现有效的回复数据
		CMD_NoFound_Valid_Reply_Data,
		//@brief   : 命令消息错误，请查看命令参数是否符合协议
		CMD_Error_Message,
		//@brief   : 无效的表达式
		CMD_Invalid_Expression,
		//@brief : 在当前调制模式下不可以加载任意波形文件
		CMD_Cannot_LoadARB_InCurrent_MOD_Mode,

		//@brief : 参数错误
		Args_Invalid = UCI_ERR - 100,
		//@brief : 参数提供的空间太小
		Args_MemoryTooSmall,
		//@brief : 所提供的文件名太长(最长50个字节)
		Args_FileNameTooLong,
		//@brief : 参数所给的数据大小与协议不匹配(防止数据错误)
		Args_DataLenNotMatch,

		//@brief : 数字溢出
		Data_Overflow = UCI_ERR - 120,
		//@brief : 超出范围
		Data_OutRange,
		//@brief : 数据未全部读取完
		Data_NotReadEnoughLenth,
		//@brief : 数据校验失败
		Data_ECC_Failed,
		//@brief   : 无效的数据
		Data_Invalid,
		//@brief   : 数据压缩失败
		Data_Zip_Error,
		//@brief   : 数据解压缩失败
		Data_UnZip_Error,
		//@brief   : 数据传输失败
		Data_Transfer_Error, 
		//@brief   : 数据传输失败
		Data_Transfer_Break,
		//@brief   : 无有效数据可读取
		Data_No_Data_Incoming,
		
		//@brief : 
		File_AccessDenied = UCI_ERR - 140,
		//@brief : An unspecified error occurred.
		File_GenericException,
		//@brief : The file could not be located.	
		File_NotFound,
		//@brief : All or part of the path is invalid.
		File_BadPath,
		//@brief : The permitted number of open files was exceeded.
		File_TooManyOpenFiles,
		//@brief : There was an attempt to use an invalid file handle 
		//      or The file format is unsupported!
		//      or The file data is error!
		File_InvalidFile,
		//@brief : The current working directory cannot be removed
		File_RemoveCurrentDir,
		//@brief : There are no more directory entries
		File_DirectoryFull,
		//@brief : There was an error trying to set the file pointer
		File_BadSeek,
		//@brief : There was a hardware error
		File_HardIO,
		//@brief : SHARE.EXE was not loaded, or a shared region was locked
		File_SharingViolation,
		//@brief : There was an attempt to lock a region that was already locked
		File_LockViolation,
		//@brief : The disk is full 
		File_DiskFull,
		//@brief : The end of file was reached
		File_EndOfFile,
		//@brief : 写文件到磁盘失败
		File_SaveToDiskFailed,
		//@brief : 文件长度超出范围
		File_Length_OutOfRange,
	}UErr;
	//}
	//////////////////////////////////////////////////////////////////////////

	//@brief : 错误的会话ID
#define INVALID_SESSION  ((u_uint32)(-1))

	////////////////////////////////////////////////////////////////////////// 
	//{                        属性
	//切换中英文：   "lang:zh-hans" or "lang:en-us"
	//使能USB设备移除和连接检测机制 ： "devchange:1"
//}


#define CACHE_LINE  1
#define CACHE_ALIGN   __declspec(align(CACHE_LINE))

	//@brief : 设置结构体的对齐方式
#define PACK_ALIGN(n) __declspec(align(n))


#ifdef __cplusplus
	extern "C"{
#endif // __cplusplus

		//one Word is 2 bytes
		u_inline u_word UCI_HIWORD(u_dword _dw) { return ((u_word)((_dw >> 16) & 0xffff)); }
		u_inline u_word UCI_LOWORD(u_dword _dw) { return ((u_word)(_dw & 0xffff)); }

		//#pragma pack(show)// C4810
		//ensure byte-packed structures（1字节对齐）
#include <PshPack1.h>//如果编译环境中没有该文件，请替换为当前平台的字节对齐语句。

		//@brief : 读数据接口的参数包
		//@remarks : 普通读数据命令可通过uci_CreateRParams接口创建
		typedef struct _RParams {
			//@brief : 命令字符串，以'\0'结尾. 具体格式见具体协议文档；	
			u_cstring CMDString;
			//@brief : 要返回的数据量，具体意义视具体命令而定，通常和要读取\查询的数据是对应的；	
			u_uint32  RetCount;
			//@brief : 读超时		
			u_uint32  Timeout;
			//@brief : 附加数据
			//@remarks : 普通操作不使用，可空置;在查询设备时用到，
			//数据类型为QParams，其它以文档为准！
			u_buf     ExtraData; //reserve		
			//@brief : 附加数据的长度	
			u_size    ExtraDataLen;
		}RParams, *PRParams;

#ifdef __cplusplus
		inline RParams* uci_CreateRParams(RParams& _p, u_cstring _cmd,
										  u_uint32 _timeout, u_buf _ExtraData,
										  u_size _ExtraDataLen) {
			memset(&_p, 0, sizeof(_p));
			_p.CMDString = _cmd;
			_p.Timeout = _timeout;
			_p.ExtraData = _ExtraData;
			_p.ExtraDataLen = _ExtraDataLen;
			return &_p;
		}
#else
		u_inline RParams* uci_CreateRParams(RParams* _p, u_cstring _cmd,
			u_uint32 _timeout, u_buf _ExtraData,
			u_size _ExtraDataLen) {
			memset(_p, 0, sizeof(*_p));
			_p->CMDString = _cmd;
			_p->Timeout = _timeout;
			_p->ExtraData = _ExtraData;
			_p->ExtraDataLen = _ExtraDataLen;
			return _p;
		}
#endif

		typedef struct _DeviceIOParams {
			u_int32  Count;
			u_int32* Data;
		}DeviceIOParams;

		typedef struct _QParams {
			//@brief : 要查询的通信节点类型
			//@remarks : 从enum NodeType 取值！ 通过标志位存放类型值
			//@eg ： NodeType::USB | NodeType::LAN
			u_int32   Type;
			//@brief : 端口集端口数量
			u_int32   PortCount;
			//@brief : 端口集		
			u_int32*  Ports;
			//@brief : PVID集的PVID的个数
			//@remarks : 
			u_int32   PVIDCount;
			//@brief : PID 和 VID集
			//@remarks : 使用MakePVID创建，GetPID和GetVID获取
			u_int32*  PVID;
			//@brief : 查询字符串，nullptr时表示查询所有设备，否则查询指定设备名的设备。
			u_cstring Msg;
		}QParams, *PQParams;//@brief : 查询设备时的配置参数

		u_inline u_int32 MakePVID(u_ushort _pid, u_ushort _vid) { return ((_pid << 16) | (_vid)); };

		u_inline u_ushort GetPID(u_int32 _pvid) { return UCI_HIWORD(_pvid); }

		u_inline u_ushort GetVID(u_int32 _pvid) { return UCI_LOWORD(_pvid); }

#ifdef __cplusplus
		inline QParams* UCI_CreateQParam(QParams& _qp, u_int32 _type,
										 u_int32* _ports, u_int32 _port_cnt,
										 u_int32* _pvid, u_int32 _pvid_cnt, u_cstring _msg) {
			memset(&_qp, 0, sizeof(_qp));
			_qp.Type = _type;
			_qp.Ports = _ports;
			_qp.PortCount = _port_cnt;
			_qp.PVID = _pvid;
			_qp.PVIDCount = _pvid_cnt;
			_qp.Msg = _msg;
			return &_qp;
		}
#else
		u_inline QParams* UCI_CreateQParam(QParams* _qp, u_int32 _type,
			u_int32* _ports, u_int32 _port_cnt,
			u_int32* _pvid, u_int32 _pvid_cnt, u_cstring _msg) {
			memset(_qp, 0, sizeof(*_qp));
			_qp->Type = _type;
			_qp->Ports = _ports;
			_qp->PortCount = _port_cnt;
			_qp->PVID = _pvid;
			_qp->PVIDCount = _pvid_cnt;
			_qp->Msg = _msg;
			return _qp;
		}
#endif
		//@brief : 写数据接口参数包
		//@remarks : 可通过接口uci_CreateWParams创建
		typedef struct _WParams {
			//@brief : 命令字符串，以'\0'结尾. 具体格式见文档；	
			u_cstring CMDString;
			//@brief : 返回的数据量数据，具体意义视具体命令而定；	
			u_uint32  RetCount;
			//@brief : 读超时		
			u_uint32  Timeout;
		}WParams, *PWParams;

#ifdef __cplusplus
		u_inline WParams* uci_CreateWParams(WParams& _p, u_cstring _cmd, u_uint32 _timeout) {
			memset(&_p, 0, sizeof(_p));
			_p.CMDString = _cmd;
			_p.Timeout = _timeout;
			return &_p;
		}
#else
		u_inline WParams* uci_CreateWParams(WParams* _p, u_cstring _cmd, u_uint32 _timeout) {
			memset(_p, 0, sizeof(*_p));
			_p->CMDString = _cmd;
			_p->Timeout = _timeout;
			return _p;
		}
#endif
		typedef struct _CommandParams {
			//@brief : 命令字符串，以'\0'结尾. 具体格式见文档；	
			u_cstring CMDString;
			//@brief : 命令参数1
			u_uint32  Param1;
			//@brief : 命令参数1
			u_uint32  Param2;
			//@brief : 超时		
			u_uint32  Timeout;
		}CommandParams, *PCommandParams;

		//@brief : 写文件操作的参数包
		typedef struct _WFileParams {
			//@brief : 命令字符串，以'\0'结尾. 具体格式见文档；	
			u_cstring  CMDString;
			//@brief : 要写入的文件路径（包括文件名）
			u_cstring  FilePath;
			//@brief : 写超时(单位：ms)
			u_uint32   Timeout;
		}WFileParams, *PWFileParams;

		//@brief : 读文件接口的参数包
		typedef struct _RFileParams {
			//@brief : 命令字符串，以'\0'结尾. 具体格式见文档；	
			u_cstring   CMDString;
			//@brief : 缓冲数据到本地磁盘的文件路径。（包括文件名和后缀）
			u_cstring   FilePath;
			//@brief : 读超时(单位：ms)
			u_uint32    Timeout;
			//@brief : 最终输出文件的路径
			u_tchar     FilePathFinal[MAX_PATH];
		}RFileParams, *PRFileParams;

		//@brief : 通信节点类型
		//@remarks : 在QParams中是按位与，在Node中取的是enum值。
		typedef enum _NodeType{
			LAN = 0x0001,
			USB = 0x0010,
		}NodeType;

		//@brief : USB设备的描述符
		//@remarks : 
		typedef struct _USBDescriptor {
			//@brief : PID
			u_ushort  PID;
			//@brief : VID
			u_ushort  VID;
			//@brief : 地址	
			u_ushort  Addr;
		}USBDescriptor;

		//@brief : IP地址
		//@remarks : 填充顺序为 f1(192).f2(168).f3(1).f4(253) - 小端模式
		typedef union _IPAddr {
			struct { u_byte f1, f2, f3, f4; } Field;
			u_int32 Addr;
		}u_IPAddr;

		//@brief : LAN口通信的设备描述符
		//@remarks : 
		typedef struct _LANDescriptor {
			//@brief : IP地址（字符串类型）		
			u_tchar    IP[16];
			//@brief : IP地址
			u_IPAddr   Addr;
			//@brief : 网络端口
			//@remarks : TCPIP连接用的端口号
			u_ushort   Port;
		}LANDescriptor;

		//@brief : 查询到的设备节点的参数
		//@remarks : 
		typedef struct _Node {
			//@brief : 通信接口方式
			NodeType Type;
			//@brief : 设备名(机型名)
			//@remarks : 在本UCI库导出时，该名称为UCI库协议一致的名称，
			//			 在已出厂机型未统一设置内置名称时，UCI库内部自动匹配为UCI协议设置的名字！
			u_tchar   Name[50];
			//@brief : 设备类型
			//@remarks : 信号源 = SG, 示波器 = DSO;
			u_tchar   DevType[10];
			//@brief : LAN口参数		
			LANDescriptor LAN;
			//@brief : USB接口参数		
			USBDescriptor USB;
			//@brief : 连接字符串
			u_tchar   UCIAddr[256];
			//@brief : 序列号
			u_tchar   SN[50];
			//@brief : 设备状态
			u_status  Status;
			//@brief : 设备实际显示名称
			u_tchar   IDN[20];
		}Node, *PNode;


		//@brief : 查询到的设备节点的参数
		//@remarks : 
		typedef struct _NodeEx {
			//@brief : 通信接口方式
			NodeType Type;
			//@brief : 设备名(机型名)
			//@remarks : 在本UCI库导出时，该名称为UCI库协议一致的名称。
			u_tchar   Name[50];
			//@brief : 设备类型
			//@remarks : 信号源 = SG, 示波器 = DSO;
			u_tchar   DevType[10];
			//@brief : LAN口参数		
			LANDescriptor LAN;
			//@brief : USB接口参数		
			USBDescriptor USB;
			//@brief : 连接字符串
			u_tchar   UCIAddr[256];
			//@brief : 序列号
			u_tchar   SN[50];
			//@brief : 设备状态
			u_status  Status;
			//@brief : 设备实际显示名称
			u_tchar   IDN[20];
		}NodeEx, *PNodeEx;

		typedef struct _UCIMSG {
			u_session   Session;
			u_uint32    Message;
			u_wparam    wParams;
			u_lparam    lParams;
			u_byte      Reserved[240];
		}UCIMSG;//256Bytes

		//////////////////////////////////////////////////////////////////////////	
		typedef enum _uci_msg {
			//@brief   : 与设备建立的连接已经断开
			//@remarks : 目前只支持LAN口访问的连接的侦测
			// wParams LAN : 端口号(十进制)；
			// Reserved LAN : IP 地址
			UMSG_CONNECT_CLOSED = 1,
			//@brief   : 文件传输消息
			//@remarks :  
			// wParams 总帧数； 
			// lParams 帧计数； -1 ： 开始传输； [0,wParams)传输中， 等于wParams传输结束
			//-> UCIMSGProc return -1 ： 表示中断传输.
			UMSG_FILE_TRANSFER = 2,
			//@brief   : USB插拔消息
			//@remarks :  
			// wParams : 使用低16位，其中，高8位为pid，低8位为vid；
			// lParams : 事件类型， 移除 = 0， 插入 = 1；
			UMSG_DEVICE_NOTIFY = 3,
		}EUCIMSG;

		typedef int(__stdcall *UCIMSGProc)(UCIMSG* _msg);
#include <poppack.h>

#ifdef __cplusplus
	}
#endif//__cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ucidef_h__