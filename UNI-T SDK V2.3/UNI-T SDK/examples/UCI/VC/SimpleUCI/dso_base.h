/*!
 * \created : 2016/12/27
 * \author  : M.Yang
 * \purpose : 提供配合UCI接口使用的，DSO通用定义。
 */
#ifndef dso_base_h__
#define dso_base_h__

#ifdef __cplusplus
namespace comAPICommon {
#endif
	//时基档位编码所有机型一致，如果出现了不同的编码可以单独定义
#ifdef __cplusplus
	typedef enum : short {
#else
	typedef enum {
#endif // __cplusplus
		TLEVEL_1NS = 0,
		TLEVEL_2NS,
		TLEVEL_5NS,
		TLEVEL_10NS,
		TLEVEL_20NS,
		TLEVEL_50NS,
		TLEVEL_100NS,
		TLEVEL_200NS,
		TLEVEL_500NS,
		TLEVEL_1US,
		TLEVEL_2US,
		TLEVEL_5US,
		TLEVEL_10US,
		TLEVEL_20US,
		TLEVEL_50US,
		TLEVEL_100US,
		TLEVEL_200US,
		TLEVEL_500US,
		TLEVEL_1MS,
		TLEVEL_2MS,
		TLEVEL_5MS,
		TLEVEL_10MS,
		TLEVEL_20MS,
		TLEVEL_50MS,
		TLEVEL_100MS,
		TLEVEL_200MS,
		TLEVEL_500MS,
		TLEVEL_1S,
		TLEVEL_2S,
		TLEVEL_5S,
		TLEVEL_10S,
		TLEVEL_20S,
		TLEVEL_50S,
		TLEVEL_100S,
		TLEVEL_200S,
	}E_TIMEBASE_LEVEL;

#ifdef __cplusplus
	typedef enum : short {
#else
	typedef enum {
#endif // __cplusplus
		VLEVEL_1MV = 0,
		VLEVEL_2MV,
		VLEVEL_5MV,

		VLEVEL_10MV,
		VLEVEL_20MV,
		VLEVEL_50MV,

		VLEVEL_100MV,
		VLEVEL_200MV,
		VLEVEL_500MV,

		VLEVEL_1V,
		VLEVEL_2V,
		VLEVEL_5V,

		VLEVEL_10V,
		VLEVEL_20V,
	} E_VOLTAGEBASE_LEVEL;

	typedef struct _measure_param {
		float value;
		int unit;
	}MeasureParam;

	typedef struct _time {
		unsigned short Year;
		unsigned short Month;
		unsigned short Day;
		unsigned short Hour;
		unsigned short Minute;
		unsigned short Second;
	}Time;

	typedef struct _UnitParam {
		char Type;      //单位类型，比如Time、Freq等。具体定义见文档。
		char Scale;     //量级，比如k、n，p，M等。具体定义见文档。
	}UnitParam;

	//@brief : 物理量数值 
	//@remark: 4Byte align -> 8Bytes
	typedef struct _MeaValue {
		float      Value;
		UnitParam  Unit;
		char       IsValid;   //是否有效。  0 表示无效； 1表示有效。
		char       IsExist;   //是否存在。  0 表示不存在； 1表示存在。
	}MeaValue;

	//@brief : 参数测量数据包的通用定义。 
	//@remark: 
	typedef enum _EMeaParam {
		MP_MAX = 0, //最大值		
		MP_MIN,     //最小值		
		MP_HIGH,    //High(Top)-高电平(顶端值)		
		MP_MIDDLE,  //Middle-中间值		
		MP_LOW,     //Low(Bottom) - 低电平(底端值)	

		MP_PKPK,    //VPP-峰峰值		
		MP_AMP,     //最小值	
		MP_MEAN,    //平均值		
		MP_CYCMEAN, //		
		MP_RMS,     //均方根

		MP_CYCRMS,   //周期均方值		
		MP_AREA,     //面积		
		MP_CYCAREA,  //周期面积		
		MP_OVERSHOOT,//过冲		
		MP_PRESHOOT, //预冲

		MP_PERIOD,   //周期		
		MP_FREQ,     //频率	
		MP_RISE_TIME,//上升时间	
		MP_FALL_TIME,//下降时间		
		MP_PWIDTH,   //正脉宽

		MP_NWIDTH,   //负脉宽
		MP_PDUTY,    //正占空比		
		MP_NDUTY,    //负占空比		
		MP_RISEDELAY,//上升延时		
		MP_FALLDELAY,//下升延时

		MP_PHASE,	 //相位		
		MP_FRR,	     //
		MP_FRF,
		MP_FFR,
		MP_FFF,

		MP_LRF,
		MP_LRR,
		MP_LFR,
		MP_LFF,

		MP_BURST_WIDTH, //突发脉冲

		//
		//reserve section  
		//   新参数向后追加
		//
		// 
		//固定位50个参数
		MP_MAX_COUNT = 50,
	}EMeaParam;

#ifdef __cplusplus
	}
	namespace uci {
		using namespace comAPICommon;
		namespace cb = comAPICommon;//comAPICommon alias
	}
#endif

#endif // dso_base_h__
