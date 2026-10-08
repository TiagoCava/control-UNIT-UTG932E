#include "main.h"

//==============================================================================
//
// Title:       UCIDEMO
// Purpose:     A short description of the application.
//
// Created on:  2016/10/27 at 14:20:19 by admin.
// Copyright:   . All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>							 
#include <cvirte.h>     
#include <userint.h>
#include "UCIDEMO.h"
#include "toolbox.h"
#include "include\uci.h"
#include "include\dso_base.h"         
#include "include\unit.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;

static u_session g_session = INVALID_SESSION;

typedef struct _UCIParams{
	  u_session Session;
	  MeaValue Params[50];
}UCIParams;

static UCIParams g_UCIParams;
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
int HandleResult(char* tips, u_status r){			  
	if(UCIERR(r))  
		printf("%s : r = %d, %s\n", tips == NULL ? "--" : tips, r, uci_GetLastError());
	return r;
}

int OpenDevice(){
			//@brief          : 与设备建立连接
	//@param _addr    : 设备连接字符串， 以'\0'结尾
	//@param _timeOut : 连接超时时间 ms
	//@return :  < 0 错误码； >=0 会话ID
	//@remarks : 本接口是uci_Open的简化版本。
//	u_status _UCIAPI uci_OpenX(u_cstring _addr, u_uint32 _timeOut);
	
	u_status r = HandleResult( 		
		"Open devcie",
		uci_OpenX("[C:DSO][D:UPO2000CS][T:USB][PID:0x1234][VID:0x5345][EI:0x81][EO:0x3][CFG:3]", 2000)
	);		 
	if(UCISUCCESS(r))
		g_UCIParams.Session = (u_session)r;    
	return r;
}

void InitMeasure(){		  
	u_status r = 0;			
	
	if(g_UCIParams.Session == INVALID_SESSION) {  
		printf("please open device first!\n");        
		return;
	}
	
	//@brief : 格式化写入数据
	//@param u_session _sesn        : 会话ID
	//@param u_uint32 _timeOut      : 写超时(单位：ms)
	//@param const u_tchar * format : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param ... : 
	//@return u_status : 查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 
	//u_status _UCIAPI uci_FormatWrite(u_session _sesn, u_uint32 _timeOut, const u_tchar *format, ...);
		
	r = HandleResult( 		
		"Enabel Measure",
		uci_FormatWrite(g_UCIParams.Session, 1000, "mea@ALL:1;")
	);
	
	 r = HandleResult( 		
		"Switch to measure CH1",
		uci_FormatWrite(g_UCIParams.Session, 1000, "mea@src:0;")
	);
}

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;

    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "UCIDEMO.uir", PANEL));
	
	g_UCIParams.Session = INVALID_SESSION;
	memset(g_UCIParams.Params, 0, sizeof(g_UCIParams.Params));
	
	if(OpenDevice() >= 0)
		InitMeasure();
	
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());  
Error:
    /* clean up */
    DiscardPanel (panelHandle);
    return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}	

int ReadMeaParams(void){
		//@brief          : 读取数据
	//@param _session : 会话ID
	//@param _msg     : 命令字符串，以'\0'结尾. 具体格式见文档；
	//@param _timeout : 读取超时。
	//@param _data    : 接收读取到的数据的缓冲区地址。
	//@param _dataLen : _data指向的缓冲区长度（Bytes）
	//@return : 	查看本文件中的“关于接口返回的状态值”说明块
	//@remarks : 本接口是uci_Read的简化版本
//	u_status _UCIAPI uci_ReadX(u_session _session, u_cstring _msg, 
//		u_uint32 _timeout, u_byte* _data, u_size _dataLen);					 

	return HandleResult(  
		"Read measure result " ,
		uci_ReadX(g_UCIParams.Session, "mea:all?;", 1000, (u_byte*)g_UCIParams.Params, sizeof(g_UCIParams.Params))
		);
}

MeaValue* GetParam(EMeaParam id){
	return &g_UCIParams.Params[id];
}

void ShowMeaParam(char* name, EMeaParam id){
	MeaValue* p = NULL;  
	if(name == NULL)
		return;  
	
	p = GetParam(id);
	
	if(p->IsExist == 0)			   
		printf("%s = Not exist\n", name);			  
	else{  							 
		if(p->IsValid == 1){					  
			printf("%s = %f\n",name, p->Value);   	  
		}else{		   					
			printf("%s = --\n", name);  			
		}	 		
	}
}

int CVICALLBACK _Read (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(ReadMeaParams() >=0 ){
				ShowMeaParam("FREQ", MP_FREQ);
				ShowMeaParam("MAX", MP_MAX); 
				ShowMeaParam("AMP", MP_AMP); 
			}
			//	_numFreq = g_UCIParams.Params[MP_FREQ].Value;
			break;
	}
	return 0;
}

int CVICALLBACK _timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if(ReadMeaParams() >=0 ){
				ShowMeaParam("FREQ", MP_FREQ);
				ShowMeaParam("MAX", MP_MAX); 
				ShowMeaParam("AMP", MP_AMP); 
			}
			break;
	}
	return 0;
}
