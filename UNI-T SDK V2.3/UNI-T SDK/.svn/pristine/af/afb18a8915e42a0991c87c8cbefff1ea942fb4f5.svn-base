
// UCIDemoProj.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUCIDemoProjApp: 
// 有关此类的实现，请参阅 UCIDemoProj.cpp
//

enum ELANGE {
	E_LANG_CHINESE = 0,
	E_LANG_ENGLISH,
};

class CUCIDemoProjApp : public CWinApp
{
public:
	CUCIDemoProjApp();

	
	ELANGE GetSystemLange() const { return m_SystemLange; }
	// 重写
public:
	virtual BOOL InitInstance();
	// 实现

	DECLARE_MESSAGE_MAP()

protected:
	void InitLanguage(ELANGE _lang);
private:
	ELANGE m_SystemLange;
	bool m_IsNewOS;
};

extern CUCIDemoProjApp theApp;