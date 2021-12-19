
// DirectionalDiagram.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CDirectionalDiagramApp:
// Сведения о реализации этого класса: DirectionalDiagram.cpp
//

class CDirectionalDiagramApp : public CWinApp
{
public:
	CDirectionalDiagramApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CDirectionalDiagramApp theApp;
