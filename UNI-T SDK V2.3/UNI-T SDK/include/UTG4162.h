/********************************************************************
	created:	2015/01/14
	author:		M.Yang
	purpose:	与UCI库联合使用的有关UTG4000&UTG8000的定义
	*********************************************************************/
#ifndef UTG4162_h__
#define UTG4162_h__

#ifdef __cplusplus
namespace uci {
	namespace utg4162 {	
#endif // __cplusplus		
		
		/////////////////////////////////Key Define/////////////////////////////

		//@brief : UTG4000&UTG8000系列机型的面板按键键值定义。
		//@remark:
		typedef enum _SGKeys {
			Key_F1 = 0,
			Key_F2,
			Key_F3,
			Key_F4,
			Key_F5,
			Key_F6,
			Key_0 = '0',
			Key_1,
			Key_2,
			Key_3,
			Key_4,
			Key_5,
			Key_6,
			Key_7,
			Key_8,
			Key_9,
			Key_Dot,
			Key_Symbol,
			Key_Right,
			Key_Left,
			Key_Ok,
			Key_Up,
			Key_Down,
			//
			Key_User,
			Key_Digital,
			Key_Counter,
			Key_Mod,
			Key_Sweep,
			Key_Bst,
			Key_Sine,
			Key_Noise,
			Key_Square,
			Key_Ramp,
			Key_Pulse,
			Key_Arb,
			Key_Harmonic,
			Key_DC,
			Key_Ch1,
			Key_Ch2,
			Key_Trigger,
			Key_SwapCh,
			Key_OtherPage,
			Key_Preset,
			Key_Storge,
			Key_StorgeL,
			Key_Utility,
			Key_Help,
			Key_HelpL,
			Key_Esc,
			Key_PrScreen,
			Key_Null = 0xff
		}SGKeys;

		inline int GetKeyIndex(SGKeys _code) {
			return (_code >= Key_F1 && _code < Key_F6) ? _code : _code - '0' + 6;
		};


		//@brief       : 判断按键是否锁定
		//@param _code : 键值
		//@param _v    : 标记按键锁定状态的64bit整数。使用指令："lock?"获取。
		//@return      : == 0 未锁定。否则锁定。
		//@remarks     : 
		inline int IsKeyLocked(SGKeys _code, long long _v) {
			return _v & ((long long)1 << GetKeyIndex(_code));
		};

		//@brief   : 各LED的状态
		//@remarks : 0 表示不亮， 1 表示亮。
		typedef struct _LEDStatus {
			char Noise;
			char UTILTY;
			char STORGE;
			char RAMP;
			char PULSE;
			char COUNTER;
			char USER;
			char DIGTAL;
			char HARMONIC;
			char DC;
			char TRGGER;
			char CH2;
			char SWAPCH;
			char ARB;
			char CH1;
			char SINE;
			char SQUARE;
		}LEDStatus;


		/////////////////////////////////File Load/////////////////////////////
		//@brief   : 任意波的加载方式
		typedef enum _ARBWriteMode {
			//@brief   : 以载波方式加载	
			ARB_MODE_CARRIER = 0,
			//@brief   : 以调制波方式加载	
			ARB_MODE_MOD,
		}ARBWriteMode;

		//@brief   : 下载的文件存放介质
		typedef enum _FileMedium {
			//@brief   : 存放到RAM
			FM_RAM = 0,
			//@brief   : 存放到ROM
			FM_ROM,
			//@brief   : 存放到TF卡
			FM_TF,
			//@brief   : 存放的U盘
			FM_UDISK
		}FileMedium;


		/////////////////////////////////Parameters Address Define/////////////////////////////
		//工作模式
		typedef enum _EWorkMode : long long {
			//@brief : 基波模式
			WM_BASE = 0,
			//@brief : 调制波模式
			WM_MODE,
			//@brief : 扫频模式
			WM_SWEEP,
			//@brief : 猝发
			WM_BURST,
		}EWorkMode;

		//基本波形
		typedef enum _EBaseWave : long long {
			//@brief : 正弦波
			BASE_SINE = 0,
			//@brief : 方波
			BASE_SQUARE = 1,
			//@brief : 斜波
			BASE_RAMP = 3,
			//@brief : 脉冲波
			BASE_PULSE = 4,
			//@brief : 噪声
			BASE_NOISE = 5,
			//@brief : 任意波
			BASE_ARB = 6,
			//@brief : 谐波
			BASE_HARMONIC = 7,
			//@brief : 直流
			BASE_DC = 8
		}EBaseWave;

		typedef enum _EModeType : long long {
			MT_AM = 0,
			MT_FM = 1,
			MT_PM = 2,
			MT_ASK = 3,
			MT_FSK = 4,
			MT_PSK = 5,
			MT_BPSK = 6,
			MT_QPSK = 7,
			MT_OSK = 8,
			MT_QAM = 9,
			MT_PWM = 10,
			MT_SUM = 11,
		}EModeType;

		//调制波形
		typedef enum _EModeWaveType : long long {
			MOD_WAVE_SINE = 0,
			MOD_WAVE_SQUARE = 1,
			MOD_WAVE_UPRAMP = 2,
			MOD_WAVE_DNRAMP = 3,
			MOD_WAVE_NOISE = 4,
			MOD_WAVE_ARB = 5,
		}EModeWaveType;

		//@brief : 信号源参数编码 
		//@remark: 读参数使用rp指令，写参数使用wp指令。 
		//@example: wp@ch:0@addr:0x8000@v:0;
		//          rp@ch:0@addr:0x8000;
		// 所有参数的对应的数据量是8bytes，double类型
		typedef enum _enumRemoteMessage {
			//@brief : 工作模式 
			//@remark: {IO:WR}{DATA:EWorkMode}
			RM_WORK_MODE = 0x8000,

			//{ 基波
			//@brief : 通道开关 
			//@remark: {IO:WR}{DATA: 0-OFF，1-ON}
			RM_CH_SW = 0x8001,
			//@brief : 同步开关 
			//@remark: {IO:WR}{DATA: 0-OFF，1-ON，2-reverse}
			RM_CH_SYNC_SW,
			//@brief : 通道反向 
			//@remark: {IO:WR}{DATA:  0：OFF，1：ON}
			RM_CH_REVERTSE,
			//@brief : 通道负载阻抗 
			//@remark: {IO:WR}{DATA: 1~1000}
			RM_CH_LOAD,
			//@brief : 通道输出限制使能 
			//@remark: {IO:WR}{DATA:   0：OFF，1：ON}
			RM_CH_OUTPUT_LIMIT_ENABLE,
			//@brief : 通道输出限制最低电平  
			//@remark: {IO:WR}{DATA<?>:   -10V~MAX_LEVEL}
			RM_CH_OUTPUT_LIMIT_MIN_LEVEL,
			//@brief : 通道输出限制最高电平 
			//@remark: {IO:WR}{DATA:   MIN_LEVEL~10V}
			RM_CH_OUTPUT_LIMIT_MAX_LEVEL,

			//@brief : 基波类型 
			//@remark: {IO:WR}{DATA:EBaseWave}
			RM_BASE_WAVE_TYPE = 0x8008,
			//@brief : 基波频率(单位：Hz)
			//@remark: {IO:WR}{DATA:	1uHz~但前基波最大频率}
			RM_BASE_FREQ,
			//@brief : 基波相位(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}		
			RM_BASE_PHASE = 0x800A,
			//@brief : 基波幅度(单位：VPP)
			//@remark: {IO:WR}{DATA:	1mVpp~10Vpp（50欧姆）}				
			RM_BASE_AMP_VPP,
			//@brief : 基波幅度(单位：VRMS)
			//@remark: {IO:WR}{DATA:	1mVRMS~5Vpp（50欧姆）}	
			RM_BASE_AMP_VRMS,
			//@brief : 基波幅度(单位：VDBM)
			//@remark: {IO:WR}{DATA:	-53.010VDBM~26.99VDBM（50欧姆）}		
			RM_BASE_AMP_VDBM,
			//@brief : 基波偏移(单位：V)
			//@remark: {IO:WR}{DATA:	-5VRMS~5Vpp（50欧姆）}
			RM_BASE_OFFSET,
			//@brief : 基波高电平(单位：V)
			//@remark: {IO:WR}{DATA:	BASE_LOW~5Vpp（50欧姆）}
			RM_BASE_HIGHT = 0x800F,
			//@brief : 基波低电平(单位：V)
			//@remark: {IO:WR}{DATA:	-5VRMS~BASE_HIGHT（50欧姆）}
			RM_BASE_LOW = 0x8010,
			//@brief : 基波占空比对方波占空比、脉冲波占空比、三角波对称度共用
			//@remark: {IO:WR}{DATA:	0~100}
			RM_BASE_DUTY,
			//@brief : 脉冲波上升时间(单位：s)
			//@remark: {IO:WR}{DATA:	min ~ 周期*0.4}
			RM_BASE_RISETIME,
			//@brief : 脉冲波下降时间(单位：s)
			//@remark: {IO:WR}{DATA:	min ~ 周期*0.4}
			RM_BASE_FALLTIME,
			//@brief : 任意波播放模式开关
			//@remark: {IO:WR}{DATA:0：OFF，1：ON}
			RM_BASE_ARB_PLAY_ENABLE,
			//@brief : 谐波 - 工作模式
			//@remark: {IO:WR}
			//{DATA:
			//0:    奇次,
			//1:    偶次, 
			//2:    全部, 
			//3:    USER,自定义
			// }
			RM_BASE_HARMOIC_TYPE = 0x8080,
			//@brief :谐波开关,在RM_BASE_HARMOIC_TYPE=USER时有效
			//@remark: {IO:WR}{DATA:BIT14至BIT0分别对应2~16次谐波开关，BIT15对应基波强制开启}
			RM_BASE_HARMONIC_ONOFF,
			//设置N(2~16)次谐波,可以可以有两种方法:
			//EG:N=3(三次谐波),AMP = 1Vpp, PHASE = 90°
			//方法1:
			//（1）、指定谐波次数，RM_HARMONIC_NUM = N(2~16),
			//（2）、设定谐波的幅度和相位,
			//		RM_HARMONIC_SN_AMP_N = 1.0; RM_HARMONIC_SN_PHASE_N = 90.0;
			//方法1:直接设置第三次谐波的幅度和相位
			//RM_HARMONIC_SN_AMP_3 = 1.0, RM_HARMONIC_SN_PHASE_3 = 90.0,	

			//@brief : 谐波次数
			//@remark: {IO:WR}{DATA:1~15}
			RM_HARMONIC_NUM,
			//@brief : 谐波幅度Vpp
			//@remark: {IO:WR}{DATA:0~基波幅度}
			RM_HARMONIC_SN_AMP_N,
			//@brief : 谐波相位(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}
			RM_HARMONIC_SN_PHASE_N = 0x8084,
			//@brief : 谐波幅度 同RM_HARMONIC_SN_AMP_N
			RM_HARMONIC_SN_AMP_2,
			RM_HARMONIC_SN_AMP_3,
			RM_HARMONIC_SN_AMP_4,
			RM_HARMONIC_SN_AMP_5,
			RM_HARMONIC_SN_AMP_6,
			RM_HARMONIC_SN_AMP_7 = 0x808A ,
			RM_HARMONIC_SN_AMP_8,
			RM_HARMONIC_SN_AMP_9,
			RM_HARMONIC_SN_AMP_10,
			RM_HARMONIC_SN_AMP_11,
			RM_HARMONIC_SN_AMP_12 = 0x808F,
			RM_HARMONIC_SN_AMP_13 = 0x8090,
			RM_HARMONIC_SN_AMP_14,
			RM_HARMONIC_SN_AMP_15,
			RM_HARMONIC_SN_AMP_16,
			//@brief : 谐波相位 RM_HARMONIC_SN_PHASE_N
			RM_HARMONIC_SN_PHASE_2,
			RM_HARMONIC_SN_PHASE_3,
			RM_HARMONIC_SN_PHASE_4,
			RM_HARMONIC_SN_PHASE_5,
			RM_HARMONIC_SN_PHASE_6,
			RM_HARMONIC_SN_PHASE_7,
			RM_HARMONIC_SN_PHASE_8 = 0x809A,
			RM_HARMONIC_SN_PHASE_9,
			RM_HARMONIC_SN_PHASE_10,
			RM_HARMONIC_SN_PHASE_11,
			RM_HARMONIC_SN_PHASE_12,
			RM_HARMONIC_SN_PHASE_13 = 0x809F,
			RM_HARMONIC_SN_PHASE_14 = 0x80A0,
			RM_HARMONIC_SN_PHASE_15,
			RM_HARMONIC_SN_PHASE_16,
			//}

			//{MOD
			//@brief : 调制模式
			//@remark: {IO:WR}{DATA:EModeType}
			RM_MOD_TYPE = 0x8100,
			//调制波形
			//0:  MOD_SINE,
			//1:  MOD_SQUARE,
			//2:  MOD_UPRAMP,
			//3:  MOD_DNRAMP,
			//4:  MOD_NOISE,
			//5:  MOD_ARB,
			//@brief : 调制波形
			//@remark: {IO:WR}{DATA:EModeWaveType} 
			RM_MOD_WAVE,
			//@brief : 调制波频率（单位：Hz）
			//@remark: {IO:WR}{DATA: 1uHz~200KHz}
			RM_MOD_FREQ,
			//@brief : 调制波速率（单位：s）
			//@remark: {IO:WR}{DATA<double>: 2ms~1Ms}
			RM_MOD_RATE,
			//@brief : 调制深度（单位：%）
			//@remark: {IO:WR}{DATA: 0~120}
			RM_MOD_SCOPE,
			//@brief : 调制源
			//@remark: {IO:WR}{DATA: 0-Internal,1-External}
			RM_MOD_SOURCE,
			//@brief : FM频差（单位：Hz）
			//@remark: {IO:WR}{DATA: 0~载波但前频率}
			RM_MOD_FRE_DEV,
			//@brief : PM相差	(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}
			RM_MOD_PHASE_DEV,
			//@brief : FSK跳频（单位：Hz）
			//@remark: {IO:WR}{DATA: 0~载波最大频率}
			RM_MOD_HOP_FREQ,
			//@brief : BPSK调制数据源
			//@remark: {IO:WR}
			//{DATA<double>: 
			//0:  PN7,
			//1:  PN9, 
			//2:  PN15, 
			//3:  PN21,
			//}
			RM_MOD_DATA_SOURCE = 0x8109,
			//@brief : BPSK相位、QPSK相位1,(单位：°)
			//@remark: {IO:WR}{DATA:	-360~360}}
			RM_MOD_PSK_PHASE1,
			//@brief : QPSK相位2(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}}
			RM_MOD_PSK_PHASE2,
			//@brief : QPSK相位3(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}
			RM_MOD_PSK_PHASE3,
			//@brief : OSK震荡时间(单位：s)
			//@remark: {IO:WR}{DATA: 8ns~200s}
			RM_MOD_OSC_TIME,
			//@brief : QAM调制IQ MAM
			//@remark: {IO:WR}{DATA: 0-4QAM,1-8QAM,2-16QAM,3-32QAM,4-64QAM,5-128QAM,6-256QAM,}
			RM_MOD_IQ_MAP,
			//@brief : PWM调制占空比差值
			//@remark: {IO:WR}{DATA: 0~PULS DUTY}
			RM_MOD_DUTY_DEV,
			//}

			//{SWEEP			
			//@brief : 扫频类型
			//@remark: {IO:WR}{DATA: 0：线性，1：对数}
			RM_SWEEP_TYPE = 0x8200,
			//@brief : 扫频触发源
			//@remark: {IO:WR}{DATA: 0：内部，1：外部，2：手动}
			RM_SWEEP_SOURCE,
			//@brief : 扫频时间(单位：s)
			//@remark: {IO:WR}{DATA: 1ms~500s}
			RM_SWEEP_TIME,
			//@brief : 扫频起始频率（单位：Hz）
			//@remark: {IO:WR}{DATA: 1uHz~最大载波频率}
			RM_SWEEP_START_FREQ,
			//@brief : 扫频停止频率（单位：Hz）
			//@remark: {IO:WR}{DATA: 1uHz~最大载波频率}
			RM_SWEEP_STOP_FREQ,
			//@brief : 同步输出触发频率（单位：Hz）
			//@remark: {IO:WR}{DATA: RM_SWEEP_START_FREQ~RM_SWEEP_STOP_FREQ}
			RM_SWEEP_SYNC_FREQ,
			//@brief : 扫频触发输出
			//@remark: {IO:WR}{DATA: 0-OFF，1-ON}
			RM_SWEEP_TIRG_OUT,
			//}

			//{BURST		
			//@brief : 猝发类型
			//@remark: {IO:WR}{DATA: 
			//0:  N周期,
			//1:  无限,
			//2:  门控			
			//}
			RM_BURST_TYPE = 0x8300,
			//@brief : 猝发触发源
			//@remark: {IO:WR}{DATA: 0：内部，1：外部，2：手动}
			RM_BURST_SOURCE,
			//@brief : 触发输出
			//@remark: {IO:WR}{DATA: 0-OFF，1-ON}
			RM_BURST_TIRG_OUT,
			//@brief : 猝发周期（单位：s）
			//@remark: {IO:WR}{DATA: 1~500}
			RM_BURST_PERIOD,
			//@brief : 猝发相位(单位：°)
			//@remark: {IO:WR}{DATA:-360~360}
			RM_BURST_PHASE,
			//@brief : 猝发周期数（单位：个）
			//@remark: {IO:WR}{DATA: 1~50000}
			RM_BURST_CYCLES,
			//@brief : 触发边沿
			//@remark: {IO:WR}{DATA: 0-Rise，1-Fall}
			RM_BURST_TIRG_EDGE,
			//}
		}ERemoteMessage;


#ifdef __cplusplus
	}
}
#endif // __cplusplus

#endif // UTG4162_h__
