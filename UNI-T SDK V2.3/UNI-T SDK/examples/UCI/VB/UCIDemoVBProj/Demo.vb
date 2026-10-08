Module Demo
    '目标CPU必须设置为x86平台,将uci.dll和ucivb.dll拷贝到工程运行目录下并添加uci.vb引用到工程中，即可使用接口
    Sub Main()

        Dim ret As Integer = 0
        Dim seesion As UInteger = 0

        '简单版搜索设备API
        'Dim addr(10 * 256) As Char '表示只搜索10个设备，每个设备地址最大字符串长度不超过256个字节，搜索到的每一个设备地址以逗号隔开
        'ret = ucivb.UCI.QueryNodes_Simple(addr, UBound(addr), ucivb.NodeType.USB, 2000)

        '搜索设备API
        Dim m_Nodes(9) As ucivb.Node
        Dim msg As String = ""
        Dim type As UInteger = ucivb.NodeType.USB
        Select Case type
            Case ucivb.NodeType.USB
                msg = "USB:0x1234&0x5345,0x7777&0x5345,0x0834&0x5656,0x5537&0x4348, 0xE008&0x1A86;"
            Case ucivb.NodeType.LAN
                msg = "LAN:5000,4162,8000,18191;"
        End Select
        ret = ucivb.UCI.QueryNodes(msg, m_Nodes, UBound(m_Nodes) + 1, 2000)

        '简单版打开设备API
        'seesion = ucivb.UCI.Open_Simple(m_Nodes(0).UCIAddr, 2000)
        '打开设备API
        ret = ucivb.UCI.Open(m_Nodes(0).UCIAddr, seesion, 2000)

        '读配置文件测试
        Dim filePath As String = Format(Now, "yyyymmdd_hhmmss") & ".set"
        Dim rfp As ucivb.RFileParams
        rfp.CMDString = "dconfig;"
        rfp.FilePath = filePath
        rfp.TimeOut = 2000
        rfp.FilePathFinal = ""
        ret = ucivb.UCI.ReadToFile(seesion, rfp)

        '写配置文件测试
        Dim wfp As ucivb.WFileParams
        wfp.CMDString = "dconfig;"
        wfp.FilePath = rfp.FilePathFinal
        wfp.TimeOut = 2000
        ret = ucivb.UCI.WriteFromFile(seesion, wfp)
        '简单版写文件API
        'ret = ucivb.UCI.WriteFromFileX(seesion, "dconfig;", rfp.FilePathFinal, 2000)

        '写指令API
        ucivb.UCI.Write(seesion, "Proc:STOP;", 2000)

        '读指令API
        Dim data(1024) As Byte
        ucivb.UCI.Read(seesion, "mea:all?;", 2000, data, 400)

        'Dim filePath As String = Format(Now, "yyyymmdd_hhmmss") & ".csv"
        ''简单版读数据到文件API
        ''Dim MAX_PATH_LENGTH As UInteger = 260
        ''Dim finalFilePath(MAX_PATH_LENGTH) As Char
        ''ret = ucivb.UCI.ReadToFileX(seesion, "capture wave:.csv@CH:0@DT:vol;", filePath, 2000, finalFilePath, MAX_PATH_LENGTH)
        ''读数据到文件API
        'Dim rfp As ucivb.RFileParams
        'rfp.CMDString = "capture wave:.csv@CH:0@DT:vol;"
        'rfp.FilePath = filePath
        'rfp.TimeOut = 2000
        'rfp.FilePathFinal = ""
        'ret = ucivb.UCI.ReadToFile(seesion, rfp)

        '获取设备产生的最后一个错误信息
        'Dim errorInfo As String = ""
        'errorInfo = ucivb.UCI.GetLastError()

        '关闭设备
        ret = ucivb.UCI.Close(seesion)

    End Sub

End Module
