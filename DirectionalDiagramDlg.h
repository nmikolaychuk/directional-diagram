
// DirectionalDiagramDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CDirectionalDiagramDlg
class CDirectionalDiagramDlg : public CDialogEx
{
// Создание
public:
	CDirectionalDiagramDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTIONALDIAGRAM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	/////////////////////////////////////
	/// Переменные, связанные с интерфейсом
	/////////////////////////////////////

	double EMIT_DISTANCE;
	double EMIT_HEIGHT;
	double EMIT_WAVE_LENGTH;
	double EMIT_WIDTH;
	double SPHERE_HEIGHT;
	double SPHERE_RADIUS;
	double SPHERE_WIDTH;

	/////////////////////////////////////
	/// Обработчики кнопок
	/////////////////////////////////////

	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonCalculate();
};
