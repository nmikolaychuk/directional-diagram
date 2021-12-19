
// DirectionalDiagramDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "DirectionalDiagram.h"
#include "DirectionalDiagramDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CDirectionalDiagramDlg



CDirectionalDiagramDlg::CDirectionalDiagramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIRECTIONALDIAGRAM_DIALOG, pParent)
	, SPHERE_RADIUS(0)
	, SPHERE_WIDTH(0)
	, SPHERE_HEIGHT(0)
	, EMIT_WIDTH(0)
	, EMIT_HEIGHT(0)
	, EMIT_DISTANCE(0)
	, EMIT_WAVE_LENGTH(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDirectionalDiagramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPHERE_RADIUS, SPHERE_RADIUS);
	DDX_Text(pDX, IDC_EDIT_SPHERE_WIDTH, SPHERE_WIDTH);
	DDX_Text(pDX, IDC_EDIT_SPHERE_HEIGHT, SPHERE_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_EMIT_WIDTH, EMIT_WIDTH);
	DDX_Text(pDX, IDC_EDIT_EMIT_HEIGHT, EMIT_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_EMIT_DISTANCE, EMIT_DISTANCE);
	DDX_Text(pDX, IDC_EDIT_EMIT_WAVE_LENGTH, EMIT_WAVE_LENGTH);
	DDX_Text(pDX, IDC_EDIT_EMIT_DISTANCE, EMIT_DISTANCE);
	DDX_Text(pDX, IDC_EDIT_EMIT_HEIGHT, EMIT_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_EMIT_WAVE_LENGTH, EMIT_WAVE_LENGTH);
	DDX_Text(pDX, IDC_EDIT_EMIT_WIDTH, EMIT_WIDTH);
	DDX_Text(pDX, IDC_EDIT_SPHERE_HEIGHT, SPHERE_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_SPHERE_RADIUS, SPHERE_RADIUS);
	DDX_Text(pDX, IDC_EDIT_SPHERE_WIDTH, SPHERE_WIDTH);
}

BEGIN_MESSAGE_MAP(CDirectionalDiagramDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CDirectionalDiagramDlg::OnBnClickedButtonCalculate)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDirectionalDiagramDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// Обработчики сообщений CDirectionalDiagramDlg

BOOL CDirectionalDiagramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CDirectionalDiagramDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CDirectionalDiagramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDirectionalDiagramDlg::OnBnClickedButtonCalculate()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDirectionalDiagramDlg::OnBnClickedButtonClear()
{
	// TODO: добавьте свой код обработчика уведомлений
}
