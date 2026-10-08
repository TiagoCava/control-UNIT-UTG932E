/*!
* \created : 2016/10/10
* \author  : M.Yang
* \purpose : 提供物理单位的标准化定义
* \remarks : 所有单位的定义都是以标准单位为0，小于标准单位的为负向递减，大于标准单位时为正向递增。
*			  这样做的好处是：可以保证以标准单位为基准，向两端延展。
*/

#ifndef Unit_h__
#define Unit_h__

#ifdef __cplusplus
namespace unit {
	extern "C"{
#endif
#ifdef __cplusplus
		typedef enum _EScale : char {
#else
		typedef enum _EScale  {
#endif // DEBUG
			SCALE_p = -4,
			SCALE_n,
			SCALE_u,
			SCALE_m,
			SCALE_STD = 0,
			SCALE_K,
			SCALE_M,
			SCALE_G,
			SCALE_T,
		}EScale;

#ifdef __cplusplus
		typedef enum _EType : char{
#else
		typedef enum _EType {
#endif // __cplusplus
			TYPE_INVALID = -1,
			TYPE_FREQ,
			TYPE_TIME,
			TYPE_AREA,   //面积(Vs)
			TYPE_SAMPLERATE,//采样率（Sa/s）
			TYPE_POINT,  //点数（Sa）
			TYPE_VPP,    //峰峰值
			TYPE_VOLTAGE,//电压
			TYPE_CURRENT,//电流
			TYPE_DB,     //DB
			TYPE_VV,     //
			TYPE_PERCENT,//百分比
			TYPE_DEGREE, //度数
			TYPE_WATT,   //瓦特，功率
			TYPE_UNKNOWN,// 未知单位
		}EType;

		typedef struct _CodeName {
			char Code;
			char Name[5];
		}CodeName;

		static const CodeName ScaleTabel[] = {
			{ SCALE_p, "p" },
			{ SCALE_n, "n" },
			{ SCALE_u, "u" },
			{ SCALE_m, "m" },
			{ SCALE_STD,"" },
			{ SCALE_K, "K" },
			{ SCALE_M, "M" },
			{ SCALE_G, "G" },
			{ SCALE_T, "T" }
		};

		static const CodeName TypeTabel[] = {
			{ TYPE_FREQ, "Hz" },
			{ TYPE_TIME, "s" },
			{ TYPE_AREA, "Vs" },
			{ TYPE_SAMPLERATE, "Sa/s" },
			{ TYPE_POINT, "Sa" },
			{ TYPE_VPP, "Vpp" },
			{ TYPE_VOLTAGE, "V" },
			{ TYPE_CURRENT, "A" },
			{ TYPE_DB, "dB" },
			{ TYPE_VV, "VV" },
			{ TYPE_PERCENT, "%" },
			{ TYPE_DEGREE, "°" },
			{ TYPE_WATT, "W" },
			{ TYPE_UNKNOWN, "U" },
		};

		const char* _UCIAPI uci_UnitFindTypeName(char _code);

		const char* _UCIAPI uci_UnitFindScaleName(char _code);
		//@brief : 将当前单位的数值转换到指定的单位。
		//@param _cur_value : 当前数值
		//@param _cur_unit  : 当前单位编码
		//@param _des_unit  : 要转换到的单位编码
		//@param _scale     : 进制，一般为1000，比如频率的进制是1000
		//@return : 返回转换后的数值，比如2.3KHz 转换为Hz就是2300Hz
		//@remarks : 注意单位编码必须如EScale定义的，连续递增的量。
		double _UCIAPI uci_UnitConvertTo(double _cur_value, int _cur_unit, int _des_unit, double _scale);
		
		typedef struct _UVale {
			double Value;
			long long Unit;
		}UVale;

		//@brief : 将当前单位的数值自动转换到合适的单位类型，以使数值小于进制。比如2300Hz，转换后就是2.3KHzs
		//@param _value : 当前数值
		//@param _unit  : 当前单位编码
		//@param _scale : 进制，一般为1000，比如频率的进制是1000
		//@remarks :  注意单位编码必须如EScale定义的，连续递增的量。
		UVale _UCIAPI uci_UnitRefresh(double _value, char _unit, double _scale);

#ifdef __cplusplus
	}
}
#endif

#endif // Unit_h__