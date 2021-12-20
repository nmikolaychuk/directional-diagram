#pragma once
#include "pch.h"
#include "framework.h"
#include "DirectionalDiagram.h"
#include "DirectionalDiagramDlg.h"
#include "afxdialogex.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DOTS(x,y) (emitters_xp*((x)-emitters_xmin)),(emitters_yp*((y)-emitters_ymax))

using namespace std;

CDirectionalDiagramDlg::CDirectionalDiagramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIRECTIONALDIAGRAM_DIALOG, pParent)
	, SPHERE_RADIUS(300)
	, SPHERE_PLATFORM_SIZE(15)
	, EMIT_PLATFORM_SIZE(10)
	, EMIT_DISTANCE(1)
	, EMIT_WAVE_LENGTH(2)
	, GRID_STEP_3D(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDirectionalDiagramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPHERE_RADIUS, SPHERE_RADIUS);
	DDX_Text(pDX, IDC_EDIT_SPHERE_WIDTH, SPHERE_PLATFORM_SIZE);
	DDX_Text(pDX, IDC_EDIT_EMIT_WIDTH, EMIT_PLATFORM_SIZE);
	DDX_Text(pDX, IDC_EDIT_EMIT_DISTANCE, EMIT_DISTANCE);
	DDX_Text(pDX, IDC_EDIT_EMIT_WAVE_LENGTH, EMIT_WAVE_LENGTH);
	DDX_Text(pDX, IDC_EDIT_3D_GRID_STEP, GRID_STEP_3D);
	DDX_Control(pDX, IDC_CHECK_3D_AXIS, IS_DRAW_AXIS_3D);
	DDX_Control(pDX, IDC_CHECK_3D_GRID, IS_DRAW_GRID_3D);
	DDX_Control(pDX, IDC_CHECK_3D_TEXT, IS_DRAW_TEXT_3D);
	DDX_Control(pDX, IDC_RADIO_3D_GRAPH_REAL, RADIO_DRAW_REAL);
	DDX_Control(pDX, IDC_RADIO_3D_GRAPH_IMAGE, RADIO_DRAW_IMAGE);
	DDX_Control(pDX, IDC_RADIO_3D_GRAPH_MODULE, RADIO_DRAW_MODULE);
}

BEGIN_MESSAGE_MAP(CDirectionalDiagramDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CDirectionalDiagramDlg::OnBnClickedButtonCalculate)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDirectionalDiagramDlg::OnBnClickedButtonClear)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Обработчики сообщений CDirectionalDiagramDlg

BOOL CDirectionalDiagramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	PicWnd = GetDlgItem(IDC_PICTURE_ADD_EMITTER);			//связываем с ID окон
	PicDc = PicWnd->GetDC();
	PicWnd->GetClientRect(&Pic);

	zoom = 280.;
	spin_x = -60.;
	spin_y = 0.;
	spin_z = 120.;

	RADIO_DRAW_MODULE.SetCheck(TRUE);
	IS_DRAW_GRID_3D.SetCheck(TRUE);
	pDC_3d = GetDlgItem(IDC_PICTURE_3D_DIAGRAM)->GetDC();
	InitiateOPGL();

	UpdateData(TRUE);

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

		DrawImage(vec_source_radiation, PicDc, Pic);
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CDirectionalDiagramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDirectionalDiagramDlg::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |            // support OpenGL
		PFD_DOUBLEBUFFER,               // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	int pixelformat;
	if ((pixelformat = ChoosePixelFormat(pDC_3d->GetSafeHdc(), &pfd)) == 0)
	{
		MessageBox(L"ChoosePixelFormat failed!", L"ERROR", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (SetPixelFormat(pDC_3d->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		MessageBox(L"SetPixelFormat failed!", L"ERROR", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

GLvoid CDirectionalDiagramDlg::BuildFont(GLvoid)
{
	HFONT  font;            // Идентификатор фонта
	base = glGenLists(96);  // Выделим место для 96 символов ( НОВОЕ )

	font = CreateFont(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH, L"Century Gothic");

	SelectObject(pDC_3d->GetSafeHdc(), font);

	wglUseFontBitmaps(pDC_3d->GetSafeHdc(), 32, 96, base);
}

GLvoid CDirectionalDiagramDlg::KillFont(GLvoid)            // Удаление шрифта
{
	glDeleteLists(base, 96);        // Удаление всех 96 списков отображения ( НОВОЕ )
}

GLvoid CDirectionalDiagramDlg::glPrint(const char* fmt, ...)
{
	char text[256];			// Место для нашей строки
	va_list ap;				// Указатель на список аргументов

	if (fmt == NULL)     // Если нет текста
		return;            // Ничего не делать

	va_start(ap, fmt);           // Разбор строки переменных
	vsprintf_s(text, fmt, ap); // И конвертирование символов в реальные коды
	va_end(ap);                  // Результат помещается в строку

	glPushAttrib(GL_LIST_BIT);      // Протолкнуть биты списка отображения
	glListBase(base - 32);          // Задать базу символа в 32

	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);// Текст списками отображения
	glPopAttrib();	// Возврат битов списка отображения
}

void CDirectionalDiagramDlg::InitiateOPGL()
{
	HGLRC hrc;

	if (!bSetupPixelFormat())
	{
		return;
	}

	hrc = wglCreateContext(pDC_3d->GetSafeHdc());
	if (hrc != NULL)
	{
		wglMakeCurrent(pDC_3d->GetSafeHdc(), hrc);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		gluLookAt(0, 0, -1, 0, 0, 1, 0, 100, 0);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClearDepth(1.);
		BuildFont();
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		glMatrixMode(GL_MODELVIEW);
	}
}

double CDirectionalDiagramDlg::findmax(vector<vector<complex<double>>> D, bool max_min)
{
	if (RADIO_DRAW_REAL.GetCheck() == BST_CHECKED)
	{
		double find = D[0][0].real();

		if (max_min)
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (D[i][j].real() > find)
					{
						find = D[i][j].real();
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (D[i][j].real() < find)
					{
						find = D[i][j].real();
					}
				}
			}
		}
		return find;
	}

	if (RADIO_DRAW_IMAGE.GetCheck() == BST_CHECKED)
	{
		double find = D[0][0].imag();

		if (max_min)
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (D[i][j].imag() > find)
					{
						find = D[i][j].imag();
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (D[i][j].imag() < find)
					{
						find = D[i][j].imag();
					}
				}
			}
		}
		return find;
	}

	if (RADIO_DRAW_MODULE.GetCheck() == BST_CHECKED)
	{
		double find = sqrt(D[0][0].real() * D[0][0].real() + D[0][0].imag() * D[0][0].imag());

		if (max_min)
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (sqrt(D[i][j].real() * D[i][j].real() + D[i][j].imag() * D[i][j].imag()) > find)
					{
						find = sqrt(D[i][j].real() * D[i][j].real() + D[i][j].imag() * D[i][j].imag());
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < D.size(); i++)
			{
				for (int j = 0; j < D[i].size(); j++)
				{
					if (sqrt(D[i][j].real() * D[i][j].real() + D[i][j].imag() * D[i][j].imag()) < find)
					{
						find = sqrt(D[i][j].real() * D[i][j].real() + D[i][j].imag() * D[i][j].imag());
					}
				}
			}
		}
		return find;
	}
}

void CDirectionalDiagramDlg::DrawGL(vector<vector<complex<double>>> Wave)
{
	if (RADIO_DRAW_REAL.GetCheck() == BST_CHECKED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);
		glRotated(spin_x, 1.f, 0.f, 0.f);
		glRotated(spin_y, 0.f, 1.f, 0.f);
		glRotated(spin_z, 0.f, 0.f, 1.f);

		double mash = zoom / 200;
		if (firsttimedraw)//1
		{
			max = findmax(Wave, 1);
			min = findmax(Wave, 0);
			if (abs(max) > abs(min))
			{
				absmax = abs(max);
			}
			else
			{
				absmax = abs(min);
			}
			maxmin = abs(max) + abs(min);
			firsttimedraw = 0;
		}
		glPointSize(1);

		for (int i = 0; i < Wave.size() - 1; i++)
		{
			for (int j = 0; j < Wave[i].size() - 1; j++)
			{
				if (IS_DRAW_AXIS_3D.GetCheck() == BST_CHECKED)
				{
					glLineWidth(9);
					glBegin(GL_LINE_STRIP);
					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(SPHERE_PLATFORM_SIZE - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, max);
					glEnd();
				}

				if (IS_DRAW_TEXT_3D.GetCheck() == BST_CHECKED)
				{
					// 0
					glLineWidth(1);
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", 0.);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)((double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					if (i % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)i);  // Печать текста GL на экран
					}
					if (j % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(j - (double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)j);  // Печать текста GL на экран
					}
				}

				glLineWidth(1);
				glBegin(GL_TRIANGLE_STRIP);

				//glColor3d((Wave[i][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j].real() + max) / maxmin);
				glColor3d(abs(1-(Wave[i][j].real() + max) / maxmin) * 2, 0.0f, (Wave[i][j].real() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].real() / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i+1][j].real() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j].real() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j].real() / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i][j+1].real() + max) / maxmin) * 2, 0.0f, (Wave[i][j + 1].real() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + 1].real() / 2 / absmax / mash);

				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i+1][j].real() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j].real() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j].real() / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i][j+1].real() + max) / maxmin) * 2, 0.0f, (Wave[i][j + 1].real() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + 1].real() / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i+1][j + 1].real() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j + 1].real() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j + 1].real() / 2 / absmax / mash);
				glEnd();
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (IS_DRAW_GRID_3D.GetCheck() == BST_CHECKED && GRID_STEP_3D != 0)
		{
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].real() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j].real() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + GRID_STEP_3D].real() / 2 / absmax / mash + 1. / 50 / mash);
				}
			}
			glEnd();
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].real() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j].real() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + GRID_STEP_3D].real() / 2 / absmax / mash - 1. / 50 / mash);
				}
			}
			glEnd();
		}
		glFinish();
		SwapBuffers(wglGetCurrentDC());
	}

	if (RADIO_DRAW_IMAGE.GetCheck() == BST_CHECKED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);
		glRotated(spin_x, 1.f, 0.f, 0.f);
		glRotated(spin_y, 0.f, 1.f, 0.f);
		glRotated(spin_z, 0.f, 0.f, 1.f);

		double mash = zoom / 200;
		if (firsttimedraw)//1
		{
			max = findmax(Wave, 1);
			min = findmax(Wave, 0);
			if (abs(max) > abs(min))
			{
				absmax = abs(max);
			}
			else
			{
				absmax = abs(min);
			}
			maxmin = abs(max) + abs(min);
			firsttimedraw = 0;
		}
		glPointSize(1);

		for (int i = 0; i < Wave.size() - 1; i++)
		{
			for (int j = 0; j < Wave[i].size() - 1; j++)
			{
				if (IS_DRAW_AXIS_3D.GetCheck() == BST_CHECKED)
				{
					glLineWidth(9);
					glBegin(GL_LINE_STRIP);
					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(SPHERE_PLATFORM_SIZE - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, max);
					glEnd();
				}

				if (IS_DRAW_TEXT_3D.GetCheck() == BST_CHECKED)
				{
					// 0
					glLineWidth(1);
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", 0.);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)((double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					if (i % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)i);  // Печать текста GL на экран
					}
					if (j % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(j - (double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)j);  // Печать текста GL на экран
					}
				}

				glLineWidth(1);
				glBegin(GL_TRIANGLE_STRIP);
				//glColor3d((Wave[i][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i][j].imag() + max) / maxmin) * 2, 0.0f, (Wave[i][j].imag() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].imag() / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i + 1][j].imag() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j].imag() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j].imag() / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i][j + 1].imag() + max) / maxmin) * 2, 0.0f, (Wave[i][j + 1].imag() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + 1].imag() / 2 / absmax / mash);

				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i + 1][j].imag() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j].imag() + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j].imag() / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i][j + 1].imag() + max) / maxmin) * 2, 0.0f, (Wave[i][j + 1].imag() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + 1].imag() / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (Wave[i + 1][j + 1].imag() + max) / maxmin) * 2, 0.0f, (Wave[i + 1][j + 1].imag() + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + 1][j + 1].imag() / 2 / absmax / mash);
				glEnd();
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (IS_DRAW_GRID_3D.GetCheck() == BST_CHECKED)
		{
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].imag() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j].imag() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag() / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + GRID_STEP_3D].imag() / 2 / absmax / mash + 1. / 50 / mash);
				}
			}
			glEnd();
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j].imag() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j].imag() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag() / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, Wave[i][j + GRID_STEP_3D].imag() / 2 / absmax / mash - 1. / 50 / mash);
				}
			}
			glEnd();
		}
		glFinish();
		SwapBuffers(wglGetCurrentDC());
	}

	if (RADIO_DRAW_MODULE.GetCheck() == BST_CHECKED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);
		glRotated(spin_x, 1.f, 0.f, 0.f);
		glRotated(spin_y, 0.f, 1.f, 0.f);
		glRotated(spin_z, 0.f, 0.f, 1.f);

		double mash = zoom / 200;
		if (firsttimedraw)//1
		{
			max = findmax(Wave, 1);
			min = findmax(Wave, 0);
			if (abs(max) > abs(min))
			{
				absmax = abs(max);
			}
			else
			{
				absmax = abs(min);
			}
			maxmin = abs(max) + abs(min);
			firsttimedraw = 0;
		}
		glPointSize(1);

		for (int i = 0; i < Wave.size() - 1; i++)
		{
			for (int j = 0; j < Wave[i].size() - 1; j++)
			{
				if (IS_DRAW_AXIS_3D.GetCheck() == BST_CHECKED)
				{
					glLineWidth(9);
					glBegin(GL_LINE_STRIP);
					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(1.0f, 0.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 1.0f, 0.0f);
					glVertex3d((double)(SPHERE_PLATFORM_SIZE - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);

					glColor3d(0.0f, 0.0f, 1.0f);
					glVertex3d((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, max);
					glEnd();
				}

				if (IS_DRAW_TEXT_3D.GetCheck() == BST_CHECKED)
				{
					// 0
					glLineWidth(1);
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", 0.);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)((double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					// SPHERE_PLATFORM_SIZE
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)((double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
					glPrint("%5.0f", (double)SPHERE_PLATFORM_SIZE);  // Печать текста GL на экран

					if (i % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(-(double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)i);  // Печать текста GL на экран
					}
					if (j % 10 == 0)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos3f((double)(-(double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(j - (double)SPHERE_PLATFORM_SIZE / 2)
							* 2 / (double)SPHERE_PLATFORM_SIZE / mash, min);
						glPrint("%5.0f", (double)j);  // Печать текста GL на экран
					}
				}

				double wave_ij = sqrt(Wave[i][j].real() * Wave[i][j].real() + Wave[i][j].imag() * Wave[i][j].imag());
				double wave_i1j = sqrt(Wave[i + 1][j].real() * Wave[i + 1][j].real() + Wave[i + 1][j].imag() * Wave[i + 1][j].imag());
				double wave_ij1 = sqrt(Wave[i][j + 1].real() * Wave[i][j + 1].real() + Wave[i][j + 1].imag() * Wave[i][j + 1].imag());
				double wave_i1j1 = sqrt(Wave[i + 1][j + 1].real() * Wave[i + 1][j + 1].real() + Wave[i + 1][j + 1].imag() * Wave[i + 1][j + 1].imag());

				glLineWidth(1);
				glBegin(GL_TRIANGLE_STRIP);
				//glColor3d((Wave[i][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_ij + max) / maxmin) * 2, 0.0f, (wave_ij + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_ij / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_i1j + max) / maxmin) * 2, 0.0f, (wave_i1j + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_i1j / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_ij1 + max) / maxmin) * 2, 0.0f, (wave_ij1 + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_ij1 / 2 / absmax / mash);

				//glColor3d((Wave[i + 1][j].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_i1j + max) / maxmin) * 2, 0.0f, (wave_i1j + max) / maxmin);
				glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_i1j / 2 / absmax / mash);
				//glColor3d((Wave[i][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_ij1 + max) / maxmin) * 2, 0.0f, (wave_ij1 + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_ij1 / 2 / absmax / mash);
				//glColor3d((Wave[i + 1][j + 1].real() + max) / maxmin, 0.0f, 1 - (Wave[i + 1][j + 1].real() + max) / maxmin);
				glColor3d(abs(1 - (wave_i1j1 + max) / maxmin) * 2, 0.0f, (wave_i1j1 + max) / maxmin);
				glVertex3d((double)(j + 1 - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + 1 - (double)SPHERE_PLATFORM_SIZE / 2)
					* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_i1j1 / 2 / absmax / mash);
				glEnd();
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (IS_DRAW_GRID_3D.GetCheck() == BST_CHECKED)
		{
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					double wave_ij = sqrt(Wave[i][j].real() * Wave[i][j].real() + Wave[i][j].imag() * Wave[i][j].imag());
					double wave_iSTEPj = sqrt(Wave[i + GRID_STEP_3D][j].real() * Wave[i + GRID_STEP_3D][j].real()
						+ Wave[i + GRID_STEP_3D][j].imag() * Wave[i + GRID_STEP_3D][j].imag());
					double wave_ijSTEP = sqrt(Wave[i][j + GRID_STEP_3D].real() * Wave[i][j + GRID_STEP_3D].real()
						+ Wave[i][j + GRID_STEP_3D].imag() * Wave[i][j + GRID_STEP_3D].imag());
					double wave_iSTEPjSTEP = sqrt(Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real() * Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real()
						+ Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag() * Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag());

					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, wave_ij / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, wave_iSTEPj / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, wave_iSTEPjSTEP / 2 / absmax / mash + 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2) * 2 /
						(double)SPHERE_PLATFORM_SIZE / mash, wave_ijSTEP / 2 / absmax / mash + 1. / 50 / mash);
				}
			}
			glEnd();
			glBegin(GL_QUADS);
			for (int i = 0; i < Wave.size() - GRID_STEP_3D; i += GRID_STEP_3D)
			{
				for (int j = 0; j < Wave[i].size() - GRID_STEP_3D; j += GRID_STEP_3D)
				{
					double wave_ij = sqrt(Wave[i][j].real() * Wave[i][j].real() + Wave[i][j].imag() * Wave[i][j].imag());
					double wave_iSTEPj = sqrt(Wave[i + GRID_STEP_3D][j].real() * Wave[i + GRID_STEP_3D][j].real()
						+ Wave[i + GRID_STEP_3D][j].imag() * Wave[i + GRID_STEP_3D][j].imag());
					double wave_ijSTEP = sqrt(Wave[i][j + GRID_STEP_3D].real() * Wave[i][j + GRID_STEP_3D].real()
						+ Wave[i][j + GRID_STEP_3D].imag() * Wave[i][j + GRID_STEP_3D].imag());
					double wave_iSTEPjSTEP = sqrt(Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real() * Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].real()
						+ Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag() * Wave[i + GRID_STEP_3D][j + GRID_STEP_3D].imag());

					glColor3d(1.0f, 1.0f, 1.0f);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_ij / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_iSTEPj / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_iSTEPjSTEP / 2 / absmax / mash - 1. / 50 / mash);
					glVertex3d((double)(j + GRID_STEP_3D - (double)SPHERE_PLATFORM_SIZE / 2) * 2 / (double)SPHERE_PLATFORM_SIZE / mash, (double)(i - (double)SPHERE_PLATFORM_SIZE / 2)
						* 2 / (double)SPHERE_PLATFORM_SIZE / mash, wave_ijSTEP / 2 / absmax / mash - 1. / 50 / mash);
				}
			}
			glEnd();
		}
		glFinish();
		SwapBuffers(wglGetCurrentDC());
	}
}

void CDirectionalDiagramDlg::DrawImage(vector<vector<int>> vec, CDC* WinDc, CRect WinxmaxGraphc)
{
	UpdateData(TRUE);
	double GU_X = EMIT_PLATFORM_SIZE;
	double GU_Y = EMIT_PLATFORM_SIZE;

	emitters_xmin = -GU_X * 0.01;
	emitters_xmax = GU_X * 1.01;
	emitters_ymin = -GU_Y * 0.01;      //минимальное значение y
	emitters_ymax = GU_Y * 1.01;

	// создание контекста устройства
	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);

	double widthX = WinxmaxGraphc.Width();
	double heightY = WinxmaxGraphc.Height();
	emitters_xp = (widthX / (emitters_xmax - emitters_xmin));      //Коэффициенты пересчёта координат по Х
	emitters_yp = -(heightY / (emitters_ymax - emitters_ymin));      //Коэффициенты пересчёта координат по У

	bmp.CreateCompatibleBitmap(WinDc, widthX, heightY);
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);

	// заливка фона графика
	MemDc->FillSolidRect(WinxmaxGraphc, RGB(51, 76, 76));

	CPen line_pen;
	line_pen.CreatePen(    //для сетки
		PS_SOLID,          //пунктирная
		1,            //толщина 1 пиксель
		RGB(255, 0, 0));      //цвет  grey

	CBrush white_circle(RGB(255, 255, 255));      //цвет white
	CBrush black_circle(RGB(51, 76, 76));      //цвет black

	if (vec_source_radiation.empty())
	{
		vector<vector<int>> vecSource(EMIT_PLATFORM_SIZE, vector<int>(EMIT_PLATFORM_SIZE, 0));
		vec_source_radiation = vecSource;
	}

	for (int i = 0; i < EMIT_PLATFORM_SIZE; i++)
	{
		for (int j = 0; j < EMIT_PLATFORM_SIZE; j++)
		{
			double xxi = i;
			double yyi = j;

			if (vec_source_radiation[i][j] == 1)
			{
				MemDc->MoveTo(DOTS(xxi, yyi));
				CRect rect(DOTS(xxi, yyi + 1), DOTS(xxi + 1, yyi));
				LPCRECT lpRect = rect;
				MemDc->FillRect(lpRect, &white_circle);
			}
			else if (vec_source_radiation[i][j] == 0)
			{
				MemDc->MoveTo(DOTS(xxi, yyi));
				CRect rect(DOTS(xxi, yyi + 1), DOTS(xxi + 1, yyi));
				LPCRECT lpRect = rect;
				MemDc->FillRect(lpRect, &black_circle);
			}
		}
	}

	MemDc->SelectObject(&line_pen);
	MemDc->MoveTo(DOTS(0, 0));
	MemDc->LineTo(DOTS(0, EMIT_PLATFORM_SIZE));
	MemDc->LineTo(DOTS(EMIT_PLATFORM_SIZE, EMIT_PLATFORM_SIZE));
	MemDc->LineTo(DOTS(EMIT_PLATFORM_SIZE, 0));
	MemDc->LineTo(DOTS(0, 0));

	for (int i = 1; i < EMIT_PLATFORM_SIZE; i++)
	{
		MemDc->MoveTo(DOTS(i, 0));
		MemDc->LineTo(DOTS(i, EMIT_PLATFORM_SIZE));
	}

	for (int i = 1; i < EMIT_PLATFORM_SIZE; i++)
	{
		MemDc->MoveTo(DOTS(0, i));
		MemDc->LineTo(DOTS(EMIT_PLATFORM_SIZE, i));
	}

	// вывод на экран
	WinDc->BitBlt(0, 0, widthX, heightY, MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void CDirectionalDiagramDlg::CreateSphere(vector<vector<double>>& sphere) {
	if (sphere.size() != SPHERE_PLATFORM_SIZE)
	{
		sphere.resize(SPHERE_PLATFORM_SIZE);
	}
	double x = -SPHERE_PLATFORM_SIZE / 2, y = -SPHERE_PLATFORM_SIZE / 2;
	for (int i = 0; i < sphere.size(); i++)
	{
		if (sphere[i].size() != SPHERE_PLATFORM_SIZE)
		{
			sphere[i].resize(SPHERE_PLATFORM_SIZE);
		}
		x = -SPHERE_PLATFORM_SIZE / 2;
		for (int j = 0; j < sphere[i].size(); j++)
		{
			if (SPHERE_RADIUS * SPHERE_RADIUS < (x * x + y * y)) {
				sphere[i][j] = 0;
			}
			else {
				sphere[i][j] = sqrt(SPHERE_RADIUS * SPHERE_RADIUS - (x * x + y * y));
			}
			x++;
		}
		y++;
	}
}

struct Sources {
	double x;
	double y;
};

double CDirectionalDiagramDlg::rast(int x1, int y1, int z1, int x2, int y2)
{
	return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + z1 * z1));
}

void CDirectionalDiagramDlg::OnBnClickedButtonCalculate()
{
	UpdateData(TRUE);

	if (bTimer) {
		KillTimer(1);
		bTimer = false;
	}
	firsttimedraw = true;

	// Получение информации об источниках.
	vector <Sources> sources;
	for (int i = 0; i < vec_source_radiation.size(); i++) {
		for (int j = 0; j < vec_source_radiation[i].size(); j++) {
			if (vec_source_radiation[i][j] == 1) {
				Sources src;
				src.x = i;
				src.y = j;
				sources.push_back(src);
			}
		}
	}

	// Создание сферы.
	vector<vector<double>> sphere;
	CreateSphere(sphere);

	// Задание параметров.
	double r = 0.0;
	double wave_k = (2 * M_PI) / EMIT_WAVE_LENGTH;
	vec_diagram.clear();
	vec_diagram.resize(sphere.size());

	double x = -SPHERE_PLATFORM_SIZE / 2.;
	double y = -SPHERE_PLATFORM_SIZE / 2.;

	for (int i = 0; i < sphere.size(); i++) {
		vec_diagram[i].resize(sphere[i].size());
		x = -SPHERE_PLATFORM_SIZE / 2.;
		for (int j = 0; j < sphere[i].size(); j++) {
			complex<double> cmpl(0.0, 0.0);
			for (int k = 0; k < sources.size(); k++) {
				if (sphere[i][j] == 0) {
					vec_diagram[i][j] += 0;
				}
				else {
					r = rast(x, y, sphere[i][j], sources[k].x - EMIT_PLATFORM_SIZE / 2., sources[k].y - EMIT_PLATFORM_SIZE / 2.);
					cmpl.real(cos(wave_k * r));
					cmpl.imag(-sin(wave_k * r));
					vec_diagram[i][j] += cmpl / r;
				}
			}
			x++;
		}
		y++;
	}

	DrawGL(vec_diagram);

	int max_size_for_timer = 60;
	if (SPHERE_PLATFORM_SIZE <= max_size_for_timer) {
		SetTimer(1, 1, NULL);
		bTimer = true;
	}
}

void CDirectionalDiagramDlg::OnBnClickedButtonClear()
{
	UpdateData(TRUE);

	// Проверка заданного значения платформы излучателей.
	if ((int)EMIT_PLATFORM_SIZE > MAX_EMIT_PLATFORM_SIZE) {
		CString t;
		t.Format(_T("%d"), MAX_EMIT_PLATFORM_SIZE);
		MessageBox(L"Указан слишком большой размер платформы излучателей! Был задан параметр по умолчанию: m=n=" + t,
			L"Информация", MB_OK | MB_ICONINFORMATION);
		EMIT_PLATFORM_SIZE = MAX_EMIT_PLATFORM_SIZE;
		UpdateData(FALSE);
	}

	// Проверка заданного значения платформы сферы.
	if ((int)SPHERE_PLATFORM_SIZE > MAX_SPHERE_PLATFORM_SIZE) {
		CString t;
		t.Format(_T("%d"), MAX_SPHERE_PLATFORM_SIZE);
		MessageBox(L"Указан слишком большой размер платформы сферы! Был задан параметр по умолчанию: m=n=" + t,
			L"Информация", MB_OK | MB_ICONINFORMATION);
		SPHERE_PLATFORM_SIZE = MAX_SPHERE_PLATFORM_SIZE;
		UpdateData(FALSE);
	}

	vec_source_radiation.clear();
	vec_source_radiation.resize(EMIT_PLATFORM_SIZE);
	for (int i = 0; i < EMIT_PLATFORM_SIZE; i++) {
		vec_source_radiation[i].resize(EMIT_PLATFORM_SIZE);
	}

	for (int i = 0; i < EMIT_PLATFORM_SIZE; i++) {
		for (int j = 0; j < EMIT_PLATFORM_SIZE; j++) {
			vec_source_radiation[i][j] = 0;
		}
	}

	// Перерисовка.
	DrawImage(vec_source_radiation, PicDc, Pic);

	// Удаление таймера для перерисовки 3D.
	if (bTimer) {
		KillTimer(1);
		bTimer = false;
	}
}


void CDirectionalDiagramDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);
	CRect rect_emitter;
	CWnd* pWnd_emitter = GetDlgItem(IDC_PICTURE_ADD_EMITTER);
	pWnd_emitter->GetWindowRect(&rect_emitter);
	ScreenToClient(&rect_emitter);
	// Если левая кнопка мыши была отпущена в области задания излучателей.
	if (point.x < rect_emitter.right && point.x > rect_emitter.left &&
		point.y > rect_emitter.top && point.y < rect_emitter.bottom) {
		if (!vec_source_radiation.empty()) {
			// Проверка, что существующий вектор соответствует заданным параметрам.
			if (vec_source_radiation.size() != EMIT_PLATFORM_SIZE &&
				vec_source_radiation[0].size() != EMIT_PLATFORM_SIZE) {
				OnBnClickedButtonClear();
			}

			// Получение количество ячеек.
			int cells_count_x = vec_source_radiation.size();
			int cells_count_y = vec_source_radiation[0].size();

			// Получение диапазона для каждой ячейки.
			double step_x = (double)rect_emitter.Width() / (double)cells_count_x;
			double step_y = (double)rect_emitter.Height() / (double)cells_count_y;

			// Получение индексов, в которые нужно добавить источник.
			int emitter_x_idx = (point.x - rect_emitter.left) / step_x;
			int emitter_y_idx = (point.y - rect_emitter.top) / step_y;

			// Поставить/убрать источник.
			if (vec_source_radiation[emitter_x_idx][vec_source_radiation[0].size() - 1 - emitter_y_idx] == 0) {
				vec_source_radiation[emitter_x_idx][vec_source_radiation[0].size() - 1 - emitter_y_idx] = 1;
			}
			else {
				vec_source_radiation[emitter_x_idx][vec_source_radiation[0].size() - 1 - emitter_y_idx] = 0;
			}

			// Перерисовка.
			DrawImage(vec_source_radiation, PicDc, Pic);
		}
	}
	else {
		CRect rect_3d;
		CWnd* pWnd_3d = GetDlgItem(IDC_PICTURE_3D_DIAGRAM);
		pWnd_3d->GetWindowRect(&rect_3d);
		ScreenToClient(&rect_3d);
		// Если левая кнопка мыши была отпущена в области 3d графика.
		if (point.x < rect_3d.right && point.x > rect_3d.left &&
			point.y > rect_3d.top && point.y < rect_3d.bottom) {
			move_mode = false;
		}
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDirectionalDiagramDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnBnClickedButtonClear();

	CDialogEx::OnRButtonUp(nFlags, point);
}


void CDirectionalDiagramDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect_3d;
	CWnd* pWnd_3d = GetDlgItem(IDC_PICTURE_3D_DIAGRAM);
	pWnd_3d->GetWindowRect(&rect_3d);
	ScreenToClient(&rect_3d);
	// Если левая кнопка мыши была отпущена в области 3d графика.
	if (point.x < rect_3d.right && point.x > rect_3d.left &&
		point.y > rect_3d.top && point.y < rect_3d.bottom) {
		move_mode = true;
		start_move_point.x = point.x;
		start_move_point.y = point.y;
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDirectionalDiagramDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect_3d;
	CWnd* pWnd_3d = GetDlgItem(IDC_PICTURE_3D_DIAGRAM);
	pWnd_3d->GetWindowRect(&rect_3d);
	ScreenToClient(&rect_3d);
	// Если левая кнопка мыши была отпущена в области 3d графика.
	if (point.x < rect_3d.right && point.x > rect_3d.left &&
		point.y > rect_3d.top && point.y < rect_3d.bottom) {
		if (move_mode)
		{
			if (start_move_point.x < point.x)
			{
				spin_z -= (double)(point.x - start_move_point.x) / 5;
			}
			else if (start_move_point.x > point.x)
			{
				spin_z += (double)(start_move_point.x - point.x) / 5;
			}

			if (start_move_point.y < point.y)
			{
				spin_x += (double)(point.y - start_move_point.y) / 5;
			}
			else if (start_move_point.y > point.y)
			{
				spin_x -= (double)(start_move_point.y - point.y) / 5;
			}

			start_move_point.x = point.x;
			start_move_point.y = point.y;
		}
	}
	else {
		move_mode = false;
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CDirectionalDiagramDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect_3d;
	CWnd* pWnd_3d = GetDlgItem(IDC_PICTURE_3D_DIAGRAM);
	pWnd_3d->GetWindowRect(&rect_3d);
	ScreenToClient(&rect_3d);
	ScreenToClient(&pt);
	// Если левая кнопка мыши была отпущена в области 3d графика.
	if (pt.x < rect_3d.right && pt.x > rect_3d.left &&
		pt.y > rect_3d.top && pt.y < rect_3d.bottom) {
		CRect rect;
		GetDlgItem(IDC_PICTURE_3D_DIAGRAM)->GetWindowRect(&rect);

		if (pt.x <= rect.right || pt.x >= rect.left && pt.y <= rect.top || pt.y >= rect.bottom)
		{
			if (zDelta > 0)
			{
				zoom -= 40.;
			}
			else if (zDelta < 0)
			{
				zoom += 40.;
			}
		}
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CDirectionalDiagramDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		DrawGL(vec_diagram);
	}

	CDialogEx::OnTimer(nIDEvent);
}
