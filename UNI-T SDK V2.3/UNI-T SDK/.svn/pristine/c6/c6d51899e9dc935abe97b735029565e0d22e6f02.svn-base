/*!
 * \created : 2016/12/28
 * \author  : M.Yang
 * \purpose : 定义使用用UTD2000M\UTD4000M\UTD2000CEX-EDU等系列机型使用的指令定义。
 * 具体如何使用，请查看对应机型的协议文档。
 */
#ifndef CMD_COMVer2_h__
#define CMD_COMVer2_h__

#ifdef __cplusplus
namespace comApiEx {
#endif
	static const int Invalid_CMD = -1;

#ifdef __cplusplus
	static bool IsCMDValid(short cmd) {
		return (cmd != Invalid_CMD);
	}
#else
	static int IsCMDValid(short cmd)
	{
		return (cmd == Invalid_CMD) ? 0 : 1;
	}
#endif
	
	//@brief   : 通用命令
	//@remarks : 接口参数CH须设置为-1；命令参数为Enum的数据，对应的都是shot(2Bytes)类型进行读写.
	typedef enum _commom_cmd {
		//@brief   : 获取设备类型编码
		//@remarks : {IO:R}{DATA<Enum>:comAPICommon::E_DSO_TYPE}
		CMD_GET_DSO_TYPE = 0,	
		//@brief   : 设置键盘锁
		//@remarks :  {IO:W}{DATA:null}
		CMD_LOCK_KEYPAD,
		//@brief   : 设置键盘解锁
		//@remarks :  {IO:W}{DATA:null}
		CMD_UNLOCK_KEYPAD,		
		//@brief   : 获取设备运行状态
		//@remarks : {IO:R}{DATA<Enum>:comApi::E_PROC_STATE from <comApiDef.h>}
		CMD_PROCSTATE,
		//@brief   : 执行AUTOSET命令
		//@remarks :  {IO:W}{DATA:null}
		CMD_AUTOSET,				
		//@brief   : 执行强制触发（FORCE）
		//@remarks :  {IO:W}{DATA:null}
		CMD_TRIG_FORCE,
		//@brief   : 执行单次触发（SINGLE）
		//@remarks :  {IO:W}{DATA:null}
		CMD_SINGLE,	
		//@brief   : 执行COARSE功能
		//@remarks :  {IO:W}{DATA:null}
		CMD_COARSE,	
		//@brief   : 执行按键PRTSC功能
		//@remarks :  {IO:W}{DATA:null}
		CMD_PRTSC,
		//@brief   :  
		CMD_GET_BITMAP_FILENAME,
		//@brief   : 读取截图数据
		//@remarks : {IO:R}{DATA: 2000M/4000M是768512Bytes}
		CMD_GET_BITMAP_FILEDATA,

		//@brief   : 按键指令
		//@remarks : {IO:WR}{DATA<8Bytes>:comAPICommon::Key from <comApiCommon.h>}
		CMD_KEY,
		//@brief   : 截取压缩后的屏幕像素数据
		CMD_CAPTURE_SCREEN, 
		//@brief   : 读写配置数据
		CMD_DEV_CONFIG,
		//@brief   : 查询设备显示名。
		CMD_IDN,
		//@brief   : 查询协议版本号。
		CMD_CVER,

		//@brief   : 读取屏幕信息 
		//@remarks : {IO:R}{DATA<20Bytes>:comApiEx::SCREEN_INFO from <Model_UTD2102CM.h>}
		CMD_GET_SCREEN_INFO = 50,		//SCREEN_INFO	
		//@brief   : short 1: usb connection state is ok; other: usb connection state is not ok		(ok)
		//@remarks : 
		CMD_GET_USB_CONNECTION_STATE,
		//@brief   : 得到当前频率计值
		//@remarks :  {IO:R}{DATA:comApiEx::SignalFreq from <Model_UTD2102CM.h>}
		CMD_GET_CYMOMETER_VALUE,
		//@brief   : 得到当前选中的通道
		//@remarks : short 0:	CH1; 1: CH2; 2: MATH; 3: RefA; 4: RefB; -1:other				(ok)
		CMD_GET_CURRENT_ACTIVE_CHANNEL, //short 0:	CH1; 1: CH2; 2: MATH; 3: RefA; 4: RefB; -1:other				(ok)
		//@brief   : 得到设备端当前有操作焦点的菜单
		//@remarks : short 0: CH1 menu; 1: CH2 menu; 2: MATH or FFT menu; -1: other menu	(ok)
		CMD_GET_CURRENT_ACTIVE_MENU,	
		//@brief   : 获取当前菜单是否显示
		//@remarks : short 0: current menu is not display; 1: current menu is display			(ok)
		CMD_GET_CURRENT_MENU_SHOW_STATE,
		CMD_GET_SAMPLE,
		//@brief   : 检查U盘是否插入
		//@remarks : {IO:R}{DATA<Enum>: 0 未插入； 1 插入}
		CMD_GET_USBDISK_STATE,

		//{acquire
		//@brief   : 采集模式 
		//@remarks : {IO:WR}{DATA<Enum>:comApiEx::E_ACQ_MODE from <ItemCode_COMVer2.h>}
		CMD_ACQ_MODE = 400,	
		//@brief   : 平均次数
		//@remarks : {IO:WR}{DATA<short>: averageNum}
		//实际次数必须用公式<2^(averageNum + 1)>计算, 比如: {(0: 2), (1: 4), (2: 8), (3: 16), (4: 32), (5: 64), (6: 128), (7: 256)}
		CMD_ACQ_AVERAGE_NUM,
		//@brief   : 采样方式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_SAMPLE_MODE from <ItemCode_COMVer2.h>}
		CMD_ACQ_SAMPLING,				
		//@brief   : 采样深度
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_ACQ_SAV_DEPTH from <ItemCode_COMVer2.h>}
		CMD_ACQ_DEPTH,				
		//@brief   : 快速采集
		//@remarks : {IO:WR}{DATA<Enum>: 0 关闭 ； 1 打开}
		CMD_ACQ_FAST,
		//}

		//{trigger		
		//@brief   : 触发类型
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_TYPE from <ItemCode_COMVer2.h>}
		CMD_TRIG_TYPE = 500,
		//@brief   : 触发信源
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_SOURCE from <ItemCode_COMVer2.h>}
		CMD_TRIG_SOURCE,
		//@brief   : 触发耦合
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TIRG_COUPLING from <ItemCode_COMVer2.h>}
		CMD_TRIG_COUPLING,	
		//@brief   : 触发方式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_MODE from <ItemCode_COMVer2.h>}
		CMD_TRIG_METHOD,
		//@brief   : 边沿触发和斜率触发-斜率类型
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_EDGE_SLOPE(边沿、超幅)  E_SLOP_POLARITY(斜率) from <ItemCode_COMVer2.h>}
		CMD_TRIG_SLOPE_TYPE,	
		//@brief   : 脉宽极性
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_PULSE_POLAR from <ItemCode_COMVer2.h>}
		CMD_TRIG_PULSE_POLAR,
		//@brief   : 脉宽条件
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_PULSE_CONDITION from <ItemCode_COMVer2.h>}
		CMD_TRIG_PULSE_COND,
		//@brief   : 脉宽时间
		//@remarks : {IO:WR}{DATA<UINT>: 步进值step, 计算公式(t = K * step), 一般K = 4}
		CMD_TRIG_PULSE_TIME,		
		//@brief   : 视频触发-视频制式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_VIDEO_STANDARD from <ItemCode_COMVer2.h>}
		CMD_TRIG_VIDEO_STD,		
		//@brief   : 视频触发-同步
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_TRIG_VIDEO_SYNC_MODE from <ItemCode_COMVer2.h>}
		CMD_TRIG_VIDEO_SYNC,	
		//@brief   : 视频触发-指定行数
		//@remarks : {IO:WR}{DATA<short>:行数 }
		CMD_TRIG_VIDEO_LINE_NUMBER,
		//@brief   : 斜率触发-斜率条件
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_SLOP_WHEN from <ItemCode_COMVer2.h>}
		CMD_TRIG_SLOPE_COND,
		//@brief   : 斜率触发-时间设置
		//@remarks : {IO:WR}{DATA<UINT>: 步进值， 计算公式为t = K * step, 一般K = 4;}
		CMD_TRIG_SLOPE_TIME,	
		//@brief   : 斜率触发-阈值(欠幅/超幅下的阈值)
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_SLOP_THRESHOLD from <ItemCode_COMVer2.h>}
		CMD_TRIG_SLOPE_THRESHOLD,
		//@brief   : 斜率触发-压摆率
		//@remarks : {IO:R}{DATA<char[16]>: 以字符串形式读取}
		CMD_TRIG_SLOPE_VT,

		//@brief   : 脉宽时间1 CEM兼容"脉宽条件"><"第二个时间
		//@remarks : {IO:WR}{DATA<UINT>: 步进值step, 计算公式(t = K * step), 一般K = 4}
		CMD_TRIG_PULSE_LT_TIME,
		//@brief   : 斜率触发-时间设置2 CEM兼容"斜率条件"><"第二个时间
		//@remarks : {IO:WR}{DATA<UINT>: 步进值， 计算公式为t = K * step, 一般K = 4;}
		CMD_TRIG_SLOPE_LT_TIME,

		//@brief   : 超幅-条件
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_WINDOE_WHEN from <ItemCode_COMVer2.h>}
		CMD_TRIG_WINDOW_COND,
		//@brief   : 超幅-时间
		//@remarks :{IO:WR}{DATA<UINT>: 步进值， 计算公式为t = K * step, 一般K = 4;}
		CMD_TRIG_WINDOW_TIME,
		//@brief   : 欠幅-条件
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_RUNT_WHEN from <ItemCode_COMVer2.h>}
		CMD_TRIG_RUNT_COND,
		//@brief   : 欠幅-时间
		//@remarks :{IO:WR}{DATA<UINT>: 步进值， 计算公式为t = K * step, 一般K = 4;}
		CMD_TRIG_RUNT_TIME,
		//@brief   : 欠幅-时间1 CEM兼容"条件"><"第二个时间
		//@remarks : {IO:WR}{DATA<UINT>: 步进值， 计算公式为t = K * step, 一般K = 4;}
		CMD_TRIG_RUNT_LT_TIME,
		//}


		//{display
		//@brief   : DISPLAY 类型
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_DISPLAY_TYPE from <ItemCode_COMVer2.h>}
		CMD_DISPLAY_TYPE = 600,
		//@brief   : DISPLAY 格式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_DISPLAY_FORMAT from <ItemCode_COMVer2.h>}
		CMD_DISPLAY_FORMAT,
		//@brief   : DISPLAY 栅格
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_Display_Graticule from <ItemCode_COMVer2.h>}
		CMD_DISPLAY_GRID,
		//@brief   : DISPLAY 余辉时间
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_Dislay_PersistTime from <ItemCode_COMVer2.h>}
		CMD_DISPLAY_PERSIST,
		//@brief   : DISPLAY 波形亮度
		//@remarks : {IO:WR}{DATA<short>: 一般是 ： 1 - 100；  2000M、4000M是 step∈[0,49]，亮度 = curStep * 2 + 2}
		CMD_DISPLAY_WAVE_LUM,
		//}

		//{cursor
		//@brief   : Cursor 类型
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_CURSOR_TYPE from <ItemCode_COMVer2.h>}
		CMD_CURSOR_TYPE,
		//@brief   : Cursor 模式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_CURSOR_MODE from <ItemCode_COMVer2.h>}
		CMD_CURSOR_MODE,
		//@brief   : Cursor 模式
		//@remarks : {IO:WR}{DATA<Enum>: 时间测量 ： comApiEx::E_CURSOR_V_UNIT  电压测量 : comApiEx::E_CURSOR_H_UNIT from <ItemCode_COMVer2.h>}
		CMD_CURSOR_UNIT,
		//@brief   : Cursor 基准
		//@remarks : {IO:WR}{DATA<Enum>: E_CURSOR_POS from <ItemCode_COMVer2.h>}
		CMD_CURSOR_BASE,
		//@brief   : Cursor 光标线A位置调节
		//@remarks : {IO:WR}{DATA<short> : 水平线：居中为0，一格为25；垂直线：居中为350，一格为50}
		CMD_CURSOR_A_POS,
		//@brief   : Cursor 光标线B位置调节
		//@remarks : {IO:WR}{DATA<short> : 水平线：居中为0，一格为25；垂直线：居中为350，一格为50 }
		CMD_CURSOR_B_POS,
		//}

		//{windows
		CMD_WIN_TYPE,					//												(ok)					
		CMD_WIN_HOLDOFF,				//			
		//}

		//{storage
		//@brief   : Storage 类型
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_STORAGE_TYPE from <ItemCode_COMVer2.h>}
		CMD_STORAGE_TYPE = 700,
		//@brief   : Storage 文件名
		//@remarks : {IO:WR}{DATA<char[8>: }
		CMD_STORAGE_FILE_NAME,
		//@brief   : Storage 参考波形-信源
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_CHANNEL_ID from <ItemCode_COMVer2.h>}
		CMD_STORAGE_REFWAVE_SOURCE,
		//@brief   : Storage 参考波形-文件格式
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_STORAGE_REFWAVE_FILE_FORMAT from <ItemCode_COMVer2.h>}
		CMD_STORAGE_REFWAVE_FILE_FORMAT,
		//@brief   : 读取内置存储的文件列表
		//@remarks :{IO:WR}{DATA<Struct>: comApiEx::FileList_Flash from <ItemCode_UTD2102CM.h>} 
		CMD_STORAGE_GET_FILE_LIST_FLASH,	//											(ok)
		//@brief   : 读取外置存储的文件列表
		//@remarks :{IO:WR}{DATA<Struct>: comApiEx::FileList_USB from <ItemCode_UTD2102CM.h>} 
		CMD_STORAGE_GET_FILE_LIST_USB,
		//@brief   : 执行SAVE命令，保存到内置Flash.
		//@remarks :  {IO:W}{DATA<short> : 文件存储位置}
		CMD_STORAGE_SAVE,
		//@brief   : 执行LOAD命令，从内置Flash加载.
		//@remarks :  {IO:W}{DATA<short> : 文件存储位置}
		CMD_STORAGE_LOAD,
		//@brief   : 执行Import命令，从外置U盘加载.
		//@remarks :  {IO:W}{DATA<short> : 文件存储位置}
		CMD_STORAGE_IMPORT,
		//@brief   : 执行Export命令
		//@remarks :  {IO:W}{DATA<null>}
		CMD_STORAGE_EXPORT,	
		//@brief   : 查询是否存在已保存的数据
		//@remarks :  {IO:R}{DATA<short> : 0 不存在， 1 存在}
		CMD_STORAGE_REFSETTING_EXIST,
		//@brief   : 存储位置
		//@remarks :  {IO:WR}{DATA<short> : 位置索引}
		CMD_STORAGE_POS,
		//}

		//{measure
		MEASURE_ALL = 800,
		MEASURE_CLEAR,
		MEASURE_CUSTOMIZE,
		MEASURE_INDICATOR,
		MEASURE_DELAY_FROM,
		MEASURE_DELAY_FEDGE,
		MEASURE_DELAY_TO,
		MEASURE_DELAY_TEGDE,
		MEASURE_DELAY_OK,
		MEASURE_PHASE_FROM,

		MEASURE_PHASE_TO,
		MEASURE_PHASE_OK,
		//}

		//for UTD2102CEX 
		CMD_MEASURE_SOURCE,
		CMD_MEASURE_STATISTICS,

		//{utility
		//@brief   : 执行自校正命令
		//@remarks :  {IO:W}{DATA<null>}
		CMD_UTILITY_SELF_CALIBRATE = 1000,
		CMD_UTILITY_SYSTEM_INFO,							//(ok)
		//@brief   : 执行Clear命令
		//@remarks :  {IO:W}{DATA<null>}
		CMD_UTILITY_CLEAR_INFO,	
		//@brief   : 执行恢复出厂设置命令
		//@remarks :  {IO:W}{DATA<null>}
		CMD_UTILITY_RESET_TO_FACTORY,
		//@brief   : 对比度
		//@remarks :  {IO:WR}{DATA<short>}
		CMD_UTILITY_CONTRAST,
		//@brief   : 语言切换
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_Utility_Language from <ItemCode_COMVer2.h>}
		CMD_UTILITY_LANGUAGE,
		//@brief   : 界面风格
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_Utility_Skin from <ItemCode_COMVer2.h>}
		CMD_UTILITY_SKIN,
		//@brief   : 菜单显示时间
		//@remarks : {IO:WR}{DATA<Enum>: comApiEx::E_Utility_Menu_Display_Time from <ItemCode_COMVer2.h>}
		CMD_UTILITY_MENU_DISP,	
		//@brief   : 波形亮度
		//@remarks :  {IO:WR}{DATA<short>}
		CMD_UTILITY_GRID_LUM,
		//@brief   : 频率计开关
		//@remarks : {IO:WR}{DATA<Enum>:comApiEx:: E_SWITCH_STATE from <ItemCode_COMVer2.h>}
		CMD_UTILITY_CYMOMETER,

		CMD_UTILITY_AUTOSET_CHANNEL,
		CMD_UTILITY_AUTOSET_ACQUIRE,
		CMD_UTILITY_AUTOSET_TRIG,
		CMD_UTILITY_AUTOSET_SIGNAL,
		//}

		//{record
		CMD_DSO_RECORD_START,
		CMD_DSO_RECORD_STOP,
		CMD_DSO_RECORD_REPLAY,
		CMD_DSO_RECORD_CLEAR,
		//}
	
		//{
		CMD_UTILITY_SQUARE_WAVE,
		CMD_UTILITY_SLEEP_TIME,
		CMD_UTILITY_MEASURE_UNIT,
		//}

		//{
		CMD_SYS_TIME_SWITCH = 1100,
		CMD_SYS_TIME,
		//}

		//{pass fail
		CMD_PASS_FAIL_ALLOW_TEST,					
		CMD_PASS_FAIL_OUTPUT,						
		CMD_PASS_FAIL_SOURCE,						
		CMD_PASS_FAIL_DISP_INFO,					
		CMD_PASS_FAIL_OPERATION,					
		CMD_PASS_FAIL_STOP_TYPE,					
		CMD_PASS_FAIL_STOP_COND,					
		CMD_PASS_FAIL_STOP_THRESHOLD,				
		CMD_PASS_FAIL_TEMPLATE_WAVEREF,				
		CMD_PASS_FAIL_TEMPLATE_REFLOAD,				
		CMD_PASS_FAIL_TEMPLATE_HRANGE,				
		CMD_PASS_FAIL_TEMPLATE_VRANGE,				
		CMD_PASS_FAIL_TEMPLATE_CREATE,				
		//}

		//@brief   : 读取通信数据包，格式见文档；
		//@remarks : 
		CMD_GET_DISPLAY_DATA = 1200,	//get ch1, ch2, math, ref a/b data ok
		//同步DSO的所有状态，数据类型：DSOAttrState，类型可能因机型而不同；
		CMD_SYNC_DSO,
		
		//定时发送锁键盘命令给DSO，无命令数据。
		CMD_PERIOD_MSG2DSO,

		//{CEX-EDU使用
		//CEX-EDU使用，发送将要读取REC波形数据的头的准备命令。
		CMD_DSO_RECORD_HEADER_READY = 1400,
		//CEX-EDU使用，读取REC波形数据头。
		CMD_DSO_RECORD_HEADER,		
		//CEX-EDU使用，发送将要读取REC波形数块准备命令。
		CMD_DSO_RECORD_DATA_READY,
		//CEX-EDU使用，读取REC波形数据块。
		CMD_DSO_RECORD_DATA,
		//CEX-EDU使用，判断当前REC数据是否有效。
		CMD_DSO_RECORD_DATA_VALID,
		//}

		//{2000M-special
		CMD_VARDEPTH_SAMPLE_RATE = 1420,
		CMD_VARDEPTH_SAVE_DEPTH,
		CMD_VARDEPTH_PRE_TRIG_TIME,
		CMD_VARDEPTH_AUTO_TRIG_TIME,
		//}
	
		//
		CMD_MESSAGE = 1430,	
		// 
		CMD_DUMP_BEGIN = 1440,
		CMD_DUMP_ID = CMD_DUMP_BEGIN,
		CMD_DUMP_END = 1500, //#TODO:如果非要提供命令编码，就将CEX和2000M的编码检查表，内置处理，对外提供统一的命令编码。
	}COMMON_CMD;
	
	//@brief   : 通道命令
	//@remarks : 
	typedef enum _channel_cmd
	{
		CH_Invalid_CMD = -1,	
		//@brief   : 通道耦合
		//@remarks : {IO:WR}{DATA<Enum>:E_CH_COUPLING_TYPE}
		CH_ATTR_COUPLING = 0,		
		//@brief :  带宽限制
		//@remarks : {IO:WR}{DATA<Enum>:E_CH_BANDLIMIT}
		CH_ATTR_BAND_WIDTH,
		//@brief   : 
		//@remarks : 
		CH_ATTR_VOLT_SCALE,
		//@brief   : 放大倍数
		//@remarks : {IO:WR}{DATA<Enum>:CH_PROBE}
		CH_ATTR_PROBE,
		//@brief :  反相
		//@remarks : {IO:WR}{DATA<Enum>:E_SWITCH_STATE}
		CH_ATTR_INVERT,	
		//@brief   : 偏置电压开关
		//@remarks : {IO:WR}{DATA<Enum>:E_SWITCH_STATE}
		CH_ATTR_PRE_BIAS_VOLT_SWITCH,      
		//@brief   : 偏置电压值
		//@remarks : {IO:W}{DATA<Short>: +1 increase; -1 decrease;}
		CH_ATTR_PRE_BIAS_VOLT_VALUE,    	
		//@brief   : 偏置电压归零
		//@remarks : {IO:W}{DATA:null}
		CH_ATTR_PRE_BIAS_VOLT_TOZERO,

		//@brief   : 通道开关
		//@remarks :  {IO:WR}{DATA<Enum>:E_SWITCH_STATE}
		CH_ATTR_CHANNEL_SWITCH = 100,   
		//@brief   : 垂直位置
		//@remarks : {IO:WR}{DATA<Short>: UTD2000M -> 上到下为 [-100 , 100], CEX -> 从下到上[28,128]}
		CH_ATTR_VERTICAL_POS,	
		//@brief   : 垂直位置归零
		//@remarks : {IO:W}{DATA:null}
		CH_ATTR_VERTICAL_POS_TOZERO,	
		//@brief   : 预触发深度位置
		//@remarks : {IO:WR}{DATA<Short>: UTD2000M -> 屏幕中间为350，一格为50}
		CH_ATTR_TRIG_DEPTH,	
		//@brief   : 预触发深度归零
		//@remarks : {IO:W}{DATA:null}
		CH_ATTR_TRIG_DEPTH_TOZERO,
		//@brief : 触发电平信息
		//@remarks : {IO:R}{DATA:comApiEx::CellBaseValue}
		//      普通触发只有一个触发电平 数据为 comApiEx::CellBaseValues TrigVolt;
		//      斜率触发有两个触发电平， 数据为 comApiEx::CellBaseValues TrigVolt[2];
		CH_ATTR_TRIG_LEVEL,   
		//@brief : 触发电平位置
		//@remarks : {IO:WR}{DATA<Short>: 以通道基线为0点，上正下负，每格25}
		CH_ATTR_TRIG_LEVEL_POS,	
		//@brief   : 触发电平位置归零
		//@remarks : {IO:WR}{DATA<Short>: 以通道基线为0点，上正下负，每格25}
		CH_ATTR_TRIG_LEVEL_TOZERO,
		//@brief   : 伏格挡信息
		//@remarks : {IO:R}{DATA: CellBaseValue}
		CH_ATTR_VOLT_AMPLITUDE,      
		//伏格挡档位，类型为short
		//@brief   : 伏格挡档位
		//@remarks : {IO:WR}{DATA<short>: +1 increase; -1 decrease; other : code }
		CH_ATTR_VOLT_AMPLITUDE_LEVEL,   
		//时基档档位，类型为short，只能设置！
		//@brief   : 时基档档位 
		//@remarks : {IO:W}{DATA<short>: E_TIMEBASE_LEVEL }
		CH_ATTR_TIME_BASE,      
		//@brief   : 读取时基档时间
		//@remarks : {IO:R}{DATA: CellBaseValue}
		CH_ATTR_TIME_BASE_VALUE,      
	
		//@brief   : MATH类型
		//@remarks : {IO:WR}{DATA<Enum>: E_MATH_TYPE }
		CH_MATH_TYPE = 200,		
		//@brief   : MATH信源1
		//@remarks : {IO:WR}{DATA<Enum>: E_CHANNEL_ID }
		CH_MATH_OP1,	
		//MATH算子类型，数据类型：short
		//@brief   : MATH算子类型
		//@remarks : {IO:WR}{DATA<Enum>: E_MATH_OP_SIGN }
		CH_MATH_OP,	
		//@brief   : MATH信源2
		//@remarks : {IO:WR}{DATA<Enum>: E_CHANNEL_ID }
		CH_MATH_OP2,
		//@brief   : FFT信源
		//@remarks : {IO:WR}{DATA<Enum>: E_CHANNEL_ID }
		CH_FFT_SRC,	
		//@brief   : FFT窗
		//@remarks : {IO:WR}{DATA<Enum>: E_FFT_WINDOW }
		CH_FFT_WINDOW,		
		//@brief   : FFT垂直单位
		//@remarks : {IO:WR}{DATA<Enum>: E_FFT_UNIT }
		CH_FFT_VSCALE,		
		//@brief   : 读取FFT频率值
		//@remarks : {IO:R}{DATA: SignalFreq }
		CH_FFT_FREQ,
		//@brief   : 滤波类型
		//@remarks : {IO:WR}{DATA<Enum>: E_FILTER_TYPE }
		CH_FILTER_TYPE,		
		//@brief   : 频率下限
		//@remarks : {IO:WR}{DATA<LONGLONG>: 步进值；频率值要自行计算}
		CH_FILTER_FREQ_LOW,
		//@brief   : 频率上限
		//@remarks : {IO:WR}{DATA<LONGLONG>: 步进值；频率值要自行计算}
		CH_FILTER_FREQ_HIGH,
		//@brief   : 滤波信源
		//@remarks : {IO:WR}{DATA<Enum>: E_CHANNEL_ID }
		CH_FILTER_SRC,	
	
		//@brief   : REF回调波形(本地)
		//@remarks : {IO:W}{DATA<short>: 为存储位置值 }
		CH_REF_LOAD_FROM_FLASH = 300,	
		//@brief   : REF导入波形(USB)
		//@remarks : {IO:W}{DATA<short>: 为存储位置值 }
		CH_REF_LOAD_FROM_USB,	
		//@brief   : REF存储位置调整
		//@remarks : {IO:W}{DATA<short>: 为存储位置值 }
		CH_REF_POS,
		//@brief   : 判断某一位置波形是否存在，位置是CH_REF_POS命令设置的！
		//@remarks : {IO:R}{DATA<short>: 为存储位置值 }
		CH_REF_POS_WAVE_EXIST,	
		//@brief   : 清除所有存储波形
		//@remarks : {IO:W}{DATA : null }
		CH_REF_CLEAR,

		//[弃用]
		CH_DISPLAY_DATA = 400,
		//[弃用]读取原始波形数据，改用CH_GET_SAV_DATA
		CH_ORIGIN_DATA,	

		//@brief   : 提取SAV文件数据，数据格式 ： [4Bytes 文件长度] + [文件数据]。
		//@remarks : 使用方式详见文档和示例！
		// UTD2102CM、UTD4000M 对应缓冲区大小为32768Bytes， UTD2102CEX-EDU对应的是32000Bytes
		CH_GET_SAV_DATA,

		//{2102CM-x
		CH_VARDEPTH_ENABLE = 500,
		CH_VARDEPTH_SAMPLE_RATE,

		CH_VARDEPTH_DATA_READY = 510,
		CH_VARDEPTH_DATA,

		CH_VOLT_AMPLITUDE_LEVEL,			//电压幅值设置，直接接设置0~15档位
		CH_PRE_BIAS_VOLT_VALUE,			//偏置电压设置，直接设置电压单精度浮点值

		CH_VARDEPTH_TRIG_READY,         //通过该命令启动DSO捕捉信号的机制
		//}
	}CHAN_CMD;

#ifdef __cplusplus
}
#endif

#endif // CMD_COMVer2_h__
