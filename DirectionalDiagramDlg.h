#pragma once
#include <vector>
#include <complex>
#include <GL/freeglut.h>

using namespace std;

struct Pixel {
	double value = 0.0;
	int color = 0;
};

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
	/// Глобальные переменные
	/////////////////////////////////////

	// Максимальный размер платформ, ячейки.
	complex<double> i_unit = complex<double>(0., 1.);
	int MAX_EMIT_PLATFORM_SIZE = 70;
	int MAX_SPHERE_PLATFORM_SIZE = 800;
	CWnd* PicWnd;
	CDC* PicDc;
	CRect Pic;
	CDC* pDC_3d;
	double emitters_xp, emitters_yp, emitters_xmin, emitters_xmax, emitters_ymin, emitters_ymax;
	vector<vector<int>> vec_source_radiation;
	vector<vector<complex<double>>> vec_diagram;
	vector<vector<Pixel>> vec_diagram_2d;

	bool bTimer = false;

	CWnd* Pic2dWnd;
	CDC* Pic2dDc;
	CRect Pic2d;
	double diagram_2d_xp, diagram_2d_yp, diagram_2d_xmin, diagram_2d_xmax, diagram_2d_ymin, diagram_2d_ymax;

	// OpenGL
	double spin_x, spin_y, spin_z, zoom;
	GLuint base;
	bool firsttimedraw = true;
	double max, min;
	double maxmin;
	double absmax;
	bool move_mode = false;
	CPoint start_move_point;

	/////////////////////////////////////
	/// Переменные, связанные с интерфейсом
	/////////////////////////////////////

	double EMIT_DISTANCE;
	double EMIT_WAVE_LENGTH;
	int EMIT_PLATFORM_SIZE;
	double SPHERE_RADIUS;
	int GRID_STEP_3D;
	int SPHERE_PLATFORM_SIZE;
	CButton IS_DRAW_AXIS_3D;
	CButton IS_DRAW_GRID_3D;
	CButton IS_DRAW_TEXT_3D;
	CButton RADIO_DRAW_REAL;
	CButton RADIO_DRAW_IMAGE;
	CButton RADIO_DRAW_MODULE;

	/////////////////////////////////////
	/// Функции
	/////////////////////////////////////
	
	// OpenGL
	BOOL bSetupPixelFormat();
	GLvoid BuildFont(GLvoid);
	GLvoid KillFont(GLvoid);
	GLvoid glPrint(const char* fmt, ...);
	void InitiateOPGL();
	void DrawGL(vector<vector<complex<double>>> diagram);
	double findmax(vector<vector<complex<double>>> D, bool max_min);

	void DrawImage(vector<vector<int>> vec, CDC* WinDc, CRect WinxmaxGraphc);
	vector<vector<Pixel>> calculate_color_of_image(vector<vector<Pixel>> image_vec);
	int get_color_from_value(Pixel vec, double min_value, double max_value);
	void draw_image(vector<vector<Pixel>> image);
	void CreateSphere(vector<vector<double>>& sphere);
	double rast(int x1, int y1, int z1, int x2, int y2);

	/////////////////////////////////////
	/// Обработчики кнопок
	/////////////////////////////////////

	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonCalculate();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
