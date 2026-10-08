using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using uci;

/*
 *  本工程演示基本的设备访问接口，如查询、打开、读、写、关闭等等。
 *  以示波器为演示机型。
 */ 
namespace SimpleIO
{
    class Program
    {
        /// <summary>
        /// 会话类，也可以使用UCIInterop的适配接口，但最终都是调用的uci.dll中的接口
        /// </summary>
        static Session _com = new Session(true);
       
        static void Main(string[] args)
        {
            //{全局配置
            //切换所有打印输出为中文；
            UCIInterop.SetAttribute(UCIInterop.InvalidSession,"lang:zh-Hans;", null, 0);
            //切换所有打印输出为英文；
            //UCIInterop.SetAttribute(UCIInterop.InvalidSession, "lang:en-US;", null, 0);
            //}

            //{查询设备示例
            string queryMsg = "USB:0x1234&0x5345,0x7777&0x5345,0x0834&0x5656,0x5537&0x4348"; //查询USB设备
            //queryMsg += ";LAN:4162,5000";//查询网络设备
            queryMsg += ";";
            uci.Node[] nodes = new uci.Node[10];
            int count = UCIInterop.QueryNodesX(queryMsg, nodes, nodes.Length, 2000);
            if (count > 0)
            {
                Console.WriteLine("查询到的满足条件的USB设备数量:{0}", count);
                for (int i = 0; i < count; i++)
                {
                    Console.WriteLine("[{0}]\t:\t{1}", i, nodes[i].UCIAddr);
                }
            }
            else
            {
                OutputResult(count, "查询设备");    
            }
            //}

            //{查询串口设备
            // 此查询不包括任何的协议，只是查询串口。如果要查询指定协议的设备，请遍历打开串口
            // 并使用指定协议进行验证。
            uci.COMPort[] comPorts = new uci.COMPort[10];
            count = UCIInterop.QueryCOMPort(comPorts, comPorts.Length);
            if (count > 0)
            {
                Console.WriteLine("Ports:{0}", count);
                for (int i = 0; i < count; i++)
                {
                    Console.WriteLine("{0} - COM{1} : {2}", i, comPorts[i].Port, comPorts[i].FriendlyName);
                }
            }
            else if (count == 0)
            {
                Console.WriteLine("未查询到任何串口设备");
            }
            else
            {
                OutputResult(count, "查询串口设备"); 
            }
            //* Ports:1
            //* 0 - COM3 : Silicon Labs CP210x USB to UART Bridge (COM3)       
            //}
              
            
            //打开设备,设备地址获取方式：
            //1 可以从文档中获取；
            //2 使用UCIInterop.QueryNodesX查询获取；
            //3 使用“SDK\examples\VC\UCIDEMO\Unicode\UCI_DEMO.exe"程序，查询获取。
            string addr = "[C:DSO][D:DSO-E][T:USB][PID:0x5537][VID:0x4348][EI:0x82][EO:0x2][CFG:1][I:0][addr:0]";
            //addr = "[C:UPO][D:UPO][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x3][CFG:3][I:0]";
            var r = _com.Open(addr);         
            if (!OutputResult(r, "打开设备"))
            {//error : r < 0；  
                //特别的，如果返回-1040（建立连接的字符串地址格式错误），说明当前工程
                //和调用的uci.dll的字符编码不一致，一般C#的工程都是UNICODE的版本，
                //请拷贝“SDK\lib\C\Unicode\uci.dll”到当前工程的执行程序的运行目录下。
                return;
            }

            //写参数
            r = _com.Write("CH:0@sel;");
            OutputResult(r, "写参数");         

            //读参数测量结果
            //{打包读取所有参数测量结果--适用于所有示波器
            byte[] allMeaBytes = new byte[50 * 8]; //固定的50个参数，一个参数8Bytes
            r = _com.Read("mea:all?;", allMeaBytes);
            if (OutputResult(r, "读参数测量"))
            {
                //转换为结构体
                uci.Model.mea.MeaValue[] allMea = new uci.Model.mea.MeaValue[50];
                uci.Utility.ByteArrayToStructArray(allMeaBytes, ref allMea, 50);
                //打印输出
                ParseParamsMeasure(allMea);
            }
            //}

            //读取单个参数 - 请参照文档查看是否支持。
#if false
            double v = 0.0;
            r = _com.Read("mea:freq;", out v);
#endif

            //{读取波形文件
            // 指令格式（capture wave:.csv@CH:0@DT:vol;）请参照具体的文档，个别机型有差异。  
            uci.RFileParams rfp = new RFileParams()
            {
                CMDString = "capture wave:.csv@CH:0@DT:vol;",
                FilePath = "waveform.csv",
                TimeOut = 2000,
            };
            r = _com.ReadToFile(ref rfp);
            OutputResult(r, "读波形文件");
            //}

            //Session析构时自动关闭
        }

        /// <summary>
        /// 输出UCI API的接口返回的结果
        /// </summary>
        /// <param name="r"></param>
        /// <param name="msg"></param>
        /// <returns></returns>
        static bool OutputResult(int r, string msg)
        {
            string s = "[" + msg + "]";
            if (Session.Error(r))
            {
                s += string.Format("失败，错误码：{0}，错误信息：{1}",
                    r, _com.GetLastError());
            }
            else
            {
                s += "成功！";
            }
            Console.WriteLine(s);
            return r >= 0;
        }

        /// <summary>
        /// 得到物理单位量级 
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        static string GetUnitScaleName(uci.Model.unit.Scale s)
        {
            if (s == uci.Model.unit.Scale.STD)
                return string.Empty;
            return s.ToString();
        }

        /// <summary>
        /// 打印参数测量的所有结果
        /// </summary>
        /// <param name="m"></param>
        static void ParseParamsMeasure(uci.Model.mea.MeaValue[] m)
        {
            for(int i = 0; i < m.Length; i++)
            {
                Console.WriteLine("{0}:IsExist={1},IsValid={2}, Value={3}, Unit(Type={4},Scale={5}) --> {6}{7}{8}", 
                    (uci.Model.mea.MeaParam)(i), 
                    m[i].IsExist, m[i].IsValid, m[i].Value, m[i].Unit.Type, m[i].Unit.Scale,
                    m[i].Value,
                    GetUnitScaleName((uci.Model.unit.Scale)m[i].Unit.Scale),
                    //获取物理量单位，可以使用uci API ： UCIInterop.GetUnitTypeName
                    //也可以自己建表
                    UCIInterop.GetUnitTypeName((char)m[i].Unit.Type));
            }
        }
    }
}
